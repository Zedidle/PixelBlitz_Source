// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PXPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "PXGameplayTags.h"
#include "Character/BasePXCharacter.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "PixelBlitz/PixelBlitz.h"
#include "SaveGame/PXMainSaveGame.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Player/PXCharacterPlayerState.h"
#include "AbilitySystemComponent.h"

class UCustomConfigSettings;
class UEnhancedInputLocalPlayerSubsystem;

void APXPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PXCharacter = Cast<ABasePXCharacter>(GetPawn());

	if (const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>())
	{
		if (const UPXGameDataAsset* GameDataAsset = Settings->GameDataAsset.LoadSynchronous())
		{
			IMC_Default = GameDataAsset->IMC_Default.LoadSynchronous();
			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Subsystem);

			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;
			Subsystem->AddMappingContext(IMC_Default, 1, Options);
		}
	}

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle_OnLevelLoading = MessageSubsystem.RegisterListener(PXGameplayTags::GameplayFlow_OnLevelLoading, this, &ThisClass::OnLevelLoading);
	ListenerHandle_OnLevelLoaded = MessageSubsystem.RegisterListener(PXGameplayTags::GameplayFlow_OnLevelLoaded, this, &ThisClass::OnLevelLoaded);
	
}

void APXPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle_OnLevelLoading);
	MessageSubsystem.UnregisterListener(ListenerHandle_OnLevelLoaded);
}

void APXPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 角色缓存同步：蓝图 Possess 切换角色时角色自己的 BeginPlay/PossessedBy 不一定覆盖到
	// （切换目标可能是 B_SwitchCharacter_* 等 NPC 系，不是 ABasePXCharacter）
	PXCharacter = Cast<ABasePXCharacter>(InPawn);

	// ASC 挂在 PlayerState 上：Avatar 必须重定向到新 Pawn，
	// 否则属性/GE 仍作用于已销毁的旧角色（切换 RedHair → Atalanta 崩溃的直接原因）
	if (APXCharacterPlayerState* PS = GetPlayerState<APXCharacterPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			if (ASC->GetOwner() && ASC->GetOwner() != InPawn)
			{
				ASC->InitAbilityActorInfo(ASC->GetOwner(), InPawn);
			}
		}
	}
}


void APXPlayerController::OnGameStart()
{
	GameAlreadyStart = true;
	OnCharacterControl(true);
	
	if (PXCharacter)
	{
		PXCharacter->ReadyToStart();
	}
}

void APXPlayerController::OnCharacterControl(bool On)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Subsystem);
	
	CharacterControlling = On;
	if (On)
	{
		const FModifyContextOptions Options;
		Subsystem->AddMappingContext(IMC_Default, 1, Options);
		SetShowMouseCursor(false);
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
	else
	{
		Subsystem->RemoveMappingContext(IMC_Default);
		SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
	}
	FlushPressedKeys();
}

bool APXPlayerController::CanPause()
{
	return CharacterControlling && GameAlreadyStart;
}

void APXPlayerController::OnLevelLoading_Implementation(FGameplayTag Channel, const FDefaultEmptyMessage& Message)
{
	UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave);

	OnCharacterControl(false);
}



void APXPlayerController::OnLevelLoaded_Implementation(FGameplayTag Channel, const FDefaultEmptyMessage& Message)
{
	UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave);

	OnCharacterControl(false);
}

