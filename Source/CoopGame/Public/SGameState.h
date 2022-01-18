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
 * Game State �������ÿͻ��˼����Ϸ״̬���Ӹ����϶��ԣ�Game State Ӧ�ù������������ӿͻ�����֪����Ϣ���ض��� Game Mode �����ض����κθ�����ң���
            ���ܹ�׷����Ϸ��������ԣ�����������ҵ��б�������Ϸ�е��Ŷӵ÷֡�����������Ϸ������ɵ����񣬵ȵȡ�

  Game State ����׷������������ݣ������������ض����Ϊ�Ŷӻ�õķ����������֮������Ϊ������ Player State �������ش���
            ������ԣ�GameState Ӧ��׷����Ϸ�����б仯�����ԡ���Щ�����������˽���أ��������˿ɼ���
 
 Game mode ֻ�����ڷ������ϣ��� Game State �����ڷ��������һᱻ���Ƶ����пͻ��ˣ��������������ӻ�������Ϸ���̸��¡�
 
 AGameStateBase �ǻ���ʵ�֣��䲿��Ĭ�Ϲ��ܰ�����
���������                      ʹ��
GetServerWorldTimeSeconds      ���� UWorld ���� GetTimeSeconds �ķ������汾�����ڿͻ��˺ͷ�������ͬ������˸�ʱ������ڸ��ƣ�ʮ�ֿɿ���
PlayerArray                    �������� APlayerState ��������У�����Ϸ���������ִ�в���ʱʮ��ʵ�á�
HasBegunPlay                   �� BeginPlay ��������Ϸ�е� actor �ϵ��ã��򷵻� true��

AGameStateBase ͨ���� C++ ����ͼ����չ����������ʹ��Ϸ�����֪����ǰ����Ķ�������ͺ�����
              ���е��ض��޸�ͨ������ Game State ����� Game Mode�� Game Mode ����Ҳ�ɽ���Ĭ�� Game State �า��Ϊ������ AGameStateBase ������ C++ �����ͼ��
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
