// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FOnInventoryClicked);
DECLARE_DELEGATE(FOnCraftClicked);
DECLARE_DELEGATE(FOnUnknownRecordClicked);

UCLASS()
class THIEF_API UInventory : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere ,BlueprintReadWrite)
	class AThiefPlayer* PlayerRef;

	UPROPERTY(meta=(BindWidget))
	class USizeBox* InventorySection;

	UPROPERTY(meta=(BindWidget))
	class UInventoryPanel* InventoryPanel;


protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
