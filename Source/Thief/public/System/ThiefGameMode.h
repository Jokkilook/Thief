// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ThiefPlayer.h"
#include "GameFramework/GameModeBase.h"
#include "Item/ItemBase.h"
#include "ThiefGameMode.generated.h"

class APickup;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeDecrease);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeOut);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

USTRUCT(BlueprintType)
struct FVaultInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalAmount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalValue = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalWeight = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EfficientValue = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Score = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsTimeOut = false;
};

USTRUCT(BlueprintType)
struct FCandidateInfo
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemData Data;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Efficiency;
};

UCLASS()
class THIEF_API AThiefGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AThiefGameMode();

	UPROPERTY(BlueprintAssignable)
	FOnTimeDecrease OnTimeDecrease;

	UPROPERTY(BlueprintAssignable)
	FOnTimeOut OnTimeOut;
	
	UPROPERTY(BlueprintAssignable)
	FOnGameOver OnGameOver;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AThiefPlayer* PlayerRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ASpawnVolume* SpawnBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDataTable* ItemTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemMin = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemMax = 100;

	UPROPERTY(EditDefaultsOnly)
	float MaxValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FItemData> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimerTime = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeRate = 0.01;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTimerHandle GameTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UMainHUD* MainHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMainHUD> MainHUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UGameResult* GameResult;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameResult> GameResultClass;

	UFUNCTION(BlueprintCallable)
	float AssessPlayer(AThiefPlayer* Player);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class ULevelSequence* PoliceSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ULevelSequence* RunSequence;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsTimeOut = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<APickup> PickUpClass;

	UFUNCTION()
	UItemBase* CreateItemByID(FName ItemID, int32 Amount);

	UFUNCTION(BlueprintCallable)
	FVector GetRandomLocation();

	UFUNCTION()
	float SolveKnapsack(float WeightCapacity);

	UFUNCTION(BlueprintCallable)
	void SetItems();

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void TimeOut();

	UFUNCTION(BlueprintCallable)
	void RunSuccess();

	UFUNCTION(BlueprintCallable)
	void EndGame();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTimeOutEvent();

	UFUNCTION(BlueprintCallable)
	FVaultInfo GetResultInfo();

	UFUNCTION(BlueprintCallable)
	TArray<FItemData> GetCandidateItems();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void DecreaseTimer();
	
};