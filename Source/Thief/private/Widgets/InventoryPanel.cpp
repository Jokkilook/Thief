// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryPanel.h"

#include "Character/ThiefPlayer.h"
#include "Component/InventoryComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Item/ItemBase.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerRef = Cast<AThiefPlayer>(GetOwningPlayerPawn());
	if (PlayerRef)
	{
		InventoryRef = PlayerRef->FindComponentByClass<UInventoryComponent>();
		
		if (InventoryRef)
		{
			InventoryRef->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
			SetInfoText();
			UE_LOG(LogTemp, Warning, TEXT("RefreshInventory Registered"));
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("No InventoryRef"));
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerRef"));
	}
}

void UInventoryPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (InventoryRef)
	{
		WeightBar->SetPercent(InventoryRef->GetWeightPercent());
	}
}

void UInventoryPanel::SetInfoText() const
{
	//용량 ( ex) 2/100 ) 텍스트 업데이트
	const FString WeightInfoText =
		{FString::SanitizeFloat(InventoryRef->GetCurrentWeight())+"/"+
		 FString::FromInt(InventoryRef->GetWeightCapacity())+"kg"};
	WeightInfo->SetText(FText::FromString(WeightInfoText));

	FText ValueText = FText::Format(
	FText::FromString(TEXT("{0} $")),
	FText::AsNumber(InventoryRef->GetCurrentValue()));
	ValueInfo->SetText(ValueText);
}

void UInventoryPanel::RefreshInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("RefreshInventory"));
	if (InventoryRef && SlotClass)
	{
		InventoryPanel->ClearChildren();
		
		int32 Index = 0;
		for (FItemSlot InventorySlot : InventoryRef->GetInventory())
		{
			UInventorySlot* ItemSlot = CreateWidget<UInventorySlot>(this, SlotClass);
			ItemSlot->SetIndex(Index++);
			
			if (InventorySlot.Item)
			{
				ItemSlot->SetItemReference(InventorySlot.Item);
			}
			
			InventoryPanel->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
