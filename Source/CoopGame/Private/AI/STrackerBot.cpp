// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequireDistanceToTarget = 100;

	ExplosionDamage = 60;
	ExplosionRadius = 350;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	/*UE4 SetVisibility()和SetHiddenInGame()的比较
     区别与联系：SetVility()实现的更加广泛一些，而SetHiddenInGame()则是只在SceneComponent中有实现，意味着SetHiddenInGame()只能隐藏SceneComponent。
	             SetVisibility()可以隐藏包括SceneComponent在内的很多东西(如UI组件)。一般来说能在场景中显示(看的见的)物体，都有SceneComponent,两种方法都可以达到目的，
				                但是如果是一些SWeiget，SPanel，UWeiget等就只能通过SetVisibility()来做。
     另外，在SceneComponent中两种方法的实现几乎是相同的。

	 参数bool bPropagateToChildre：是否遍历子对象（用来设置子对象可见性）
	*/
	MeshComp->SetVisibility(false, true);

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	/*（1）ApplyRadialDamage函数用来处理球形范围伤害，此函数大概是选取球形范围内可被伤害的物体，进行伤害处理;
	* 另外，此函数还处理了遮挡伤害的问题，比如，伤害范围内包含一根柱子，假设人正好在柱子后，如果想让柱子后的人不承受伤害该怎么处理呢，有个参数为ECollisionChannel DamagePreventionChannel，
	        此参数一看名字就知道是用来处理此种情况的；函数内部会从爆炸点到被伤害物体的Component做射线检测，检测函数为LineTraceSingleByChannel；
			假设DamagePreventionChannel设置为ECC_Visibility，那范围内的所有Component中ECC_Visibility Channel设置为block的，都会阻挡伤害，此参数设置不当，可能会出现范围内有些物体不被伤害的问题；
     （2）INSTIGATOR：GetInstigatorController()
          Instigator是导致伤害的发起者，它通常是PlayerController或者AIController。比如，在火灾的情况下，就可能是player 或者 Ai点燃的火。
	*/
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

	Destroy();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (NULL == MatInst)
	{
		/*CreateAndSetMaterialInstanceDynamicFromMaterial(int32 ElementIndex, class UMaterialInterface * Parent)
		Creates a Dynamic Material Instance for the specified element index. The parent of the instance is set to the material being replaced.
		创建一个材质对象，并替换到Parent对象上的指定index的材质
		*/
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		//设置标量参数值
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0)
	{
	//	SelfDestruct();
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());
}

FVector ASTrackerBot::GetNextPathPoint()
{
	//尝试获取玩家位置
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	//数组中的第一个路径点是球体的当前所在位置，所以要知道下一个位置是数组的第二个元素
	if (NavPath->PathPoints.Num() > 1)
	{
		//返回路径中的下一个点
		return NavPath->PathPoints[1];
	}

	//返回Actor位置表示 未找到路径
	return GetActorLocation();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget =(GetActorLocation() - NextPathPoint).Size();
	if (DistanceToTarget < RequireDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();

		DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
	}
	else
	{
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();

		ForceDirection *= MovementForce;

		/*给对象力和力矩
          力：改变物体运动状态（AddForce）
         力矩：力对物体作用时所产生的转动效应（AddTorqueInRadians，AddTorque函数已被废弃）

		AddForce(FVctor Force,FName BoneName,bool bAccolChange) ：给物体（打开物理模拟）施加力的作用，参数为矢量FVector，
		          Force为其添加的力(力：给对象施加一个力时，对象会被施加的力推动;只要让想受到力影响的对象调用AddForce函数，就能对其施加一个力，可以看到第一个参数是一个向量，就是力的方向)，
		          BoneName是指当前里所作用的骨骼，默认为NAME_None, 
				  bAccolChange是指当前力是否直接作用为加速度（跳过与质量进行运算的环节）,默认为false.
		*/
		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
}


