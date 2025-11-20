// Fill out your copyright notice in the Description page of Project Settings.


#include "System/ThiefGameMode.h"

#include "Item/Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/SpawnVolume.h"
#include "Character/ThiefPlayer.h"
#include "Component/InventoryComponent.h"

AThiefGameMode::AThiefGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AThiefGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnBox = Cast<ASpawnVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnVolume::StaticClass()));

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	PlayerRef = Cast<AThiefPlayer>(PlayerCharacter);

	if (SpawnBox)
	{
		SetItems();
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SPAWN BOX"));
	}

	StartGame();
}

void AThiefGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Tick"));

#if WITH_EDITOR
	if (GEngine)
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

    const int32 Scale = 100;
    int32 Capacity = FMath::RoundToInt(WeightCapacity * Scale); // 무게가 소수라서 100배 곱한 상태로 만들기(인덱싱 해야함)

    TArray<TArray<float>> DP;
    DP.SetNum(NumItems + 1); // 아이템 개수 + 1개만큼 행 만들기
    for (int32 i = 0; i <= NumItems; i++)
    {
        DP[i].SetNum(Capacity + 1); // 가방 무게 * 100 + 1개만큼 열 만들기
        for (int32 w = 0; w <= Capacity; w++)
        {
            DP[i][w] = 0.0f; // 초기화함
        }
    }

    for (int32 i = 1; i <= NumItems; i++)
    {
        float ItemWeight = Items[i - 1].NumericData.Weight;
        float ItemValue = Items[i - 1].NumericData.Value;
        int32 ScaledWeight = FMath::RoundToInt(ItemWeight * Scale);

        for (int32 w = 0; w <= Capacity; w++)
        {
            DP[i][w] = DP[i - 1][w];

            if (ScaledWeight <= w)
            {
                float ValueWithItem = DP[i - 1][w - ScaledWeight] + ItemValue;
                if (ValueWithItem > DP[i][w])
                {
                    DP[i][w] = ValueWithItem;
                }
            }
        }
    }

    return DP[NumItems][Capacity];  // 마지막 인덱스
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
	

	for (int i = 0; i <= maxItem; i++)
	{
		const FVector SpawnLocation = GetRandomLocation();
		const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

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
	GetWorld()->GetTimerManager().SetTimer(
		GameTimerHandle,
		this,
		&AThiefGameMode::DecreaseTimer,
		TimeRate,
		true
	);
}

void AThiefGameMode::EndGame()
{
	OnGameOver.Broadcast();	
}

void AThiefGameMode::DecreaseTimer()
{
	TimerTime -= TimeRate;
	OnTimeDecrease.Broadcast();
	
	if (TimerTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
		EndGame();
	}
}
