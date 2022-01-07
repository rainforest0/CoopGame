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
	（1）属性说明符：
	  UPROPERTY([specifier, specifier, ...], [meta(key=value, key=value, ...)])
      Type VariableName;
	  声明属性时，属性说明符 可被添加到声明，以控制属性与引擎和编辑器诸多方面的相处方式，例如：
	    VisibleAnywhere：说明此属性在所有属性窗口中可见，但无法被编辑（Indicates that this property is visible in all property windows, but cannot be edited）。此说明符与"Edit"说明符不兼容。
	    BlueprintReadOnly：此属性可由蓝图读取，但不能被修改（This property can be read by Blueprints, but not modified）。此说明符与 BlueprintReadWrite 说明符不兼容。
	    EditDefaultsOnly：说明此属性可通过属性窗口进行编辑，但只能在原型上进行（Indicates that this property can be edited by property windows, but only on archetypes）。此说明符与所有"可见"说明符均不兼容。
	    VisibleDefaultsOnly：说明此属性只在原型的属性窗口中可见，无法被编辑（Indicates that this property is only visible in property windows for archetypes, and cannot be edited）。此说明符与所有"Edit"说明符均不兼容。
		EditDefaultsOnly：说明此属性可通过属性窗口进行编辑，但只能在原型上进行（Indicates that this property can be edited by property windows, but only on archetypes）。此说明符与所有"可见"说明符均不兼容。
		EditAnywhere：说明此属性可通过属性窗口在原型和实例上进行编辑（Indicates that this property can be edited by property windows, on archetypes and instances）。此说明符与所有"可见"说明符均不兼容。
	（2）元数据说明符
     声明类、接口、结构体、列举、列举值、函数，或属性时，可添加 元数据说明符 来控制其与引擎和编辑器各方面的相处方式。每一种类型的数据结构或成员都有自己的元数据说明符列表，例如：
        ClampMin="N" 用于浮点和整数属性。指定可在属性中输入的最小值 N。
	    ClampMax="N" 用于浮点和整数属性。指定可在属性中输入的最大值 N。
	* 详情参考：https://docs.unrealengine.com/4.26/zh-CN/ProgrammingAndScripting/GameplayArchitecture/Properties/
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
