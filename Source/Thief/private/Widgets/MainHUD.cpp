// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainHUD.h"

#include "Character/ThiefPlayer.h"
#include "Component/InventoryComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerRef = Cast<AThiefPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void UMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	SetProperties();
	
}

void UMainHUD::SetProperties()
{
	if (PlayerRef)
	{
		WeightBar->SetPercent(PlayerRef->InventoryComponent->GetWeightPercent());

		FText Value = FText::Format(
			FText::FromString(TEXT("{0} $")),
			FText::AsNumber(PlayerRef->InventoryComponent->GetCurrentValue()));
		
		ValueText->SetText(Value);
	}
}
