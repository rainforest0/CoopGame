// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

// Contains information of a single hitscan weapon linetrace���ں��������ɨ���������Ĺ켣����Ϣ��
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	void PlayFireEffects(FVector TracerEnd);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TraceEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	//error : Class 'UCameraShake' not found.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UMatineeCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	void Fire();

	/*�淶1��RPC����ǰ׺
	      �ͻ��ˡ�������������ಥRPC��������ʹ�ø���ǰ׺Client_��Server_��Multicast_��
     �淶2��RepNotify����ǰ׺
          �ͻ���RepNotify��������ʹ��OnRep_ǰ׺��
     �淶3��ʹ��Authority�жϺͶ���
         �жϾͲ�׸���ˣ�ǰ���кܶ�HasAuthority()��IsLocallyControlled()���жϣ����濪��������Ҳ���ᵽ��
		 ����:�ʵ�ʹ�ö���(assertion)������ʹ�ô���������Ҳ����׳���
             checkf(HasAuthority(), TEXT("��δ���Ӧ���ڷ�����������"));
             checkf(!HasAuthority(), TEXT("��δ���Ӧ���ڿͻ���������"));
             checkf(IsLocallyControlled(), TEXT("��δ���Ӧ���ڱ��ؿ��Ƶ�Pawn������"));
	�淶4��Ȩ������ǰ׺
         ͬʱ����Ϊ��������������������������Ϊ����Ȩ���������еķ������������"Auth"ǰ׺������Server_ǰ׺��ͬ���˷�����RPC��������Ȩ�����߼���
         if (HasAuthority())
         {
	        LastAuthHitTime = GetWorld()->GetTimeSeconds();
	       AuthRegisterHit();
         }
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	//RPM:����ÿ���ӷ����ӵ�����
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RatOfFire;

	//������RatOfFire
	float TimeBetweenShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();
public:	

	void StartFire();

	void StopFire();

};
