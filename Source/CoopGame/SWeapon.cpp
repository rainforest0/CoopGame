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

//����̨�������Կ�����Ϸ�������и���Debug��Ϣ
static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"), //����̨Ҫ��������ƣ���~ ������ COOP.DebugWeapons 1 ����ô������DebugWeaponDrawingֵ
	DebugWeaponDrawing,  //Ҫ�����޸ĵı���
	TEXT("Draw Debug Lines for Weapons"),//˵��
	ECVF_Cheat);//����̨�������ͣ�ECVF_Cheat��������󷢲���Ϸ��ʱ�������в�����ʾ

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	RatOfFire = 600;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots =  60 / RatOfFire;
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		//ע�⣬SCharacter���Ѿ���дGetActorEyesViewPoint���������õ�GetPawnViewLocation����ʹ�����߼������Ϊ�������λ��
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISON_WEAPON, QueryParams))
		{
			//�����˺�
			AActor* HitActor = Hit.GetActor();
			
			float ActualDamage = BaseDamage;

			//����SURFACE_FLESHVULNERABLE��λ��Ŀǰ�趨Ϊͷ������character->mesh->Physics�����ã���ʵ���˺�����һ���ı���
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

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

			//����ʱЧ��
			if (SelectedEffect)
			{
				//Plays the specified effect at the given location and rotation, fire and forget. The system will go away when the effect is complete. Does not replicate
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
			
			TracerEndPoint = Hit.ImpactPoint;
		}

		//�Ƿ�Ҫ���õĴ���
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
		
		PlayFireEffects(TracerEndPoint);
		
		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::StartFire()
{
	//���⡰ͨ�����ٵ����갴����ʵ�ֺ�ըʽ���䡱�����������Ӷ���һ�ο���ļ�飬����������FirstDelay�����ϣ�
	//    ���������������Ȱ��Զ��������Ļ�����˭���������صȴ��Զ�����
	//��һ���ӳ�=�ϴο���ʱ��+���ʱ����-��ǰ��Ϸʱ��
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireEffects(FVector TracerEnd)
{
	//���ʱ����Ч��
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


