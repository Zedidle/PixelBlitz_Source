// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubtitleDisplayOptions.h"
#include "GameFramework/SaveGame.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXSettingsShared.generated.h"

class UObject;
struct FFrame;



UENUM(BlueprintType)
enum class ECameraFollowMode : uint8
{
	Preview,
	
	Behind
};

UENUM(BlueprintType)
enum class EColorBlindMode : uint8
{
	Off,
	// Deuteranope (green weak/blind)
	Deuteranope,
	// Protanope (red weak/blind)
	Protanope,
	// Tritanope(blue weak / bind)
	Tritanope
};

UENUM(BlueprintType)
enum class EPXAllowBackgroundAudioSetting : uint8
{
	Off,
	AllSounds,

	Num UMETA(Hidden),
};

// UENUM(BlueprintType)
// enum class EPXGamepadSensitivity : uint8
// {
// 	Invalid = 0		UMETA(Hidden),
//
// 	Slow			UMETA(DisplayName = "01 - Slow"),
// 	SlowPlus		UMETA(DisplayName = "02 - Slow+"),
// 	SlowPlusPlus	UMETA(DisplayName = "03 - Slow++"),
// 	Normal			UMETA(DisplayName = "04 - Normal"),
// 	NormalPlus		UMETA(DisplayName = "05 - Normal+"),
// 	NormalPlusPlus	UMETA(DisplayName = "06 - Normal++"),
// 	Fast			UMETA(DisplayName = "07 - Fast"),
// 	FastPlus		UMETA(DisplayName = "08 - Fast+"),
// 	FastPlusPlus	UMETA(DisplayName = "09 - Fast++"),
// 	Insane			UMETA(DisplayName = "10 - Insane"),
//
// 	MAX				UMETA(Hidden),
// };

class UPXLocalPlayer;

UCLASS()
class PIXEL2DKIT_API UPXSettingsShared : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

	public:
	DECLARE_EVENT_OneParam(UPXSettingsShared, FOnSettingChangedEvent, UPXSettingsShared* Settings);
	FOnSettingChangedEvent OnSettingChanged;

public:

	UPXSettingsShared();

	//~ULocalPlayerSaveGame interface
	int32 GetLatestDataVersion() const override;
	//~End of ULocalPlayerSaveGame interface

	bool IsDirty() const { return bIsDirty; }
	void ClearDirtyFlag() { bIsDirty = false; }

	/** Creates a temporary settings object, this will be replaced by one loaded from the user's save game */
	static UPXSettingsShared* CreateTemporarySettings(const UPXLocalPlayer* LocalPlayer);
	
	/** Synchronously loads a settings object, this is not valid to call before login */
	static UPXSettingsShared* LoadOrCreateSettings(const UPXLocalPlayer* LocalPlayer);

	DECLARE_DELEGATE_OneParam(FOnSettingsLoadedEvent, UPXSettingsShared* Settings);

	/** Starts an async load of the settings object, calls Delegate on completion */
	static bool AsyncLoadOrCreateSettings(const UPXLocalPlayer* LocalPlayer, FOnSettingsLoadedEvent Delegate);

	/** Saves the settings to disk */
	void SaveSettings();

	/** Applies the current settings to the player */
	void ApplySettings();
	
public:
	////////////////////////////////////////////////////////
	// Color Blind Options

	UFUNCTION()
	EColorBlindMode GetColorBlindMode() const;
	UFUNCTION()
	void SetColorBlindMode(EColorBlindMode InMode);

	UFUNCTION()
	int32 GetColorBlindStrength() const;
	UFUNCTION()
	void SetColorBlindStrength(int32 InColorBlindStrength);

private:
	UPROPERTY()
	EColorBlindMode ColorBlindMode = EColorBlindMode::Off;

	UPROPERTY()
	int32 ColorBlindStrength = 10;

    ////////////////////////////////////////////////////////
	// Gamepad Vibration
public:
	UFUNCTION()
	bool GetForceFeedbackEnabled() const { return bForceFeedbackEnabled; }

	UFUNCTION()
	void SetForceFeedbackEnabled(const bool NewValue);

	
private:
	/** Is force feedback enabled when a controller is being used? */
	UPROPERTY()
	bool bForceFeedbackEnabled = true;

	////////////////////////////////////////////////////////
	// Gamepad Deadzone
public:
	/** Getter for gamepad move stick dead zone value. */
	UFUNCTION()
	float GetGamepadMoveStickDeadZone() const { return GamepadMoveStickDeadZone; }

	/** Setter for gamepad move stick dead zone value. */
	UFUNCTION()
	void SetGamepadMoveStickDeadZone(const float NewValue) { ChangeValueAndDirty(GamepadMoveStickDeadZone, NewValue); }

	/** Getter for gamepad look stick dead zone value. */
	UFUNCTION()
	float GetGamepadLookStickDeadZone() const { return GamepadLookStickDeadZone; }

	/** Setter for gamepad look stick dead zone value. */
	UFUNCTION()
	void SetGamepadLookStickDeadZone(const float NewValue) { ChangeValueAndDirty(GamepadLookStickDeadZone, NewValue); }

private:
	/** Holds the gamepad move stick dead zone value. */
	UPROPERTY()
	float GamepadMoveStickDeadZone;

	/** Holds the gamepad look stick dead zone value. */
	UPROPERTY()
	float GamepadLookStickDeadZone;

	////////////////////////////////////////////////////////
	// Gamepad Trigger Haptics
public:
	UFUNCTION()
	bool GetTriggerHapticsEnabled() const { return bTriggerHapticsEnabled; }
	UFUNCTION()
	void SetTriggerHapticsEnabled(const bool NewValue) { ChangeValueAndDirty(bTriggerHapticsEnabled, NewValue); }

	UFUNCTION()
	bool GetTriggerPullUsesHapticThreshold() const { return bTriggerPullUsesHapticThreshold; }
	UFUNCTION()
	void SetTriggerPullUsesHapticThreshold(const bool NewValue) { ChangeValueAndDirty(bTriggerPullUsesHapticThreshold, NewValue); }

	UFUNCTION()
	uint8 GetTriggerHapticStrength() const { return TriggerHapticStrength; }
	UFUNCTION()
	void SetTriggerHapticStrength(const uint8 NewValue) { ChangeValueAndDirty(TriggerHapticStrength, NewValue); }

	UFUNCTION()
	uint8 GetTriggerHapticStartPosition() const { return TriggerHapticStartPosition; }
	UFUNCTION()
	void SetTriggerHapticStartPosition(const uint8 NewValue) { ChangeValueAndDirty(TriggerHapticStartPosition, NewValue); }
	
private:
	/** Are trigger haptics enabled? */
	UPROPERTY()
	bool bTriggerHapticsEnabled = false;
	/** Does the game use the haptic feedback as its threshold for judging button presses? */
	UPROPERTY()
	bool bTriggerPullUsesHapticThreshold = true;
	/** The strength of the trigger haptic effects. */
	UPROPERTY()
	uint8 TriggerHapticStrength = 8;
	/** The start position of the trigger haptic effects */
	UPROPERTY()
	uint8 TriggerHapticStartPosition = 0;

	////////////////////////////////////////////////////////
	// Subtitles
public:
	UFUNCTION()
	bool GetSubtitlesEnabled() const { return bEnableSubtitles; }
	UFUNCTION()
	void SetSubtitlesEnabled(bool Value) { ChangeValueAndDirty(bEnableSubtitles, Value); }

	UFUNCTION()
	ESubtitleDisplayTextSize GetSubtitlesTextSize() const { return SubtitleTextSize; }
	UFUNCTION()
	void SetSubtitlesTextSize(ESubtitleDisplayTextSize Value) { ChangeValueAndDirty(SubtitleTextSize, Value); ApplySubtitleOptions(); }

	UFUNCTION()
	ESubtitleDisplayTextColor GetSubtitlesTextColor() const { return SubtitleTextColor; }
	UFUNCTION()
	void SetSubtitlesTextColor(ESubtitleDisplayTextColor Value) { ChangeValueAndDirty(SubtitleTextColor, Value); ApplySubtitleOptions(); }

	UFUNCTION()
	ESubtitleDisplayTextBorder GetSubtitlesTextBorder() const { return SubtitleTextBorder; }
	UFUNCTION()
	void SetSubtitlesTextBorder(ESubtitleDisplayTextBorder Value) { ChangeValueAndDirty(SubtitleTextBorder, Value); ApplySubtitleOptions(); }

	UFUNCTION()
	ESubtitleDisplayBackgroundOpacity GetSubtitlesBackgroundOpacity() const { return SubtitleBackgroundOpacity; }
	UFUNCTION()
	void SetSubtitlesBackgroundOpacity(ESubtitleDisplayBackgroundOpacity Value) { ChangeValueAndDirty(SubtitleBackgroundOpacity, Value); ApplySubtitleOptions(); }

	void ApplySubtitleOptions();

private:
	UPROPERTY()
	bool bEnableSubtitles = true;

	UPROPERTY()
	ESubtitleDisplayTextSize SubtitleTextSize = ESubtitleDisplayTextSize::Medium;

	UPROPERTY()
	ESubtitleDisplayTextColor SubtitleTextColor = ESubtitleDisplayTextColor::White;

	UPROPERTY()
	ESubtitleDisplayTextBorder SubtitleTextBorder = ESubtitleDisplayTextBorder::None;

	UPROPERTY()
	ESubtitleDisplayBackgroundOpacity SubtitleBackgroundOpacity = ESubtitleDisplayBackgroundOpacity::Medium;

	////////////////////////////////////////////////////////
	// Shared audio settings
public:
	UFUNCTION()
	EPXAllowBackgroundAudioSetting GetAllowAudioInBackgroundSetting() const { return AllowAudioInBackground; }
	UFUNCTION()
	void SetAllowAudioInBackgroundSetting(EPXAllowBackgroundAudioSetting NewValue);

	void ApplyBackgroundAudioSettings();

private:
	UPROPERTY()
	EPXAllowBackgroundAudioSetting AllowAudioInBackground = EPXAllowBackgroundAudioSetting::Off;

	////////////////////////////////////////////////////////
	// Culture / language
public:
	/** Gets the pending culture */
	const FString& GetPendingCulture() const;

	/** Sets the pending culture to apply */
	void SetPendingCulture(const FString& NewCulture);

	// Called when the culture changes.
	void OnCultureChanged();

	/** Clears the pending culture to apply */
	void ClearPendingCulture();

	bool IsUsingDefaultCulture() const;

	void ResetToDefaultCulture();
	bool ShouldResetToDefaultCulture() const { return bResetToDefaultCulture; }
	
	void ApplyCultureSettings();
	void ResetCultureToCurrentSettings();

private:
	/** The pending culture to apply */
	UPROPERTY(Transient)
	FString PendingCulture;

	/* If true, resets the culture to default. */
	bool bResetToDefaultCulture = false;



#pragma region Gameplay Settings

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetShowHP() const { return bShowHP; }
	UFUNCTION()
	void SetShowHP(bool Value) { ChangeValueAndDirty(bShowHP, Value); }
	
	UFUNCTION()
	bool GetShowDamageText() const { return bShowDamageText; }
	UFUNCTION()
	void SetShowDamageText(bool Value) { ChangeValueAndDirty(bShowDamageText, Value); }

	UFUNCTION()
	bool GetShowRecoverText() const { return bShowRecoverText; }
	UFUNCTION()
	void SetShowRecoverText(bool Value) { ChangeValueAndDirty(bShowRecoverText, Value); }
	
	UFUNCTION()
	bool GetShowGoldsText() const { return bShowGoldsText; }
	UFUNCTION()
	void SetShowGoldsText(bool Value) { ChangeValueAndDirty(bShowGoldsText, Value); }

	UFUNCTION()
	bool GetShowBloodVFX() const { return bShowBloodVFX; }
	UFUNCTION()
	void SetShowBloodVFX(bool Value) { ChangeValueAndDirty(bShowBloodVFX, Value); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EStyleType GetDrawStyle() const { return DrawStyle; }
	UFUNCTION()
	void SetDrawStyle(EStyleType Value) { ChangeValueAndDirty(DrawStyle, Value); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECameraColorFilter GetCameraColorFilter() const { return CameraColorFilter; }
	UFUNCTION()
	void SetCameraColorFilter(ECameraColorFilter Value) { ChangeValueAndDirty(CameraColorFilter, Value); }
	
	UFUNCTION()
	ECameraFollowMode GetCameraFollowMode() const { return CameraFollowMode; }
	UFUNCTION()
	void SetCameraFollowMode(ECameraFollowMode InMode);

	UFUNCTION()
	float GetCameraOffsetSpeed() const { return CameraOffsetSpeed; }
	UFUNCTION()
	void SetCameraOffsetSpeed(float Value);
	
	
	void ApplyGameplayOptions();

private:

	UPROPERTY()
	bool bShowHP = true;
	
	UPROPERTY()
	bool bShowDamageText = true;

	UPROPERTY()
	bool bShowRecoverText = true;
	
	UPROPERTY()
	bool bShowGoldsText = true;
	
	UPROPERTY()
	bool bShowBloodVFX = true;

	UPROPERTY()
	EStyleType DrawStyle = EStyleType::Cartoon;

	UPROPERTY()
	ECameraColorFilter CameraColorFilter = ECameraColorFilter::MoonlitNight;
	
	UPROPERTY()
	ECameraFollowMode CameraFollowMode = ECameraFollowMode::Preview;

	UPROPERTY()
	float CameraOffsetSpeed = 2.0f;
	
#pragma endregion


	
	////////////////////////////////////////////////////////
	// Gamepad Sensitivity
public:
	UFUNCTION()
	double GetViewPointSensitivityPitch_Mouse() const { return ViewPointSensitivityPitch_Mouse; }
	UFUNCTION()
	void SetViewPointSensitivityPitch_Mouse(double NewValue) { ChangeValueAndDirty(ViewPointSensitivityPitch_Mouse, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	double GetViewPointSensitivityYaw_Mouse() const { return ViewPointSensitivityYaw_Mouse; }
	UFUNCTION()
	void SetViewPointSensitivityYaw_Mouse(double NewValue) { ChangeValueAndDirty(ViewPointSensitivityYaw_Mouse, NewValue); ApplyInputSensitivity(); }
	
	UFUNCTION()
	bool GetInvertVerticalAxis_Mouse() const { return bInvertVerticalAxis_Mouse; }
	UFUNCTION()
	void SetInvertVerticalAxis_Mouse(bool NewValue) { ChangeValueAndDirty(bInvertVerticalAxis_Mouse, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	bool GetInvertHorizontalAxis_Mouse() const { return bInvertHorizontalAxis_Mouse; }
	UFUNCTION()
	void SetInvertHorizontalAxis_Mouse(bool NewValue) { ChangeValueAndDirty(bInvertHorizontalAxis_Mouse, NewValue); ApplyInputSensitivity(); }


	
	UFUNCTION()
	bool GetInvertVerticalAxis_Gamepad() const { return bInvertVerticalAxis_Gamepad; }
	UFUNCTION()
	void SetInvertVerticalAxis_Gamepad(bool NewValue);

	UFUNCTION()
	bool GetInvertHorizontalAxis_Gamepad() const { return bInvertHorizontalAxis_Gamepad; }
	UFUNCTION()
	void SetInvertHorizontalAxis_Gamepad(bool NewValue) { ChangeValueAndDirty(bInvertHorizontalAxis_Gamepad, NewValue); ApplyInputSensitivity(); }
	

	
private:

	// Mouse
	UPROPERTY()
	double ViewPointSensitivityPitch_Mouse = 0.1;
	UPROPERTY()
	double ViewPointSensitivityYaw_Mouse = 0.1;
	UPROPERTY()
	bool bInvertVerticalAxis_Mouse = false;
	UPROPERTY()
	bool bInvertHorizontalAxis_Mouse = false;

	
	// Gamepad
	UPROPERTY()
	bool bInvertVerticalAxis_Gamepad = false;
	UPROPERTY()
	bool bInvertHorizontalAxis_Gamepad = false;
	UPROPERTY()
	float GamepadLookSensitivity = 1;
	
public:
	UFUNCTION()
	float GetGamepadLookSensitivity() const { return GamepadLookSensitivity; }
	UFUNCTION()
	void SetGamepadLookSensitivity(float NewValue) { ChangeValueAndDirty(GamepadLookSensitivity, NewValue); ApplyInputSensitivity(); }
	
	void ApplyInputSensitivity();

	
	////////////////////////////////////////////////////////
	/// Dirty and Change Reporting
private:
	template<typename T>
	bool ChangeValueAndDirty(T& CurrentValue, const T& NewValue)
	{
		if (CurrentValue != NewValue)
		{
			CurrentValue = NewValue;
			bIsDirty = true;
			OnSettingChanged.Broadcast(this);
			
			return true;
		}

		return false;
	}

	bool bIsDirty = false;
	
};
