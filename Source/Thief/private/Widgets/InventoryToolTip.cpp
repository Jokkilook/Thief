// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryToolTip.h"
#include "Widgets/InventorySlot.h"

#include "Components/TextBlock.h"
#include "Item/ItemBase.h"

void UInventoryToolTip::NativeConstruct()
{
	Super::NativeConstruct();
	
	

	if (InventorySlotBeingHovered)
	{
		UItemBase* Item = InventorySlotBeingHovered->GetItemReference();
		
		if (Item)
		{
			ItemName->SetText(Item->TextData.Name);

			FText ValueText = FText::Format(
			FText::FromString(TEXT("{0} $")),
			FText::AsNumber(Item->NumericData.Value));
	
			ItemValue->SetText(ValueText);

			FText WeightText = FText::Format(
			FText::FromString(TEXT("{0}kg")),
			FText::AsNumber(Item->NumericData.Weight));
			
			ItemWeight->SetText(WeightText);
		}
	} 
	else
	{
		ItemName->SetText(ItemData.TextData.Name);
		
		ItemWeight->SetText(FText::AsNumber(ItemData.NumericData.Weight));
	}
}
