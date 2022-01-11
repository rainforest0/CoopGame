// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

//控制台变量可以控制游戏开发版中各项Debug信息
static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"), //控制台要输入的名称：按~ 后，输入 COOP.DebugWeapons 1 ，那么会设置DebugWeaponDrawing值
	DebugWeaponDrawing,  //要控制修改的变量
	TEXT("Draw Debug Lines for Weapons"),//说明
	ECVF_Cheat);//控制台变量类型，ECVF_Cheat代表在最后发布游戏的时候，命令行不会显示

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	RatOfFire = 600;

	/*Actor复制  ：C++: AActor::SetReplicates(true)；            Blueprint:属性面板中设置Replicates；
	Component复制：C++: AActorComponent::SetIsReplicated(true)； Blueprint:调用SetIsReplicated节点；
	*/
	//这意味着经系统内部处理，服务器生成武器的同时，也能让客户端上生成武器
	SetReplicates(true);

	/*Actor的同步频率NetUpdateFrequency：上次同步间隔小于该频率，在该帧不进行同步
	* Actor的同步频率支持动态调整，但始终处于最大（NetUpdateFrequency）和最小（MinNetUpdateFrequency）同步频率之间；
	*/
	NetUpdateFrequency = 66.0f; //默认值：NetUpdateFrequency = 100.0f;
	MinNetUpdateFrequency = 33.0f;//默认值：MinNetUpdateFrequency = 2.0f;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots =  60 / RatOfFire;
}

void ASWeapon::Fire()
{
	//视频教程这里的条件是if (Role < ROLE_Authority)
	//检查Role不为ROLE_Authority，也就是在客户端时才执行
	if (!HasAuthority())
	{
		ServerFire();
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		//注意，SCharacter中已经重写GetActorEyesViewPoint方法（调用的GetPawnViewLocation），使得射线检测的起点为相机所在位置
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		/*ue4射线查询主要使用LineTraceSingleByChannel和LineTraceSingleByObjectType函数进行;
		LineTraceSingleByChannel是通过Channel进行查询;
		LineTraceSingleByObjectType通过ObjectType进行查询;
		要理解这两个函数的区别，首先必须理解Channel和ObjectType的区别: 此处的Channel指的是ECollisionChannel（可参考源码），而ObjectType指的是碰撞中的Object Type设置
		*/
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISON_WEAPON, QueryParams))
		{
			//处理伤害
			AActor* HitActor = Hit.GetActor();
			
			float ActualDamage = BaseDamage;

			//命中SURFACE_FLESHVULNERABLE部位（目前设定为头部，在character->mesh->Physics中设置物理材质），实际伤害增加一定的倍数
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);
			
			TracerEndPoint = Hit.ImpactPoint;
		}

		//是否要启用的代码
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
		
		PlayFireEffects(TracerEndPoint);

		if (HasAuthority())
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}
		
		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic FX(播放外观效果)
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

//返回false时，原生调用这一函数的客户端会从服务器断开连接
bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::StartFire()
{
	//避免“通过快速点击鼠标按键，实现轰炸式发射”，方法：增加对上一次开火的检查，反馈在以下FirstDelay参数上；
	//    否则，如果连续开火比按自动开火更快的话，那谁还会慢慢地等待自动开火。
	//第一次延迟=上次开火时间+射击时间间隔-当前游戏时间
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireEffects(FVector TracerEnd)
{
	//射击时火焰效果
	if (MuzzleEffect)
	{
		//Plays the specified effect attached to and following the specified component. The system will go away when the effect is complete. Does not replicate.
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	if (TraceEffect)
	{
		UParticleSystemComponent* TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
		if (TraceComp)
		{
			TraceComp->SetVectorParameter(TracerTargetName, TracerEnd);
		}
	}

	APawn* MyOwn = Cast<APawn>(GetOwner());
	if (MyOwn)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwn->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
	}

	//击中时效果
	if (SelectedEffect)
	{
		//MuzzleSocketName：枪口套接字的名字；GetSocketLocation获取是枪口位置
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		/*(1)FVector::Rotation(): Returns FRotator from the Vector's direction
		Return the FRotator orientation corresponding to the direction in which the vector points. Sets Yaw and Pitch to the proper numbers, 
		       and sets Roll to zero because the roll can't be determined from a vector.
        (2)SpawnEmitterAtLocation():
		   Plays the specified effect at the given location and rotation, fire and forget. The system will go away when the effect is complete. Does not replicate
		*/
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}
