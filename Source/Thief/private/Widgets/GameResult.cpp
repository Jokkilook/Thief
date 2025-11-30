// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameResult.h"

#include "Component/InventoryComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "System/ThiefGameMode.h"

void UGameResult::NativeConstruct()
{
	Super::NativeConstruct();

	GameModeRef = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	FInputModeUIOnly UIOnly;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PC->SetInputMode(UIOnly);
	PC->bShowMouseCursor = true;

	if (GameModeRef)
	{
		Result = GameModeRef->GetResultInfo();
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameResult::OnRestartButtonClicked);
	}

	if (MainButton)
	{
		MainButton->OnClicked.AddDynamic(this, &UGameResult::OnMainButtonClicked);
	}

	SetResultText();
}

void UGameResult::OnRestartButtonClicked()
{
	UGameplayStatics::OpenLevel(this, FName("MainLevel"), true);
}

void UGameResult::OnMainButtonClicked()
{
	UGameplayStatics::OpenLevel(this, FName("MenuLevel"), true);
}

void UGameResult::SetResultText()
{
	ScoreText->SetText(FText::AsNumber(Result.Score));

	//스코어는 S A B C D
	//S : 95 이상
	//A : 90 이상
	//B : 80 이상
	//C : 70 이상
	//D : 69이하
	if (Result.Score <= 0)
	{
		ScoreImage->SetBrushFromTexture(Images[5]);
	}
	else if (Result.Score >= 95)
	{
		ScoreImage->SetBrushFromTexture(Images[0]);
	}
	else if (Result.Score >= 90)
	{
		ScoreImage->SetBrushFromTexture(Images[1]);
	}
	else if (Result.Score >= 80)
	{
		ScoreImage->SetBrushFromTexture(Images[2]);
	}
	else if (Result.Score >= 70)
	{
		ScoreImage->SetBrushFromTexture(Images[3]);
	}
	else
	{
		ScoreImage->SetBrushFromTexture(Images[4]);
	}

	const FText TotalAmount = FText::Format(
	FText::FromString(TEXT("{0}")),
	FText::AsNumber(Result.TotalAmount));
	TotalAmountText->SetText(TotalAmount);

	const FText TotalValue = FText::Format(
		FText::FromString(TEXT("{0} $")),
		FText::AsNumber(Result.TotalValue));
	TotalValueText->SetText(TotalValue);

	const FText TotalWeight = FText::Format(
		FText::FromString(TEXT("{0}kg")),
		FText::AsNumber(Result.TotalWeight));
	TotalWeightText->SetText(TotalWeight);
	
	const FText EfficientValue = FText::Format(
    			FText::FromString(TEXT("{0} $")),
    			FText::AsNumber(Result.EfficientValue));
    		EfficientValueText->SetText(EfficientValue);

	//시간 안에 훔쳐 달아났을 때!
	if (!Result.IsTimeOut)
	{
		const FText StolenAmount = FText::Format(
				FText::FromString(TEXT("{0}")),
				FText::AsNumber(GameModeRef->PlayerRef->InventoryComponent->GetItemAmount()));
		StolenAmountText->SetText(StolenAmount);

		const FText StolenValue = FText::Format(
			FText::FromString(TEXT("{0} $")),
			FText::AsNumber(GameModeRef->PlayerRef->InventoryComponent->GetCurrentValue()));
		StolenValueText->SetText(StolenValue);

		const FText StolenWeight = FText::Format(
			FText::FromString(TEXT("{0}kg")),
			FText::AsNumber(GameModeRef->PlayerRef->InventoryComponent->GetCurrentWeight()));
		StolenWeightText->SetText(StolenWeight);
	}
	//경찰한테 잡혔을 때!
	else
	{
		const FText StolenAmount = FText::FromString("0");
		StolenAmountText->SetText(StolenAmount);

		const FText StolenValue = FText::FromString("0 $");
		StolenValueText->SetText(StolenValue);

		const FText StolenWeight = FText::FromString("0kg");
		StolenWeightText->SetText(StolenWeight);
		
		ScoreText->SetText(FText::AsNumber(0));
		ScoreImage->SetBrushFromTexture(Images[5]);		
	}

	
}
