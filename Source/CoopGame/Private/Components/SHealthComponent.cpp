// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100;

	/*
    Components(���)
    ����componet�ǲ����Ƶ�, ��Ϊ�����߼�����actor����ɵ�, ֻ��Ҫͬ��compoent��صĵ��ü���, ����component������Ҫֱ�ӽ������Ժ��¼��ĸ���.
    ������Ʒ�����:
        ��̬���(actor����ʱ������)����spawn actorʱ�Զ�������
        ��̬���(����������ʱ������)����actor����һ��������ɾ����
    AActorComponent::SetIsReplicated(true)
    */
	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = DefaultHealth;
     
	//�������ڷ�����ʱ�����������˺��¼���
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
}

//����ʱ���ᱣ����һ��ֵ
void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	//�ÿͻ���Ҳ��������ֵ�ı仯
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
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

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health+HealAmount, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

float USHealthComponent::GetHealth() const
{
	return Health;
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}
