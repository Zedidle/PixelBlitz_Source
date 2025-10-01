// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnhancedInputSubsystems.h"
#include "Settings/CustomSettings/PXSettingKeyboardInput.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"
#include "Settings/PXGameSettingRegistry.h"
#include "Player/PXLocalPlayer.h"
#include "Settings/PXSettingsShared.h"
#include "UserSettings/EnhancedInputUserSettings.h"

class ULocalPlayer;

#define LOCTEXT_NAMESPACE "PX"

UGameSettingCollection* UPXGameSettingRegistry::InitializeMouseAndKeyboardSettings(UPXLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("MouseAndKeyboardCollection"));
	Screen->SetDisplayName(LOCTEXT("MouseAndKeyboardCollection_Name", "鼠标与键盘"));
	Screen->Initialize(InLocalPlayer);


	{
		UGameSettingCollection* Sensitivity = NewObject<UGameSettingCollection>();
		Sensitivity->SetDevName(TEXT("MouseSensitivityCollection"));
		Sensitivity->SetDisplayName(LOCTEXT("MouseSensitivityCollection_Name", "Sensitivity"));
		Screen->AddSetting(Sensitivity);

		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MouseSensitivityYaw"));
			Setting->SetDisplayName(LOCTEXT("MouseSensitivityYaw_Name", "X-Axis Sensitivity"));
			Setting->SetDescriptionRichText(LOCTEXT("MouseSensitivityYaw_Description", "Sets the sensitivity of the mouse's horizontal (x) axis. With higher settings the camera will move faster when looking left and right with the mouse."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetMouseSensitivityX));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetMouseSensitivityX));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetMouseSensitivityX());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			Setting->SetMinimumLimit(0.01);
			

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MouseSensitivityPitch"));
			Setting->SetDisplayName(LOCTEXT("MouseSensitivityPitch_Name", "Y-Axis Sensitivity"));
			Setting->SetDescriptionRichText(LOCTEXT("MouseSensitivityPitch_Description", "Sets the sensitivity of the mouse's vertical (y) axis. With higher settings the camera will move faster when looking up and down with the mouse."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetMouseSensitivityY));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetMouseSensitivityY));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetMouseSensitivityY());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			Setting->SetMinimumLimit(0.01);
			

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MouseTargetingMultiplier"));
			Setting->SetDisplayName(LOCTEXT("MouseTargetingMultiplier_Name", "Targeting Sensitivity"));
			Setting->SetDescriptionRichText(LOCTEXT("MouseTargetingMultiplier_Description", "Sets the modifier for reducing mouse sensitivity when targeting. 100% will have no slow down when targeting. Lower settings will have more slow down when targeting."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetTargetingMultiplier));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetTargetingMultiplier));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetTargetingMultiplier());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			Setting->SetMinimumLimit(0.01);
			

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertVerticalAxis"));
			Setting->SetDisplayName(LOCTEXT("InvertVerticalAxis_Name", "Invert Vertical Axis"));
			Setting->SetDescriptionRichText(LOCTEXT("InvertVerticalAxis_Description", "Enable the inversion of the vertical look axis."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertVerticalAxis));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertVerticalAxis));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetInvertVerticalAxis());

			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertHorizontalAxis"));
			Setting->SetDisplayName(LOCTEXT("InvertHorizontalAxis_Name", "Invert Horizontal Axis"));
			Setting->SetDescriptionRichText(LOCTEXT("InvertHorizontalAxis_Description", "Enable the inversion of the Horizontal look axis."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertHorizontalAxis));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertHorizontalAxis));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetInvertHorizontalAxis());



			Sensitivity->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}
	
	{
		UGameSettingCollection* KeyBinding = NewObject<UGameSettingCollection>();
		KeyBinding->SetDevName(TEXT("KeyBindingCollection"));
		KeyBinding->SetDisplayName(LOCTEXT("KeyBindingCollection_Name", "按键绑定"));
		Screen->AddSetting(KeyBinding);

		const UEnhancedInputLocalPlayerSubsystem* EISubsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		const UEnhancedInputUserSettings* UserSettings = EISubsystem->GetUserSettings();

		// If you want to just get one profile pair, then you can do UserSettings->GetCurrentProfile

		// A map of key bindings mapped to their display category
		TMap<FString, UGameSettingCollection*> CategoryToSettingCollection;

		// Returns an existing setting collection for the display category if there is one.
		// If there isn't one, then it will create a new one and initialize it
		auto GetOrCreateSettingCollection = [&CategoryToSettingCollection, &Screen](FText DisplayCategory) -> UGameSettingCollection*
		{
			static const FString DefaultDevName = TEXT("Default_KBM");
			static const FText DefaultDevDisplayName = NSLOCTEXT("PXInputSettings", "PXInputDefaults", "默认");

			if (DisplayCategory.IsEmpty())
			{
				DisplayCategory = DefaultDevDisplayName;
			}
			
			FString DisplayCatString = DisplayCategory.ToString();
			
			if (UGameSettingCollection** ExistingCategory = CategoryToSettingCollection.Find(DisplayCatString))
			{
				return *ExistingCategory;
			}
			
			UGameSettingCollection* ConfigSettingCollection = NewObject<UGameSettingCollection>();
			ConfigSettingCollection->SetDevName(FName(DisplayCatString));
			ConfigSettingCollection->SetDisplayName(DisplayCategory);
			Screen->AddSetting(ConfigSettingCollection);
			CategoryToSettingCollection.Add(DisplayCatString, ConfigSettingCollection);
			
			return ConfigSettingCollection;
		};

		static TSet<FName> CreatedMappingNames;
		CreatedMappingNames.Reset();
		
		for (const TPair<FGameplayTag, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : UserSettings->GetAllSavedKeyProfiles())
		{
			const FGameplayTag& ProfileName = ProfilePair.Key;
			const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

			for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows())
			{
				// Create a setting row for anything with valid mappings and that we haven't created yet
				if (RowPair.Value.HasAnyMappings() /* && !CreatedMappingNames.Contains(RowPair.Key)*/)
				{
					// We only want keyboard keys on this settings screen, so we will filter down by mappings
					// that are set to keyboard keys
					FPlayerMappableKeyQueryOptions Options = {};
					Options.KeyToMatch = EKeys::W;
					Options.bMatchBasicKeyTypes = true;
															
					const FText& DesiredDisplayCategory = RowPair.Value.Mappings.begin()->GetDisplayCategory();
					
					if (UGameSettingCollection* Collection = GetOrCreateSettingCollection(DesiredDisplayCategory))
					{
						// Create the settings widget and initialize it, adding it to this config's section
						UPXSettingKeyboardInput* InputBinding = NewObject<UPXSettingKeyboardInput>();

						InputBinding->InitializeInputData(Profile, RowPair.Value, Options);

						Collection->AddSetting(InputBinding);
						CreatedMappingNames.Add(RowPair.Key);
					}
					else
					{
						ensure(false);
					}
				}
			}
		}
	}

	return Screen;
}

#undef LOCTEXT_NAMESPACE