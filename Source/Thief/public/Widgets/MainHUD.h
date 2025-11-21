// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class THIEF_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class AThiefPlayer* PlayerRef;

	UPROPERTY(meta=(BindWidget))
	class UTimerWidget* Timer;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* WeightBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ValueText;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetProperties();
};
