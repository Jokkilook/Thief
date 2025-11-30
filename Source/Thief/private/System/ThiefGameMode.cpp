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
#include "Character/ThiefPlayer.h"
#include "Component/InventoryComponent.h"
#include "Widgets/PlayerHUD.h"
#include "Widgets/GameResult.h"

AThiefGameMode::AThiefGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AThiefGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> SpawnBoxList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), SpawnBoxList);

	SpawnBox = Cast<ASpawnVolume>(SpawnBoxList[FMath::RandRange(0,SpawnBoxList.Num()-1)]);
	//SpawnBox = Cast<ASpawnVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnVolume::StaticClass()));

	PlayerRef = Cast<AThiefPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (SpawnBox)
	{
		SetItems();
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SPAWN BOX"));
	}

	MainHUD = CreateWidget<UMainHUD>(GetWorld(), MainHUDClass);
	MainHUD->AddToViewport(3);

	OnTimeOut.AddDynamic(this, &AThiefGameMode::OnTimeOutEvent);

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
			
			//FString Message = FString::Printf(TEXT("[ %s ]"), *ItemName);

			//UKismetSystemLibrary::PrintString(GetWorld(), Message, true, true, FLinearColor::Green, DeltaSeconds);
		}
	}
#endif
}

float AThiefGameMode::SolveKnapsack(float WeightCapacity)
{
    int32 NumItems = Items.Num();

    if (NumItems == 0)
    {
		UE_LOG(LogTemp, Warning, TEXT("NO Items"));
        return 0.0f;
    }

	// 무게가 소수라서 100배 곱한 상태로 만들기(인덱싱 해야함)
    const int32 Scale = 100;
    int32 Capacity = FMath::RoundToInt(WeightCapacity * Scale); 

	// 2차원 배열 생성 후 초기화
    TArray<TArray<float>> DP;
	// 아이템 개수 + 1개만큼 행 만들기
    DP.SetNum(NumItems + 1);
	
    for (int32 i = 0; i <= NumItems; i++)
    {
    	// 가방 무게 * 100 + 1개만큼 열 만들기
        DP[i].SetNum(Capacity + 1);
    	//모든 칸 초기화
        for (int32 w = 0; w <= Capacity; w++)
        {
            DP[i][w] = 0.0f;
        }
    }

	//DP 돌리기
    for (int32 i = 1; i <= NumItems; i++)
    {
        float ItemWeight = Items[i - 1].NumericData.Weight;
        float ItemValue = Items[i - 1].NumericData.Value;
        int32 ScaledWeight = FMath::RoundToInt(ItemWeight * Scale);

        for (int32 w = 0; w <= Capacity; w++)
        {
        	// 아이템 안넣으면 이전 값으로
            DP[i][w] = DP[i - 1][w];

        	// 아이템을 넣을 수 있으면
            if (ScaledWeight <= w)
            {
            	// 값어치 = 이전 값 + 아이템 값어치
                float ValueWithItem = DP[i - 1][w - ScaledWeight] + ItemValue;
            	// 아이템 넣은 값어치가 이전보다 크면
                if (ValueWithItem > DP[i][w])
                {
                	// 더 큰 값으로 업데이트
                    DP[i][w] = ValueWithItem;
                }
            }
        }
    }

	//마지막 테이블 값 리턴
    return DP[NumItems][Capacity];
}

float AThiefGameMode::AssessPlayer(AThiefPlayer* Player)
{
    if (!Player)
    {
		UE_LOG(LogTemp, Warning, TEXT("Player is null"));
        return 0.0f;
    }

    if (!Player->InventoryComponent)
    {
		UE_LOG(LogTemp, Warning, TEXT("Inventory is null"));
        return 0.0f;
    }

    float WeightCapacity = Player->InventoryComponent->GetWeightCapacity();
    float OptimalValue = SolveKnapsack(WeightCapacity);

    float PlayerValue = 0.0f;
	float PlayerWeight = 0.0f;

    TArray<FItemSlot> InventorySlots = Player->InventoryComponent->GetInventory();

    for (const FItemSlot& Slot : InventorySlots)
    {
        if (Slot.Item)
        {
            PlayerValue += Slot.Item->NumericData.Value * Slot.Item->Amount;
			PlayerWeight += Slot.Item->NumericData.Weight * Slot.Item->Amount;
        }
    }

    float Score = 0.0f;
    if (OptimalValue > 0.0f)
    {
        Score = (PlayerValue / OptimalValue) * 100.0f; // 최적해에 대한 비율
    }

	// 결과 출력
	/*
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("========== PLAYER ASSESSMENT =========="), true, true, FLinearColor::Yellow, 5.0f);

	FString ItemsInMap = FString::Printf(TEXT("Total Items in Map: %d"), Items.Num());
	UKismetSystemLibrary::PrintString(GetWorld(), ItemsInMap, true, true, FLinearColor::White, 5.0f);

	FString CapacityStr = FString::Printf(TEXT("Weight Capacity: %.2f"), WeightCapacity);
	UKismetSystemLibrary::PrintString(GetWorld(), CapacityStr, true, true, FLinearColor::White, 5.0f);

	FString OptimalValueStr = FString::Printf(TEXT("Optimal Max Value: %.2f"), OptimalValue);
	UKismetSystemLibrary::PrintString(GetWorld(), OptimalValueStr, true, true, FLinearColor::Blue, 5.0f);

	FString PlayerVal = FString::Printf(TEXT("Player Total Value: %.2f"), PlayerValue);
	UKismetSystemLibrary::PrintString(GetWorld(), PlayerVal, true, true, FLinearColor::Green, 5.0f);

	FString PlayerWt = FString::Printf(TEXT("Player Total Weight: %.2f"), PlayerWeight);
	UKismetSystemLibrary::PrintString(GetWorld(), PlayerWt, true, true, FLinearColor::Green, 5.0f);

	FString PlayerCount = FString::Printf(TEXT("Player Items Count: %d"), InventorySlots.Num());
	UKismetSystemLibrary::PrintString(GetWorld(), PlayerCount, true, true, FLinearColor::Green, 5.0f);

	FString ScoreMsg = FString::Printf(TEXT("Efficiency Score: %.2f%%"), Score);
	UKismetSystemLibrary::PrintString(GetWorld(), ScoreMsg, true, true, FLinearColor::Blue, 5.0f);

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("======================================"), true, true, FLinearColor::Yellow, 5.0f);
	*/

    return Score;
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
	

	for (const FItemData& Item : GetCandidateItems())
	{
		const FVector SpawnLocation = GetRandomLocation();
		const FTransform SpawnTransform(FRotator(
		FMath::FRandRange(0.f, 360.f),
		FMath::FRandRange(0.f, 360.f),
		FMath::FRandRange(0.f, 360.f)),
		SpawnLocation);

		UItemBase* NewItem = CreateItemByID(Item.ID, 1);

		Items.Add(NewItem->GetItemData());
		
		APickup* Pickup = GetWorld()->SpawnActor<APickup>(PickUpClass, SpawnTransform, SpawnParams);
		
		Pickup->InitializeDrop(NewItem, 1, false);
	}
}

void AThiefGameMode::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Started"));
	FInputModeGameOnly GameOnly;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PC->SetInputMode(GameOnly);
	PC->bShowMouseCursor = false;
	
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
	IsTimeOut = true;
	
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
	UE_LOG(LogTemp, Warning, TEXT("END GAME SEQUENCE"));

	if (MainHUD)
	{
		MainHUD->RemoveFromParent();
	}
	if (PlayerRef && PlayerRef->HUD)
	{
		PlayerRef->HUD->RemoveFromParent();
	}
	UE_LOG(LogTemp, Warning, TEXT("UI REMOVED"));


	//PlayerRef->DisableInput(UGameplayStatics::GetPlayerController(this, 0));
	
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
	UE_LOG(LogTemp, Warning, TEXT("TIME OUT CHECK"));


	GameResult = CreateWidget<UGameResult>(GetWorld(), GameResultClass);
	GameResult->AddToViewport(4);
}

FVaultInfo AThiefGameMode::GetResultInfo()
{
	FVaultInfo VaultInfo;

	for (FItemData Item : Items)
	{
		VaultInfo.TotalAmount += 1;
		VaultInfo.TotalValue += Item.NumericData.Value;
		VaultInfo.TotalWeight += Item.NumericData.Weight;
	}

	VaultInfo.Score = AssessPlayer(PlayerRef);
	VaultInfo.EfficientValue = SolveKnapsack(PlayerRef->InventoryComponent->GetWeightCapacity());
	VaultInfo.IsTimeOut = IsTimeOut;
		
	return VaultInfo;
}

TArray<FItemData> AThiefGameMode::GetCandidateItems()
{
	TArray<FItemData> Result;

    // 1. 아이템 테이블에서 모든 행 가져오기
    TArray<FName> RowNames = ItemTable->GetRowNames();

    TArray<FCandidateInfo> High, Mid, Low;

    for (auto& RowName : RowNames)
    {
        FItemData* Row = ItemTable->FindRow<FItemData>(RowName, TEXT(""));
        if (Row == nullptr) continue;

        FCandidateInfo Info;
        Info.Data = *Row;
        Info.Efficiency = (Row->NumericData.Weight > 0) ? Row->NumericData.Value / Row->NumericData.Weight : 0.f;

        if (Info.Efficiency >= 2.0f) High.Add(Info);
        else if (Info.Efficiency >= 1.0f) Mid.Add(Info);
        else Low.Add(Info);
    }

    // 2. 목표 개수
    int32 TotalCount = FMath::RandRange(ItemMin, ItemMax);

	int32 HighCount = TotalCount * 0.2f;
	int32 MidCount  = TotalCount * 0.5f;
	int32 LowCount  = TotalCount - (HighCount + MidCount);

    float SumWeight = 0.f;
    float SumValue = 0.f;

    auto AddRandom = [&](TArray<FCandidateInfo>& Src, int32 Count)
    {
        if (Src.Num() == 0) return;

        for (int i = 0; i < Count; i++)
        {
            const FCandidateInfo& Info = Src[FMath::RandRange(0, Src.Num() - 1)];
            Result.Add(Info.Data);

            SumWeight += Info.Data.NumericData.Weight;
            SumValue += Info.Data.NumericData.Value;
        }
    };

    AddRandom(High, HighCount);
    AddRandom(Mid,  MidCount);
    AddRandom(Low,  LowCount);

    // 3. 목표 총 무게 설정 (플레이어 최대 무게 기준으로)
    float PlayerMaxWeight = PlayerRef->InventoryComponent->GetWeightCapacity();
    float TargetWeight = PlayerMaxWeight * 2.0f;

    // 4. 무게가 너무 적으면 → 고효율 아이템 추가
    while (SumWeight < TargetWeight * 0.9f && High.Num() > 0)
    {
        const FCandidateInfo& Info = High[FMath::RandRange(0, High.Num() - 1)];
        Result.Add(Info.Data);
        SumWeight += Info.Data.NumericData.Weight;
        SumValue += Info.Data.NumericData.Value;
    }

    // 5. 무게가 너무 많으면 → 저효율 아이템 제거
    while (SumWeight > TargetWeight * 1.1f)
    {
        bool bRemoved = false;

        for (int i = Result.Num() - 1; i >= 0; --i)
        {
            float Efficiency = Result[i].NumericData.Value / Result[i].NumericData.Weight;
            if (Efficiency < 1.0f) // 저효율
            {
                SumWeight -= Result[i].NumericData.Weight;
                SumValue  -= Result[i].NumericData.Value;
                Result.RemoveAt(i);
                bRemoved = true;
                break;
            }
        }

        if (!bRemoved)
            break;
    }

    return Result;
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
