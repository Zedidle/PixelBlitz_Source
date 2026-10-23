// Copyright Epic Games, Inc. All Rights Reserved.

#include "PXSettingKeyboardInput.h"

#include "EnhancedInputSubsystems.h"
#include "Player/PXLocalPlayer.h"
#include "PlayerMappableInputConfig.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXSettingKeyboardInput)

class ULocalPlayer;

#define LOCTEXT_NAMESPACE "PXSettings"

namespace PX::ErrorMessages
{
	static const FText UnknownMappingName = LOCTEXT("PXErrors_UnknownMappingName", "Unknown Mapping");
}


UPXSettingKeyboardInput::UPXSettingKeyboardInput()
{
	bReportAnalytics = false;
}

FText UPXSettingKeyboardInput::GetSettingDisplayName() const
{
	if (const FKeyMappingRow* Row = FindKeyMappingRow())
	{
		if (Row->HasAnyMappings())
		{
			return Row->Mappings.begin()->GetDisplayName();
		}
	}
	
	return PX::ErrorMessages::UnknownMappingName;
}

FText UPXSettingKeyboardInput::GetSettingDisplayCategory() const
{
	if (const FKeyMappingRow* Row = FindKeyMappingRow())
	{
		if (Row->HasAnyMappings())
		{
			return Row->Mappings.begin()->GetDisplayCategory();
		}
	}

	return PX::ErrorMessages::UnknownMappingName;
}

const FKeyMappingRow* UPXSettingKeyboardInput::FindKeyMappingRow() const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		return Profile->FindKeyMappingRow(ActionMappingName);
	}

	ensure(false);
	return nullptr;
}

UEnhancedPlayerMappableKeyProfile* UPXSettingKeyboardInput::FindMappableKeyProfile() const
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings())
	{
		return Settings->GetKeyProfileWithId(ProfileIdentifier);
	}
	
	ensure(false);
	return nullptr;
}

UEnhancedInputUserSettings* UPXSettingKeyboardInput::GetUserSettings() const
{
	if(UPXLocalPlayer* PXLocalPlayer = Cast<UPXLocalPlayer>(LocalPlayer))
	{
		// Map the key to the player key profile
		if (UEnhancedInputLocalPlayerSubsystem* System = PXLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			return System->GetUserSettings();
		}
	}
	
	return nullptr;
}

void UPXSettingKeyboardInput::OnInitialized()
{
	DynamicDetails = FGetGameSettingsDetails::CreateLambda([this](ULocalPlayer&)
	{
		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			if (Row->HasAnyMappings())
			{
				return FText::Format(LOCTEXT("DynamicDetails_KeyboardInputAction", "为 {0} 绑定按键"), Row->Mappings.begin()->GetDisplayName());
			}
		}
		return FText::GetEmpty();
	});

	Super::OnInitialized();
}

void UPXSettingKeyboardInput::InitializeInputData(const UEnhancedPlayerMappableKeyProfile* KeyProfile, const FKeyMappingRow& MappingData, const FPlayerMappableKeyQueryOptions& InQueryOptions)
{
	check(KeyProfile);

	ProfileIdentifier = KeyProfile->GetProfileIdString();
	QueryOptions = InQueryOptions;
	
	for (const FPlayerKeyMapping& Mapping : MappingData.Mappings)
	{
		// Only add mappings that pass the query filters that have been provided upon creation
		if (!KeyProfile->DoesMappingPassQueryOptions(Mapping, QueryOptions))
		{
			continue;
		}
		
		ActionMappingName = Mapping.GetMappingName();
		InitialKeyMappings.Add(Mapping.GetSlot(), Mapping.GetCurrentKey());
		const FText& MappingDisplayName =Mapping.GetDisplayName();
		
		if (!MappingDisplayName.IsEmpty())
		{
			SetDisplayName(MappingDisplayName);	
		}
	}
	
	const FString NameString = TEXT("KBM_Input_") + ActionMappingName.ToString();
	SetDevName(*NameString);
}

FText UPXSettingKeyboardInput::GetKeyTextFromSlot(const EPlayerMappableKeySlot InSlot) const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;
		QueryOptionsForSlot.SlotToMatch = InSlot;
		
		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
				{
					return Mapping.GetCurrentKey().GetDisplayName();
				}
			}
		}
	}
	
	return EKeys::Invalid.GetDisplayName();
}

void UPXSettingKeyboardInput::ResetToDefault()
{	
	if (UEnhancedInputUserSettings* Settings = GetUserSettings())
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;

		FGameplayTagContainer FailureReason;
		Settings->ResetAllPlayerKeysInRow(Args, FailureReason);

		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

void UPXSettingKeyboardInput::StoreInitial()
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		if(const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptions))
				{
					ActionMappingName = Mapping.GetMappingName();
					InitialKeyMappings.Add(Mapping.GetSlot(), Mapping.GetCurrentKey());	
				}
			}
		}
	}
}

void UPXSettingKeyboardInput::RestoreToInitial()
{
	for (TPair<EPlayerMappableKeySlot, FKey> Pair : InitialKeyMappings)
	{
		ChangeBinding((int32)Pair.Key, Pair.Value);	
	}
}

bool UPXSettingKeyboardInput::ChangeBinding(int32 InKeyBindSlot, FKey NewKey)
{
	if (!NewKey.IsGamepadKey())
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;
		Args.Slot = (EPlayerMappableKeySlot) (static_cast<uint8>(InKeyBindSlot));
		Args.NewKey = NewKey;
		// If you want to, you can additionally specify this mapping to only be applied to a certain hardware device or key profile
		//Args.ProfileId =
		//Args.HardwareDeviceId =
		
		if (UEnhancedInputUserSettings* Settings = GetUserSettings())
		{
			FGameplayTagContainer FailureReason;
			Settings->MapPlayerKey(Args, FailureReason);
			NotifySettingChanged(EGameSettingChangeReason::Change);
		}

		return true;
	}

	return false;
}

void UPXSettingKeyboardInput::GetAllMappedActionsFromKey(int32 InKeyBindSlot, FKey Key, TArray<FName>& OutActionNames) const
{	
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{		
		Profile->GetMappingNamesForKey(Key, OutActionNames);
	}
}

bool UPXSettingKeyboardInput::IsMappingCustomized() const
{
	bool bResult = false;

	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;

		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
				{
					bResult |= Mapping.IsCustomized();
				}
			}
		}
	}

	return bResult;
}

#undef LOCTEXT_NAMESPACE

