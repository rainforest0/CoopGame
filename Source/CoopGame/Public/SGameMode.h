// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

/**AGameModeBase
   ���� Game Mode ��Ϊ AGameModeBase �����ࡣ�� AGameModeBase ���������ɸ��ǵĻ������ܡ�
 * �������Ϸ�ṩ��ÿ��������ʽ���������ͻ��������򴴽� AGameModeBase �����ࡣһ����Ϸ��ӵ������������ Game Mode�����Ҳ��ӵ������������ AGameModeBase �����ࣻ
       Ȼ��������ʱ����ֻ��ʹ��һ�� Game Mode��ÿ�ιؿ�������Ϸʵ����ʱ Game Mode Actor ��ͨ�� UGameEngine::LoadMap() ��������ʵ������
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	// Bots to spawn in current wave����һ�����ɵĻ�����������
	int32 NrOfBotsToSpawn;

	//�ڼ���
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
