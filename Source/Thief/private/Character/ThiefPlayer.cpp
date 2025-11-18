// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ThiefPlayer.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Widgets/PlayerHUD.h"
#include "Component/InventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Inventory.h"


// Sets default values
AThiefPlayer::AThiefPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->bUsePawnControlRotation = true;
	
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	
	//인벤토리 초기화
	InventoryComponent->SetSlotsCapacity(10);
	InventoryComponent->SetWeightCapacity(6);

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

// Called when the game starts or when spawned
void AThiefPlayer::BeginPlay()
{
	Super::BeginPlay();

	InteractableData.InteractionDuration = InteractionDuration;

	if (HUDClass)
	{
		HUD = CreateWidget<UPlayerHUD>(GetWorld(), HUDClass);
		HUD->AddToViewport(0);
	}

	if (InventoryClass)
	{
		Inventory = CreateWidget<UInventory>(GetWorld(), InventoryClass);
		Inventory->AddToViewport(1);
		Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

// Called every frame
void AThiefPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		CheckInteraction();
	}

}

// Called to bind functionality to input
void AThiefPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//IA와 IMC는 블루프린트에서 할당
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 향상된 입력 컴포넌트
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThiefPlayer::Move);
		
		// 시야
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThiefPlayer::Look);

		//달리기
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AThiefPlayer::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AThiefPlayer::StopRun);
		
		//인터랙션
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AThiefPlayer::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AThiefPlayer::EndInteract);

		//인벤토리
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AThiefPlayer::ToggleInventory);
	}

}

void AThiefPlayer::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 입력 매핑 컨텍스트(IMC) 추가
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void AThiefPlayer::CheckInteraction()
{
	//플레이어 시야 카메라 체크
	if (FirstPersonCamera)
	{
		InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
		//트레이스 시작 지점
		FVector TraceStart{ FirstPersonCamera->GetComponentLocation() };
		//트레이스 종료 지점
		FVector TraceEnd{ TraceStart + (FirstPersonCamera->GetForwardVector() * InteractionCheckDistance) };

		//라인 트레이스 디버그 라인
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);
		
		//자기 메쉬에 안부딪히게 설정
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		//충돌 결과 변수
		FHitResult HitResult;

		//라인트레이스 실행 후 부딪혔나?
		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			//부딪힌 액터가 인터랙션 인터페이스를 가지고 있나?
			if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				//UE_LOG(LogTemp, Warning, TEXT("It has interface."));
				//부딪힌 액터가 현재 인터랙터블 데이터와 다르다면
				if (HitResult.GetActor() != InteractionData.CurrentInteractable)
				{
					//UE_LOG(LogTemp, Warning, TEXT("FoundInteractable"));
					//TargetInteractable에 결과물 넣기
					FoundInteractable(HitResult.GetActor());
					return;
				}

				//부딪힌 액터가 현재 인터랙터블 액터와 같다면 암것두 안하기~
				if (HitResult.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	NotFoundInteractable();
}

void AThiefPlayer::FoundInteractable(AActor* Interactable)
{
	if (IsInteracting()) 
	{
		EndInteract();
	}

	//현재 인터랙션 액터 데이터가 있으면
	if (InteractionData.CurrentInteractable)
	{	
		TargetInteractionInterface = InteractionData.CurrentInteractable;
		TargetInteractionInterface->EndFocus();
	}

	//인터랙션 액터 데이터 지정
	InteractionData.CurrentInteractable = Interactable;
	TargetInteractionInterface = Interactable;

	//인터랙터블 액터의 상태가 인터랙션 가능한 상태가 아니면
	if (!TargetInteractionInterface->InteractableData.CanInteract)
	{
		//여기 인터랙션 UI 해제 코드 추가 예정
		TargetInteractionInterface->EndFocus();
		return;
	}
	
	//여기 인터랙션 UI 업데이트 코드 추가 예정

	TargetInteractionInterface->BeginFocus();
}

void AThiefPlayer::NotFoundInteractable()
{
	//인터랙션 중이면
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(InteractionTimer);
	}

	//인터랙션 액터 데이터가 있으면
	if (InteractionData.CurrentInteractable) 
	{
		//그 액터가 아직 유효한 액터면
		if (IsValid(TargetInteractionInterface.GetObject()))
		{
			//포커스 끝내기
			TargetInteractionInterface->EndFocus();
		}

		//여기 인터랙션 UI 업데이트 코드 추가 예정

		//인터랙션 액터 데이터 비우기
		InteractionData.CurrentInteractable = nullptr;
		TargetInteractionInterface = nullptr;
	}
}

void AThiefPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("LOOK X: %f, Y: %f"), LookAxisVector.X, LookAxisVector.Y);
	float sen = 1;

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * sen);
		AddControllerPitchInput(LookAxisVector.Y * sen);
	}
}

void AThiefPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("MOVE X: %f, Y: %f"), MovementVector.X, MovementVector.Y);

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AThiefPlayer::Run()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AThiefPlayer::StopRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AThiefPlayer::ToggleInventory()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC)
	{
		//인벤토리가 열려 있으면
		if (IsInventoryOpen)
		{
			FInputModeGameOnly GameOnly;
			IsInventoryOpen = false;
			Inventory->SetVisibility(ESlateVisibility::Collapsed);
			HUD->SetVisibility(ESlateVisibility::Visible);
			PC->bShowMouseCursor = false;
			PC->SetInputMode(GameOnly);
		
		}
		//인벤토리가 닫혀 있으면
		else
		{
			FInputModeUIOnly UIOnly;
			UIOnly.SetWidgetToFocus(Inventory->TakeWidget());
			PC->FlushPressedKeys();
			IsInventoryOpen = true;
			Inventory->SetVisibility(ESlateVisibility::Visible);
			HUD->SetVisibility(ESlateVisibility::Collapsed);
			PC->bShowMouseCursor = true;
			PC->SetInputMode(UIOnly);
		}
	}
}

void AThiefPlayer::BeginInteract()
{
	IInteractionInterface::BeginInteract();
	//인터랙션이 시작됐을 때부터 인터렉션 상태가 변하지 않는 것을 체크
	CheckInteraction();

	//인터랙션 데이터가 있으면
	if (InteractionData.CurrentInteractable)
	{
		//인터랙션 액터가 유효하면
		if (IsValid(TargetInteractionInterface.GetObject()))
		{
			//인터랙션 액터의 인터랙션 시작 함수 실행
			TargetInteractionInterface->BeginInteract();

			//즉시 인터랙션이 가능하면 (꾹 누르는 인터랙션이 아니면)
			if (TargetInteractionInterface->InteractableData.InteractionDuration == 0.0f)
			{
				//인터랙션 가능 상태인지 확인
				if (TargetInteractionInterface->InteractableData.CanInteract)
				{
					//인터랙션 실행
					Interaction();
				}
			}
			//꾹 누르는 인터랙션이면
			else
			{
				//인터랙션 실행 시간 만큼 대기 후 인터랙션 실행
				GetWorldTimerManager().SetTimer(InteractionTimer,
					this,
					&AThiefPlayer::Interaction,
					TargetInteractionInterface->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void AThiefPlayer::EndInteract()
{
	IInteractionInterface::EndInteract();

	//인터랙션 타이머 클리어
	GetWorldTimerManager().ClearTimer(InteractionTimer);

	//인터랙션 액터가 유효한 지 체크
	if (IsValid(TargetInteractionInterface.GetObject()))
	{
		//인터랙션 액터의 인터랙션 종료 함수 실행
		TargetInteractionInterface->EndInteract();
	}
}

void AThiefPlayer::Interaction()
{
	//인터랙션 타이머 클리어
	GetWorldTimerManager().ClearTimer(InteractionTimer);
	
	//인터랙션 액터가 유효한 지 체크
	if (IsValid(TargetInteractionInterface.GetObject()))
	{
		//인터랙션 액터가 인터랙션 가능한 상태이면
		if (TargetInteractionInterface->InteractableData.CanInteract)
		{
			//인터랙션 액터의 인터랙션 함수 실행
			TargetInteractionInterface->Interact(this);
		}
	}
}

void AThiefPlayer::DropItem(class UItemBase* ItemToDrop, const int32 AmountToDrop, bool IsWhole)
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO INVENTORY COMP"));
		return;
	}
	
	if (InventoryComponent->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation(GetActorLocation() + (GetActorForwardVector() * 50.0f));
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
		
		const int32 RemovedAmount = InventoryComponent->RemoveAmountOfItem(ItemToDrop, AmountToDrop);
		
		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);
		
		Pickup->InitializeDrop(ItemToDrop, RemovedAmount);
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("CAN'T FIND MATCHED ITEM."))
	}
}

