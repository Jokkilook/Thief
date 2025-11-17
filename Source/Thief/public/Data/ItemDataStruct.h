#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStruct.generated.h"

//아이템 문자 데이터 (이름, 설명)
USTRUCT(BlueprintType)
struct FItemTextData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

//아이템 숫자 데이터 (최대 수량, 한칸에 여러개 가능 여부)
USTRUCT(BlueprintType)
struct FItemNumericData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsStackable;
};

//아이템 에셋 데이터 (아이콘, 메쉬)
USTRUCT(BlueprintType)
struct FItemAssetData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;
};

//아이템 데이터 (최종)
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FItemTextData TextData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FItemNumericData NumericData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FItemAssetData AssetData;
};

//슬롯 데이터
USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_USTRUCT_BODY();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UPROPERTY()
	TObjectPtr<class UItemBase> Item = nullptr;
};