// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100;

	/*
    Components(组件)
    多数componet是不复制的, 因为多数逻辑是在actor中完成的, 只需要同步compoent相关的调用即可, 除非component本身需要直接进行属性和事件的复制.
    组件复制分两种:
        静态组件(actor创建时创建的)会在spawn actor时自动创建；
        动态组件(服务器运行时创建的)会像actor复制一样创建和删除；
    AActorComponent::SetIsReplicated(true)
    */
	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = DefaultHealth;
     
	//仅当是在服务器时，将生命和伤害事件绑定
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
}

//复制时，会保留上一个值
void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	//让客户端也触发生命值的变化
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0)
	{
		return;
	}

	//把血量控制在[0.0f, DefaultHealth]之间
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	/*字符串
     虚幻引擎4支持三种核心类型的字符串。
	   FString是典型的"动态字符数组"字符串类型。
       FName是对全局字符串表中不可变且不区分大小写的字符串的引用。相较于FString，它的大小更小，更能高效的传递，但更难以操控。
       FText是指定用于处理本地化的更可靠的字符串表示。
	对于大多数情况下，虚幻依靠TCHAR类型来表示字符。TEXT()宏可用于表示TCHAR文字。
	  MyDogPtr->DogName = FName(TEXT("Samson Aloysius"));
    原文链接：https://docs.unrealengine.com/4.26/zh-CN/ProgrammingAndScripting/GameplayArchitecture/Properties/

	* (1)FString与 FName 和 FText 不同，FString 是唯一允许操作的字符串类。字符串操作的可用方法有许多，如大小写转换、摘录子字符串和逆向。FStrings 可被搜集、修改、并与其他字符串进行对比。
		然而这些操作可能使 FString 的开销比不可变字符串类更大。
		SanitizeFloat: //float -> FString

	* (2)FName 通过一个轻型系统使用字符串。在此系统中，特定字符串即使会被重复使用，在数据表中也只存储一次。FNames 不区分大小写。它们为不可变，无法被操作。
	     FNames 的存储系统和静态特性决定了通过键进行 FNames 的查找和访问速度较快。FName 子系统的另一个功能是使用散列表为 FName 转换提供快速字符串。
     
	* (3)FText 代表一个显示字符串。为用户显示的文本都需要由 FText 进行处理。FText 类拥有本地化的内置支持，可处理已本地化并存储在查找表中的文本内容，
	    以及在运行时被本地化的文本，如数字、日期、时间和格式化文本。甚至可用 FText 处理无需进行本地化的文本。
		包括用户输入内容，如玩家名和 Slate 显示的文本。FText 不提供任何变异函数，因为对显示字符串进行修改是非常不安全的操作。
	原文链接：https://blog.csdn.net/u012793104/article/details/78470388
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
