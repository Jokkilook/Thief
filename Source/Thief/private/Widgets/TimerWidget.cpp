// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/TimerWidget.h"

#include "Components/TextBlock.h"
#include "System/ThiefGameMode.h"

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameModeRef = Cast<AThiefGameMode>(GetWorld()->GetAuthGameMode());

	if (GameModeRef)
	{
		GameModeRef->OnTimeDecrease.AddDynamic(this, &UTimerWidget::UpdateTimer);
	}
}

void UTimerWidget::UpdateTimer()
{
	if (GameModeRef)
	{
		TimerText->SetText(TimerFormat(GameModeRef->TimerTime));
	}
}

FText UTimerWidget::TimerFormat(float Time)
{
	FString FormattedTime;
	FString TimeString;
	
	if (Time <= 0.0f)
	{
		return FText::FromString(TEXT("00:00.00"));
	} else
	{
		int32 Minutes = FMath::FloorToInt(Time / 60.0f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(Time, 60.0f));
		int32 Milliseconds = (int32)((Time - FMath::FloorToInt(Time)) * 1000.0f) / 10;

		TimeString = FString::Printf(TEXT("%02d:%02d.%02d"), Minutes, Seconds, Milliseconds);
	}
	
	return FText::FromString(TimeString);
}
