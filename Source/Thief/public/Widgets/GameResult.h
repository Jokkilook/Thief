// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "System/ThiefGameMode.h"
#include "GameResult.generated.h"

/**
 * 
 */
UCLASS()
class THIEF_API UGameResult : public UUserWidget
{
	GENERATED_BODY()

public:
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameResult")
	class AThiefGameMode* GameModeRef;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TotalAmountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TotalValueText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TotalWeightText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StolenAmountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StolenValueText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StolenWeightText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* EfficientValueText;

	UPROPERTY(meta=(BindWidget))
	class UImage* ScoreImage;

	UPROPERTY(meta=(BindWidget))
	class UButton* RestartButton;

	UPROPERTY(meta=(BindWidget))
	UButton* MainButton;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TArray<class UTexture2D*> Images;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameResult")
	FVaultInfo Result;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameResult")
	TSoftObjectPtr<UWorld> MenuLevel;

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetResultText();

	UFUNCTION()
	void OnRestartButtonClicked();

	UFUNCTION()
	void OnMainButtonClicked();
	
};
