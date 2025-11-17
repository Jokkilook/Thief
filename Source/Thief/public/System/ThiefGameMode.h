// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ThiefGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THIEF_API AThiefGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter* PlayerRef;

	
	UFUNCTION(Blueprintable)
	float AssessPlayer(ACharacter* Player);
};
