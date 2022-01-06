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
	SpringArmComp->bUsePawnControlRotation = true;//使用Pawn的旋转控制相机的移动
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
	 * WorldDirection	使用世界方向向量表示移动方向
     * ScaleValue	通常使用鼠标位移表示
     * bForce	如果为真，表示总是使用此输入，忽略AController的IgnoreMoveInput作用
	 * 输入数据后续由UCharacterMovementComponent的下一次TickComponent用到
	*/

	/*GetActorForwardVector() 是在世界空间中从此角色获取前向的向量，它获取的是 X 轴的向量
	FVector USceneComponent::GetForwardVector() const
	{
		return GetComponentTransform().GetUnitAxis(EAxis::X);
	}
	*/
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	/*GetActorRightVector() 是在世界空间中从此角色获取右向的向量，，它获取的是 Y 轴的向量。
	 FVector USceneComponent::GetRightVector() const
     {
	    return GetComponentTransform().GetUnitAxis( EAxis::Y );
     }
	*/
	AddMovementInput(GetActorRightVector() * Value);
}

void  ASCharacter::BeginCrouch()
{
	//调用Character中提供的Crouch方法，需要在Character中启用Crouch 
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

	/*FMath::FInterpTo 根据时间、速度进行插值， 可以使得镜头平缓的拉近
	Current：当前值
	Target：期望的目标值
	Delta Time：时间变化值。
	Interp Speed：插值速度
	返回值：从“当前值”过渡到“期望的目标值”的一个中间值
	*/
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//注意：视频P45 045 Add Movement Input中把Player Start删除，然后直接把SCharacter的蓝图资源拖入关卡；
	// 这时必须设置SCharacter蓝图中的Auto Possess Player为Player 0；如果没设置，input不会操控角色，而是整个视口
	
	/**BindAxis
	* BindAxis 第一个参数为 虚幻4->项目设置 ->输入->按键绑定的名称(Axis Mappins)  一定要一样 ！！！
	* 参数二 当前对象
	* 参数三 事件方法的函数指针 例 &类名::方法名 的写法
	* 会在内部用this调用注册的方法
	*/
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	/*FRotator
     Yaw 表示 摇头 就是绕 Z 移动；
     Pich 表示 点头 就是绕 Y 移动；
     Roll 你可以想象成左右晃脑，绕 X轴运动。
	*/
	//控制视角: AddControllerYawInput() 和 AddControllerPitchInput() 是 UE4 已经为我们封装好的移动视角的函数
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

//重写GetActorEyesViewPoint方法，使得射线检测的起点为相机所在位置
FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}
