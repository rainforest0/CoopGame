// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASpowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	//Ӧ����ǿ�͵���Ч�����ܴ���
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int TotalNumberOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	// Total number of ticks applied
	int32 TicksProcessed;

	bool bIsPowerupActive;

	UFUNCTION()
	void OnTickPowerup();

public:	
	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();
};