// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SpowerupActor.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(RootComponent);

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		Respawn();
	}
}

void ASPickupActor::Respawn()
{
	if (PowerClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr in %s. Please update your Blueprint"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("PowerUpClass %s Respawn"), *GetName());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<ASpowerupActor>(PowerClass, GetTransform(), SpawnParams);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority() && PowerupInstance)
	{
		PowerupInstance->ActivatePowerup();
		PowerupInstance = nullptr;

		// Set Timer to respawn powerup
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, CooldownDuration);
	}
}


