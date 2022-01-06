// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;//ʹ��Pawn����ת����������ƶ�
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISON_WEAPON, ECR_Ignore);
	
	DefaultFOV = CameraComp->FieldOfView;
	bWantsToZoom = false;
	
	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Spawn a default weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}

void ASCharacter::MoveForward(float Value)
{
	/*AddMovementInput() 
	 * WorldDirection	ʹ�����緽��������ʾ�ƶ�����
     * ScaleValue	ͨ��ʹ�����λ�Ʊ�ʾ
     * bForce	���Ϊ�棬��ʾ����ʹ�ô����룬����AController��IgnoreMoveInput����
	 * �������ݺ�����UCharacterMovementComponent����һ��TickComponent�õ�
	*/

	/*GetActorForwardVector() ��������ռ��дӴ˽�ɫ��ȡǰ�������������ȡ���� X �������
	FVector USceneComponent::GetForwardVector() const
	{
		return GetComponentTransform().GetUnitAxis(EAxis::X);
	}
	*/
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	/*GetActorRightVector() ��������ռ��дӴ˽�ɫ��ȡ�����������������ȡ���� Y ���������
	 FVector USceneComponent::GetRightVector() const
     {
	    return GetComponentTransform().GetUnitAxis( EAxis::Y );
     }
	*/
	AddMovementInput(GetActorRightVector() * Value);
}

void  ASCharacter::BeginCrouch()
{
	//����Character���ṩ��Crouch��������Ҫ��Character������Crouch 
	Crouch();
}

void  ASCharacter::EndCrouch()
{
	UnCrouch();
}


void  ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void  ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void  ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void  ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	/*FMath::FInterpTo ����ʱ�䡢�ٶȽ��в�ֵ�� ����ʹ�þ�ͷƽ��������
	Current����ǰֵ
	Target��������Ŀ��ֵ
	Delta Time��ʱ��仯ֵ��
	Interp Speed����ֵ�ٶ�
	����ֵ���ӡ���ǰֵ�����ɵ���������Ŀ��ֵ����һ���м�ֵ
	*/
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//ע�⣺��ƵP45 045 Add Movement Input�а�Player Startɾ����Ȼ��ֱ�Ӱ�SCharacter����ͼ��Դ����ؿ���
	// ��ʱ��������SCharacter��ͼ�е�Auto Possess PlayerΪPlayer 0�����û���ã�input����ٿؽ�ɫ�����������ӿ�
	
	/**BindAxis
	* BindAxis ��һ������Ϊ ���4->��Ŀ���� ->����->�����󶨵�����(Axis Mappins)  һ��Ҫһ�� ������
	* ������ ��ǰ����
	* ������ �¼������ĺ���ָ�� �� &����::������ ��д��
	* �����ڲ���this����ע��ķ���
	*/
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	/*FRotator
     Yaw ��ʾ ҡͷ ������ Z �ƶ���
     Pich ��ʾ ��ͷ ������ Y �ƶ���
     Roll �������������һ��ԣ��� X���˶���
	*/
	//�����ӽ�: AddControllerYawInput() �� AddControllerPitchInput() �� UE4 �Ѿ�Ϊ���Ƿ�װ�õ��ƶ��ӽǵĺ���
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed,  this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJumping);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
}

//��дGetActorEyesViewPoint������ʹ�����߼������Ϊ�������λ��
FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}
