// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void BeginZoom();

	void EndZoom();

	void StartFire();

	void StopFire();

	/*UPROPERTY
	��1������˵������
	  UPROPERTY([specifier, specifier, ...], [meta(key=value, key=value, ...)])
      Type VariableName;
	  ��������ʱ������˵���� �ɱ���ӵ��������Կ�������������ͱ༭����෽����ദ��ʽ�����磺
	    VisibleAnywhere��˵�����������������Դ����пɼ������޷����༭��Indicates that this property is visible in all property windows, but cannot be edited������˵������"Edit"˵���������ݡ�
	    BlueprintReadOnly�������Կ�����ͼ��ȡ�������ܱ��޸ģ�This property can be read by Blueprints, but not modified������˵������ BlueprintReadWrite ˵���������ݡ�
	    EditDefaultsOnly��˵�������Կ�ͨ�����Դ��ڽ��б༭����ֻ����ԭ���Ͻ��У�Indicates that this property can be edited by property windows, but only on archetypes������˵����������"�ɼ�"˵�����������ݡ�
	    VisibleDefaultsOnly��˵��������ֻ��ԭ�͵����Դ����пɼ����޷����༭��Indicates that this property is only visible in property windows for archetypes, and cannot be edited������˵����������"Edit"˵�����������ݡ�
		EditDefaultsOnly��˵�������Կ�ͨ�����Դ��ڽ��б༭����ֻ����ԭ���Ͻ��У�Indicates that this property can be edited by property windows, but only on archetypes������˵����������"�ɼ�"˵�����������ݡ�
		EditAnywhere��˵�������Կ�ͨ�����Դ�����ԭ�ͺ�ʵ���Ͻ��б༭��Indicates that this property can be edited by property windows, on archetypes and instances������˵����������"�ɼ�"˵�����������ݡ�
	��2��Ԫ����˵����
     �����ࡢ�ӿڡ��ṹ�塢�о١��о�ֵ��������������ʱ������� Ԫ����˵���� ��������������ͱ༭����������ദ��ʽ��ÿһ�����͵����ݽṹ���Ա�����Լ���Ԫ����˵�����б����磺
        ClampMin="N" ���ڸ�����������ԡ�ָ�������������������Сֵ N��
	    ClampMax="N" ���ڸ�����������ԡ�ָ��������������������ֵ N��
	* ����ο���https://docs.unrealengine.com/4.26/zh-CN/ProgrammingAndScripting/GameplayArchitecture/Properties/
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly,  Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin=0.0, ClampMax=100.0))
	float ZoomInterpSpeed;

	bool  bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	ASWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
};
