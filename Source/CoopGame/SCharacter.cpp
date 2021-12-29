// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
	AddMovementInput(GetActorForwardVector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	/*GetActorRightVector() 是在世界空间中从此角色获取右向的向量，，它获取的是 Y 轴的向量。
	 FVector USceneComponent::GetRightVector() const
     {
	    return GetComponentTransform().GetUnitAxis( EAxis::Y );
     }
	*/
	AddMovementInput(GetActorRightVector(), Value);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
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

}

