// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DefaultHealth = 100;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = DefaultHealth;

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}
	
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0)
	{
		return;
	}

	//��Ѫ��������[0.0f, DefaultHealth]֮��
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	/*�ַ���
     �������4֧�����ֺ������͵��ַ�����
	   FString�ǵ��͵�"��̬�ַ�����"�ַ������͡�
       FName�Ƕ�ȫ���ַ������в��ɱ��Ҳ����ִ�Сд���ַ��������á������FString�����Ĵ�С��С�����ܸ�Ч�Ĵ��ݣ��������Բٿء�
       FText��ָ�����ڴ����ػ��ĸ��ɿ����ַ�����ʾ��
	���ڴ��������£��������TCHAR��������ʾ�ַ���TEXT()������ڱ�ʾTCHAR���֡�
	  MyDogPtr->DogName = FName(TEXT("Samson Aloysius"));
    ԭ�����ӣ�https://docs.unrealengine.com/4.26/zh-CN/ProgrammingAndScripting/GameplayArchitecture/Properties/

	* (1)FString�� FName �� FText ��ͬ��FString ��Ψһ����������ַ����ࡣ�ַ��������Ŀ��÷�������࣬���Сдת����ժ¼���ַ���������FStrings �ɱ��Ѽ����޸ġ����������ַ������жԱȡ�
		Ȼ����Щ��������ʹ FString �Ŀ����Ȳ��ɱ��ַ��������
		SanitizeFloat: //float -> FString

	* (2)FName ͨ��һ������ϵͳʹ���ַ������ڴ�ϵͳ�У��ض��ַ�����ʹ�ᱻ�ظ�ʹ�ã������ݱ���Ҳֻ�洢һ�Ρ�FNames �����ִ�Сд������Ϊ���ɱ䣬�޷���������
	     FNames �Ĵ洢ϵͳ�;�̬���Ծ�����ͨ�������� FNames �Ĳ��Һͷ����ٶȽϿ졣FName ��ϵͳ����һ��������ʹ��ɢ�б�Ϊ FName ת���ṩ�����ַ�����
     
	* (3)FText ����һ����ʾ�ַ�����Ϊ�û���ʾ���ı�����Ҫ�� FText ���д���FText ��ӵ�б��ػ�������֧�֣��ɴ����ѱ��ػ����洢�ڲ��ұ��е��ı����ݣ�
	    �Լ�������ʱ�����ػ����ı��������֡����ڡ�ʱ��͸�ʽ���ı����������� FText ����������б��ػ����ı���
		�����û��������ݣ���������� Slate ��ʾ���ı���FText ���ṩ�κα��캯������Ϊ����ʾ�ַ��������޸��Ƿǳ�����ȫ�Ĳ�����
	ԭ�����ӣ�https://blog.csdn.net/u012793104/article/details/78470388
	*/
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}


