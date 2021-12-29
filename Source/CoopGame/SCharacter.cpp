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
	AddMovementInput(GetActorForwardVector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	/*GetActorRightVector() ��������ռ��дӴ˽�ɫ��ȡ�����������������ȡ���� Y ���������
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

}

