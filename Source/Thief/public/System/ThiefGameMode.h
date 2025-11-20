// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Item/ItemBase.h"
#include "ThiefGameMode.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnTimeDecrease);
DECLARE_MULTICAST_DELEGATE(FGameOver);

UCLASS()
class THIEF_API AThiefGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AThiefGameMode();
	
	FOnTimeDecrease OnTimeDecrease;

	FGameOver OnGameOver;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class AThiefPlayer* PlayerRef;

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

	UFUNCTION(BlueprintCallable)
	float AssessPlayer(AThiefPlayer* Player);

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
	void EndGame();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void DecreaseTimer();
	
};