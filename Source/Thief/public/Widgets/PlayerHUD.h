// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/InventoryComponent.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */

class UProgressBar;
class UItemAcquiredBlock;

UCLASS()
class THIEF_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void AddItemMessage(FItemAddResult Result);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AThiefPlayer* PlayerRef;
		
	UPROPERTY(meta=(BindWidget))
	UProgressBar* InteractionBar;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemAcquiredBlock> ItemAcquiredBlockClass;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* InfoList;

	UPROPERTY()
	bool ShowInteraction = true;

	UFUNCTION(BlueprintCallable)
	void DisplayInteraction();
	UFUNCTION(BlueprintCallable)
	void HideInteraction();
	UFUNCTION(BlueprintCallable)
	void ToggleInteraction();
	UFUNCTION(BlueprintCallable)
	void UpdateInteraction();


protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
