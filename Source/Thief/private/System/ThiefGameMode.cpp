// Fill out your copyright notice in the Description page of Project Settings.


#include "System/ThiefGameMode.h"

#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Blueprint/UserWidget.h"
#include "Item/Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Widgets/MainHUD.h"
#include "System/SpawnVolume.h"
#include "Widgets/PlayerHUD.h"

AThiefGameMode::AThiefGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AThiefGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnBox = Cast<ASpawnVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnVolume::StaticClass()));

	PlayerRef = Cast<AThiefPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (SpawnBox)
	{
		SetItems();
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SPAWN BOX"));
	}

	MainHUD = CreateWidget<UMainHUD>(GetWorld(), MainHUDClass);
	MainHUD->AddToViewport(2);

	StartGame();
}

void AThiefGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if WITH_EDITOR
	if (!GEngine)
	{
		for (FItemData& Item : Items)
		{
			FString IsPending = "NULL";
			FString ItemName = Item.TextData.Name.ToString();
			
			FString Message = FString::Printf(TEXT("[ %s ]"), *ItemName);

			UKismetSystemLibrary::PrintString(GetWorld(), Message, true, true, FLinearColor::Green, DeltaSeconds);
		}
	}
#endif
}

float AThiefGameMode::AssessPlayer(ACharacter* Player)
{
	float score = 0;

	
	
	return 0;
}

UItemBase* AThiefGameMode::CreateItemByID(FName ItemID, int32 Amount)
{
	//데이터 데이블에서 아이템 데이터 가져오기
	if (!ItemTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDataTable is null"));
		return nullptr;
	}

	FItemData* ItemData = ItemTable->FindRow<FItemData>(ItemID, TEXT("CreateItemByID"));
	
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData is null"));
		return nullptr;
	}
	
	//아이템 데이터 생성
	UItemBase* NewItem = NewObject<UItemBase>(StaticClass());

	//데이터 테이블에서 아이템 데이터 삽입
	NewItem->ID = ItemData->ID;
	NewItem->NumericData = ItemData->NumericData;
	NewItem->TextData = ItemData->TextData;
	NewItem->AssetData = ItemData->AssetData;
	NewItem->SetAmount(Amount);

	//아이템 데이터 반환
	return NewItem;
}

FVector AThiefGameMode::GetRandomLocation()
{
	FVector location = FVector::ZeroVector;

	if (SpawnBox)
	{
		FVector Origin = SpawnBox->SpawnBox->GetComponentLocation();
		FVector Extent = SpawnBox->SpawnBox->GetScaledBoxExtent();
		return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	}
	
	return location;
}

void AThiefGameMode::SetItems()
{
	TArray<FName> ItemRowCodes = ItemTable->GetRowNames();
	int32 maxItem = FMath::RandRange(ItemMin, ItemMax);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.bNoFail = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UE_LOG(LogTemp, Warning, TEXT("MAX ITEM : %d"), maxItem);
	

	for (int i = 0; i <= maxItem; i++)
	{
		const FVector SpawnLocation = GetRandomLocation();
		const FTransform SpawnTransform(FRotator(
		FMath::FRandRange(0.f, 360.f),
		FMath::FRandRange(0.f, 360.f),
		FMath::FRandRange(0.f, 360.f)
), SpawnLocation);

		int32 RandomIndex = FMath::RandRange(0, ItemRowCodes.Num() - 1);
		FName ItemCode = ItemRowCodes[RandomIndex];
		UItemBase* NewItem = CreateItemByID(ItemCode, 1);

		Items.Add(NewItem->GetItemData());
		
		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);
		
		Pickup->InitializeDrop(NewItem, 1);
	}
}

void AThiefGameMode::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Started"));
	GetWorld()->GetTimerManager().SetTimer(
		GameTimerHandle,
		this,
		&AThiefGameMode::DecreaseTimer,
		TimeRate,
		true
	);
}

void AThiefGameMode::TimeOut()
{
	OnTimeOut.Broadcast();
	
	if (PoliceSequence)
	{
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceKeepState;

		ALevelSequenceActor* OutActor;
		ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(), PoliceSequence, Settings, OutActor);
	
		Player->Play();
	}
}

void AThiefGameMode::RunSuccess()
{
	if (RunSequence)
	{
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceKeepState;

		ALevelSequenceActor* OutActor;
		ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(), RunSequence, Settings, OutActor);
	
		Player->Play();
	}
}

void AThiefGameMode::EndGame()
{
	OnGameOver.Broadcast();

	if (MainHUD)
	{
		MainHUD->RemoveFromParent();
	}
	if (PlayerRef && PlayerRef->HUD)
	{
		PlayerRef->HUD->RemoveFromParent();
	}

	//탈출 성공
	if (!IsTimeOut)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
		RunSuccess();
	}
	//탈출 불가
	else
	{
		
	}
}

void AThiefGameMode::DecreaseTimer()
{
	TimerTime -= TimeRate;
	OnTimeDecrease.Broadcast();
	
	if (TimerTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
		TimeOut();
	}
}
