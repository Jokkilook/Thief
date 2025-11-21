// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractionInterface.h"
#include "ThiefPlayer.generated.h"

USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_USTRUCT_BODY();

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
	{

	};

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentInteractable;

	UPROPERTY(BlueprintReadOnly)
	float LastInteractionCheckTime;
};

UCLASS()
class THIEF_API AThiefPlayer : public ACharacter , public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AThiefPlayer();

	//컴포넌트=========================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* FirstPersonCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInventoryComponent* InventoryComponent;

	//입력 관련 변수====================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	class UInputMappingContext* InputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	class UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	UInputAction* RunAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	UInputAction* InteractAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	UInputAction* InventoryAction;

	//인벤토리가 열려 있는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="State")
	bool IsInventoryOpen = false;

	//인터랙션 관련 변수===============================================================
	//인터랙션 타이머 - 꾹 누르는 인터랙션을 위한 것
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Interaction")
	FTimerHandle InteractionTimer;
	//인터랙션 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	float InteractionCheckDistance = 300.0f;
	//인터랙션 체크 빈도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	float InteractionCheckFrequency = 0.1f;
	//인터랙션 데이터 (인터랙션 액터, 인터랙션 시간)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	FInteractionData InteractionData;
	//인터랙션 액터의 인터랙션 인터페이스 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractionInterface;
	//꾹 누르기 인터랙션 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	float InteractionDuration = 0.0f;
	//픽업 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TSubclassOf<class APickup> PickUpClass;

	//위젯============================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget")
	TSubclassOf<class UPlayerHUD> HUDClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget")
	UPlayerHUD* HUD;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget")
	TSubclassOf<class UInventory> InventoryClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget")
	UInventory* Inventory;

	//사운드===========================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sound")
	FTimerHandle FootStepHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sound")
	USoundBase* FootStepSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	float FootStepRate = 0.4f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyControllerChanged() override;

	//인터랙션 관련 함수===================================================
	//인터랙션 체크 함수 - 라인트레이스로 인터랙션 액터 체크
	UFUNCTION()
	void CheckInteraction();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetInteractionDuration() { return InteractableData.InteractionDuration; };
	
	//인터랙션 액터를 찾았을 때
	UFUNCTION()
	void FoundInteractable(AActor* Interactable);
	
	//인터랙션 액터를 못 찾았을 때
	UFUNCTION()
	void NotFoundInteractable();
	
	//인터랙션 중인지 확인하는 함수==========================================
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(InteractionTimer); };

	//입력 관련 함수=======================================================
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Run();

	UFUNCTION()
	void StopRun();

	UFUNCTION(BlueprintCallable)
	void ToggleInventory();

	UFUNCTION(BlueprintCallable)
	void StartFootStep();

	UFUNCTION(BlueprintCallable)
	void PlayFootStep();

	UFUNCTION(BlueprintCallable)
	void StopFootStep();

	
	UFUNCTION(BlueprintCallable)
	void BeginInteract() override;
	UFUNCTION(BlueprintCallable)
	void EndInteract() override;
	UFUNCTION(BlueprintCallable)
	void Interaction();

	UFUNCTION(BlueprintImplementableEvent)
	void Interact(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable)
	void DropItem(class UItemBase* ItemToDrop, const int32 AmountToDrop, bool IsWhole);

};
