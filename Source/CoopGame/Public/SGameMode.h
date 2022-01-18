// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

/**AGameModeBase
   所有 Game Mode 均为 AGameModeBase 的子类。而 AGameModeBase 包含大量可覆盖的基础功能。
 * 可针对游戏提供的每个比赛格式、任务类型或特殊区域创建 AGameModeBase 的子类。一款游戏可拥有任意数量的 Game Mode，因此也可拥有任意数量的 AGameModeBase 类子类；
       然而，给定时间上只能使用一个 Game Mode。每次关卡进行游戏实例化时 Game Mode Actor 将通过 UGameEngine::LoadMap() 函数进行实例化。
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	// Bots to spawn in current wave（这一波生成的机器人数量）
	int32 NrOfBotsToSpawn;

	//第几波
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

protected:

	// Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// Start Spawning Bots
	void StartWave();

	// Stop Spawning Bots
	void EndWave();

	// Set timer for next startwave
	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayers();

public:

	ASGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
};
