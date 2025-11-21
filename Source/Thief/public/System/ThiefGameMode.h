// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ThiefPlayer.h"
#include "GameFramework/GameModeBase.h"
#include "Item/ItemBase.h"
#include "ThiefGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeDecrease);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeOut);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class ULevelSequence* PoliceSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ULevelSequence* RunSequence;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsTimeOut = false;

	
	
	UFUNCTION(Blueprintable)
	float AssessPlayer(ACharacter* Player);

	UFUNCTION()
	UItemBase* CreateItemByID(FName ItemID, int32 Amount);

	UFUNCTION(BlueprintCallable)
	FVector GetRandomLocation();

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

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void DecreaseTimer();
	
};