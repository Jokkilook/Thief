// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Item/ItemBase.h"
#include "ThiefGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THIEF_API AThiefGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AThiefGameMode();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter* PlayerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ASpawnVolume* SpawnBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDataTable* ItemTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemMin = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemMax = 100;

	TArray<FItemData> Items;
	
	UFUNCTION(Blueprintable)
	float AssessPlayer(ACharacter* Player);

	UFUNCTION()
	UItemBase* CreateItemByID(FName ItemID, int32 Amount);

	UFUNCTION(BlueprintCallable)
	FVector GetRandomLocation();

	UFUNCTION(BlueprintCallable)
	void SetItems();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
};