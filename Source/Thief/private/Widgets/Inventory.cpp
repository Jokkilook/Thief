// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory.h"

#include "Character/ThiefPlayer.h"
#include "Components/Button.h"
#include "Component/InventoryComponent.h"
#include "Components/SizeBox.h"
#include "Components/WidgetSwitcher.h"
#include "Item/ItemBase.h"
#include "Widgets/InventoryPanel.h"
#include "Widgets/ItemDragDropOperation.h"

void UInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventory::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerRef = Cast<AThiefPlayer>(GetOwningPlayerPawn());
}

bool UInventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	UItemBase* Item = ItemDragDrop->SourceItem;
	
	if (PlayerRef && Item)
	{
		if (InventoryPanel)
		{
			FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
			
			FGeometry PanelGeometry = InventorySection->GetCachedGeometry();
			FVector2D PanelPos = InGeometry.AbsoluteToLocal(PanelGeometry.GetAbsolutePosition());
			FVector2D PanelSize = PanelGeometry.GetLocalSize();

			// 마우스가 패널 영역 안에 있다면 드랍 무시
			if (LocalMousePos.X >= PanelPos.X && LocalMousePos.X <= PanelPos.X + PanelSize.X &&
				LocalMousePos.Y >= PanelPos.Y && LocalMousePos.Y <= PanelPos.Y + PanelSize.Y)
			{
				return false;
			}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("LEFT CLICK DROP"));
		PlayerRef->DropItem(Item, Item->Amount, true);
		return true;
	}
	return false;
}

