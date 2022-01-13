// Fill out your copyright notice in the Description page of Project Settings.


#include "SpowerupActor.h"

// Sets default values
ASpowerupActor::ASpowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0;

	bIsPowerupActive = false;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASpowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	UE_LOG(LogTemp, Log, TEXT("OnTickPowerup %d"), TicksProcessed);

	if (TicksProcessed >= TotalNumberOfTicks)
	{
		OnExpired();

		bIsPowerupActive = false;

		// Delete timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}


void ASpowerupActor::ActivatePowerup()
{
	OnActivated();

	bIsPowerupActive = true;

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASpowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
	
}