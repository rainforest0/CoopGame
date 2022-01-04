// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

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

		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			//�����˺�
			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			//����ʱЧ��
			if (ImpactEffect)
			{
				//Plays the specified effect at the given location and rotation, fire and forget. The system will go away when the effect is complete. Does not replicate
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
			
			TracerEndPoint = Hit.ImpactPoint;
		}

		//�Ƿ�Ҫ���õĴ���
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
		
		PlayFireEffects(TracerEndPoint);
		
	}
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
}


