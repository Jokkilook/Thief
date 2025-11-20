// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBase.h"
#include "Component/InventoryComponent.h"

UItemBase::UItemBase() : IsCopy(false), IsPickup(false)
{
}

UItemBase* UItemBase::CreateItemCopy() const
{
    UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

    ItemCopy->ID = this->ID;
    ItemCopy->Amount = this->Amount;
    ItemCopy->TextData = this->TextData;
    ItemCopy->NumericData = this->NumericData;
    ItemCopy->AssetData = this->AssetData;
    ItemCopy->IsCopy = true;

    return ItemCopy;
}

void UItemBase::SetAmount(const int32 NewAmount)
{
    if (NewAmount != Amount) {
        Amount = FMath::Clamp(NewAmount, 0, NumericData.IsStackable ? NumericData.MaxAmount : 1);

        if (OwningInventory) {
            if(Amount <= 0){
                OwningInventory->RemoveSingleInstanceOfItem(this);
            }
        }
    }
}

void UItemBase::ResetItemFlags()
{
    IsCopy = false;
    IsPickup = false;
}

void UItemBase::Use_Implementation()
{
}

FItemData UItemBase::GetItemData() const
{
    FItemData ItemData;
    ItemData.ID = ID;
    ItemData.TextData = TextData;
    ItemData.NumericData = NumericData;
    ItemData.AssetData = AssetData;

    return ItemData;
}
