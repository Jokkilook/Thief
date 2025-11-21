// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class THIEF_API UTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AThiefGameMode* GameModeRef;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TimerText;

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateTimer();

	FText TimerFormat(float Time);
};
