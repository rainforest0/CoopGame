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


