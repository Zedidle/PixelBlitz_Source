// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/PXSettingsShared.h"
#include "Framework/Application/SlateApplication.h"
#include "Internationalization/Culture.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "Misc/ConfigCacheIni.h"
#include "Player/PXLocalPlayer.h"
#include "Rendering/SlateRenderer.h"
#include "SubtitleDisplaySubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Utilitys/DebugFuncLab.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXSettingsShared)

static FString SHARED_SETTINGS_SLOT_NAME = TEXT("SharedGameSettings");

namespace PXSettingsSharedCVars
{
	static float DefaultGamepadLeftStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadLeftStickInnerDeadZone(
		TEXT("gpad.DefaultLeftStickInnerDeadZone"),
		DefaultGamepadLeftStickInnerDeadZone,
		TEXT("Gamepad left stick inner deadzone")
	);

	static float DefaultGamepadRightStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadRightStickInnerDeadZone(
		TEXT("gpad.DefaultRightStickInnerDeadZone"),
		DefaultGamepadRightStickInnerDeadZone,
		TEXT("Gamepad right stick inner deadzone")
	);	
}

UPXSettingsShared::UPXSettingsShared()
{
	FInternationalization::Get().OnCultureChanged().AddUObject(this, &ThisClass::OnCultureChanged);

	GamepadMoveStickDeadZone = PXSettingsSharedCVars::DefaultGamepadLeftStickInnerDeadZone;
	GamepadLookStickDeadZone = PXSettingsSharedCVars::DefaultGamepadRightStickInnerDeadZone;
}

int32 UPXSettingsShared::GetLatestDataVersion() const
{
	// 0 = before subclassing ULocalPlayerSaveGame
	// 1 = first proper version
	return 1;
}

UPXSettingsShared* UPXSettingsShared::CreateTemporarySettings(const UPXLocalPlayer* LocalPlayer)
{
	// This is not loaded from disk but should be set up to save
	UPXSettingsShared* SharedSettings = Cast<UPXSettingsShared>(CreateNewSaveGameForLocalPlayer(UPXSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME));

	SharedSettings->ApplySettings();

	return SharedSettings;
}

UPXSettingsShared* UPXSettingsShared::LoadOrCreateSettings(const UPXLocalPlayer* LocalPlayer)
{
	// This will stall the main thread while it loads
	UPXSettingsShared* SharedSettings = Cast<UPXSettingsShared>(LoadOrCreateSaveGameForLocalPlayer(UPXSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME));

	SharedSettings->ApplySettings();

	return SharedSettings;
}

bool UPXSettingsShared::AsyncLoadOrCreateSettings(const UPXLocalPlayer* LocalPlayer, FOnSettingsLoadedEvent Delegate)
{
	FOnLocalPlayerSaveGameLoadedNative Lambda = FOnLocalPlayerSaveGameLoadedNative::CreateLambda([Delegate]
		(ULocalPlayerSaveGame* LoadedSave)
		{
			UPXSettingsShared* LoadedSettings = CastChecked<UPXSettingsShared>(LoadedSave);
			
			LoadedSettings->ApplySettings();

			Delegate.ExecuteIfBound(LoadedSettings);
		});

	return ULocalPlayerSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(UPXSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME, Lambda);
}

void UPXSettingsShared::SaveSettings()
{
	// Schedule an async save because it's okay if it fails
	AsyncSaveGameToSlotForLocalPlayer();

	// TODO_BH: Move this to the serialize function instead with a bumped version number
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->AsyncSaveSettings();
		}
	}
}

void UPXSettingsShared::ApplySettings()
{
	ApplySubtitleOptions();
	ApplyBackgroundAudioSettings();
	ApplyCultureSettings();

	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->ApplySettings();
		}
	}
}

void UPXSettingsShared::SetColorBlindStrength(int32 InColorBlindStrength)
{
	InColorBlindStrength = FMath::Clamp(InColorBlindStrength, 0, 10);
	if (ColorBlindStrength != InColorBlindStrength)
	{
		ColorBlindStrength = InColorBlindStrength;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}

void UPXSettingsShared::SetForceFeedbackEnabled(const bool NewValue)
{
	
	UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("UPXSettingsShared::SetForceFeedbackEnabled: %i"), NewValue));
	
	ChangeValueAndDirty(bForceFeedbackEnabled, NewValue);
}

int32 UPXSettingsShared::GetColorBlindStrength() const
{
	return ColorBlindStrength;
}

void UPXSettingsShared::SetColorBlindMode(EColorBlindMode InMode)
{
	if (ColorBlindMode != InMode)
	{
		ColorBlindMode = InMode;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}

EColorBlindMode UPXSettingsShared::GetColorBlindMode() const
{
	return ColorBlindMode;
}

void UPXSettingsShared::ApplySubtitleOptions()
{
	if (USubtitleDisplaySubsystem* SubtitleSystem = USubtitleDisplaySubsystem::Get(OwningPlayer))
	{
		FSubtitleFormat SubtitleFormat;
		SubtitleFormat.SubtitleTextSize = SubtitleTextSize;
		SubtitleFormat.SubtitleTextColor = SubtitleTextColor;
		SubtitleFormat.SubtitleTextBorder = SubtitleTextBorder;
		SubtitleFormat.SubtitleBackgroundOpacity = SubtitleBackgroundOpacity;

		SubtitleSystem->SetSubtitleDisplayOptions(SubtitleFormat);
	}
}

//////////////////////////////////////////////////////////////////////

void UPXSettingsShared::SetAllowAudioInBackgroundSetting(EPXAllowBackgroundAudioSetting NewValue)
{
	if (ChangeValueAndDirty(AllowAudioInBackground, NewValue))
	{
		ApplyBackgroundAudioSettings();
	}
}

void UPXSettingsShared::ApplyBackgroundAudioSettings()
{
	if (OwningPlayer && OwningPlayer->IsPrimaryPlayer())
	{
		FApp::SetUnfocusedVolumeMultiplier((AllowAudioInBackground != EPXAllowBackgroundAudioSetting::Off) ? 1.0f : 0.0f);
	}
}

//////////////////////////////////////////////////////////////////////

void UPXSettingsShared::ApplyCultureSettings()
{
	if (bResetToDefaultCulture)
	{
		const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
		check(SystemDefaultCulture.IsValid());

		const FString CultureToApply = SystemDefaultCulture->GetName();
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Clear this string
			GConfig->RemoveKey(TEXT("Internationalization"), TEXT("Culture"), GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		bResetToDefaultCulture = false;
	}
	else if (!PendingCulture.IsEmpty())
	{
		// SetCurrentCulture may trigger PendingCulture to be cleared (if a culture change is broadcast) so we take a copy of it to work with
		const FString CultureToApply = PendingCulture;
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Note: This is intentionally saved to the users config
			// We need to localize text before the player logs in and very early in the loading screen
			GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CultureToApply, GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		ClearPendingCulture();
	}
}

void UPXSettingsShared::ResetCultureToCurrentSettings()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

void UPXSettingsShared::SetCameraFollowMode(ECameraFollowMode InMode)
{
	if (CameraFollowMode != InMode)
	{
		CameraFollowMode = InMode;
	}
}

void UPXSettingsShared::SetCameraOffsetSpeed(float Value)
{
	CameraOffsetSpeed = Value;
}

void UPXSettingsShared::ApplyGameplayOptions()
{
}

const FString& UPXSettingsShared::GetPendingCulture() const
{
	return PendingCulture;
}

void UPXSettingsShared::SetPendingCulture(const FString& NewCulture)
{
	PendingCulture = NewCulture;
	bResetToDefaultCulture = false;
	bIsDirty = true;
}

void UPXSettingsShared::OnCultureChanged()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

void UPXSettingsShared::ClearPendingCulture()
{
	PendingCulture.Reset();
}

bool UPXSettingsShared::IsUsingDefaultCulture() const
{
	FString Culture;
	GConfig->GetString(TEXT("Internationalization"), TEXT("Culture"), Culture, GGameUserSettingsIni);

	return Culture.IsEmpty();
}

void UPXSettingsShared::ResetToDefaultCulture()
{
	ClearPendingCulture();
	bResetToDefaultCulture = true;
	bIsDirty = true;
}

//////////////////////////////////////////////////////////////////////

void UPXSettingsShared::SetInvertVerticalAxis_Gamepad(bool NewValue)
{
	ChangeValueAndDirty(bInvertVerticalAxis_Gamepad, NewValue); ApplyInputSensitivity(); 
}

void UPXSettingsShared::ApplyInputSensitivity()
{
	
}

