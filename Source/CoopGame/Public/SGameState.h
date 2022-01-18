// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState: uint8
{
	WaitingToStart,

	WaveInProgress,

	// No longer spawning new bots, waiting for players to kill remaining bots
	WaitingToComplete,

	WaveComplete,

	GameOver,
};

/**
 * Game State 负责启用客户端监控游戏状态。从概念上而言，Game State 应该管理所有已连接客户端已知的信息（特定于 Game Mode 但不特定于任何个体玩家）。
            它能够追踪游戏层面的属性，如已连接玩家的列表、夺旗游戏中的团队得分、开放世界游戏中已完成的任务，等等。

  Game State 并非追踪玩家特有内容（如夺旗比赛中特定玩家为团队获得的分数）的最佳之处，因为它们由 Player State 更清晰地处理。
            整体而言，GameState 应该追踪游戏进程中变化的属性。这些属性与所有人皆相关，且所有人可见。
 
 Game mode 只存在于服务器上，而 Game State 存在于服务器上且会被复制到所有客户端，保持所有已连接机器的游戏进程更新。
 
 AGameStateBase 是基础实现，其部分默认功能包括：
函数或变量                      使用
GetServerWorldTimeSeconds      这是 UWorld 函数 GetTimeSeconds 的服务器版本，将在客户端和服务器上同步，因此该时间可用于复制，十分可靠。
PlayerArray                    这是所有 APlayerState 对象的阵列，对游戏中所有玩家执行操作时十分实用。
HasBegunPlay                   如 BeginPlay 函数在游戏中的 actor 上调用，则返回 true。

AGameStateBase 通常在 C++ 或蓝图中延展，包含用于使游戏中玩家知晓当前情况的额外变量和函数。
              进行的特定修改通常基于 Game State 的配对 Game Mode。 Game Mode 自身也可将其默认 Game State 类覆盖为派生自 AGameStateBase 的任意 C++ 类或蓝图。
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public:

	void SetWaveState(EWaveState NewState);
};
