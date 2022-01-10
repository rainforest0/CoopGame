// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequireDistanceToTarget = 100;

	ExplosionDamage = 60;
	ExplosionRadius = 350;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	/*UE4 SetVisibility()��SetHiddenInGame()�ıȽ�
     ��������ϵ��SetVility()ʵ�ֵĸ��ӹ㷺һЩ����SetHiddenInGame()����ֻ��SceneComponent����ʵ�֣���ζ��SetHiddenInGame()ֻ������SceneComponent��
	             SetVisibility()�������ذ���SceneComponent���ڵĺܶණ��(��UI���)��һ����˵���ڳ�������ʾ(���ļ���)���壬����SceneComponent,���ַ��������ԴﵽĿ�ģ�
				                ���������һЩSWeiget��SPanel��UWeiget�Ⱦ�ֻ��ͨ��SetVisibility()������
     ���⣬��SceneComponent�����ַ�����ʵ�ּ�������ͬ�ġ�

	 ����bool bPropagateToChildre���Ƿ�����Ӷ������������Ӷ���ɼ��ԣ�
	*/
	MeshComp->SetVisibility(false, true);

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	/*��1��ApplyRadialDamage���������������η�Χ�˺����˺��������ѡȡ���η�Χ�ڿɱ��˺������壬�����˺�����;
	* ���⣬�˺������������ڵ��˺������⣬���磬�˺���Χ�ڰ���һ�����ӣ����������������Ӻ�����������Ӻ���˲������˺�����ô�����أ��и�����ΪECollisionChannel DamagePreventionChannel��
	        �˲���һ�����־�֪�������������������ģ������ڲ���ӱ�ը�㵽���˺������Component�����߼�⣬��⺯��ΪLineTraceSingleByChannel��
			����DamagePreventionChannel����ΪECC_Visibility���Ƿ�Χ�ڵ�����Component��ECC_Visibility Channel����Ϊblock�ģ������赲�˺����˲������ò��������ܻ���ַ�Χ����Щ���岻���˺������⣻
     ��2��INSTIGATOR��GetInstigatorController()
          Instigator�ǵ����˺��ķ����ߣ���ͨ����PlayerController����AIController�����磬�ڻ��ֵ�����£��Ϳ�����player ���� Ai��ȼ�Ļ�
	*/
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

	Destroy();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (NULL == MatInst)
	{
		/*CreateAndSetMaterialInstanceDynamicFromMaterial(int32 ElementIndex, class UMaterialInterface * Parent)
		Creates a Dynamic Material Instance for the specified element index. The parent of the instance is set to the material being replaced.
		����һ�����ʶ��󣬲��滻��Parent�����ϵ�ָ��index�Ĳ���
		*/
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		//���ñ�������ֵ
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0)
	{
	//	SelfDestruct();
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());
}

FVector ASTrackerBot::GetNextPathPoint()
{
	//���Ի�ȡ���λ��
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	//�����еĵ�һ��·����������ĵ�ǰ����λ�ã�����Ҫ֪����һ��λ��������ĵڶ���Ԫ��
	if (NavPath->PathPoints.Num() > 1)
	{
		//����·���е���һ����
		return NavPath->PathPoints[1];
	}

	//����Actorλ�ñ�ʾ δ�ҵ�·��
	return GetActorLocation();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget =(GetActorLocation() - NextPathPoint).Size();
	if (DistanceToTarget < RequireDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();

		DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
	}
	else
	{
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();

		ForceDirection *= MovementForce;

		/*��������������
          �����ı������˶�״̬��AddForce��
         ���أ�������������ʱ��������ת��ЧӦ��AddTorqueInRadians��AddTorque�����ѱ�������

		AddForce(FVctor Force,FName BoneName,bool bAccolChange) �������壨������ģ�⣩ʩ���������ã�����Ϊʸ��FVector��
		          ForceΪ����ӵ���(����������ʩ��һ����ʱ������ᱻʩ�ӵ����ƶ�;ֻҪ�����ܵ���Ӱ��Ķ������AddForce���������ܶ���ʩ��һ���������Կ�����һ��������һ���������������ķ���)��
		          BoneName��ָ��ǰ�������õĹ�����Ĭ��ΪNAME_None, 
				  bAccolChange��ָ��ǰ���Ƿ�ֱ������Ϊ���ٶȣ�������������������Ļ��ڣ�,Ĭ��Ϊfalse.
		*/
		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
}


