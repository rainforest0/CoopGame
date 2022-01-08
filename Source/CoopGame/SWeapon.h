// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

// Contains information of a single hitscan weapon linetrace（内含单次射击扫描类武器的轨迹线信息）
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

	/*规范1：RPC方法前缀
	      客户端、服务器或网络多播RPC方法声明使用各自前缀Client_、Server_和Multicast_。
     规范2：RepNotify方法前缀
          客户端RepNotify方法声明使用OnRep_前缀。
     规范3：使用Authority判断和断言
         判断就不赘述了，前面有很多HasAuthority()和IsLocallyControlled()的判断，后面开发流程中也会提到。
		 断言:适当使用断言(assertion)，可以使得代码简单明了且不容易出错。
             checkf(HasAuthority(), TEXT("这段代码应该在服务器中运行"));
             checkf(!HasAuthority(), TEXT("这段代码应该在客户端中运行"));
             checkf(IsLocallyControlled(), TEXT("这段代码应该在本地控制的Pawn中运行"));
	规范4：权威方法前缀
         同时可以为辅助方法建立命名惯例，比如为仅在权威环境运行的方法在命名添加"Auth"前缀。（与Server_前缀不同，此方法非RPC，但属于权威的逻辑）
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

	//RPM:武器每分钟发射子弹数量
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RatOfFire;

	//衍生自RatOfFire
	float TimeBetweenShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();
public:	

	void StartFire();

	void StopFire();

};
