// Fill out your copyright notice in the Description page of Project Settings.


#include "SpowerupActor.h"
#include "Net/UnrealNetwork.h"

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

		//服务器需要主动调用，客户端走的是复制
		OnPowerupStateChanged(bIsPowerupActive);

		// Delete timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void  ASpowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

//void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)函数中已经保证ActivatePowerup()只会在服务器调用
void ASpowerupActor::ActivatePowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);

	bIsPowerupActive = true;

	//服务器需要主动调用，客户端走的是复制
	OnPowerupStateChanged(bIsPowerupActive);

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASpowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
	
}

void ASpowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpowerupActor, bIsPowerupActive);
}