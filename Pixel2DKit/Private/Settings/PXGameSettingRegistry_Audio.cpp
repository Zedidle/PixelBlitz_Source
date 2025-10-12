// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/CustomSettings/PXSettingValueDiscreteDynamic_AudioOutputDevice.h"
#include "DataSource/GameSettingDataSource.h"
#include "EditCondition/WhenCondition.h"
#include "EditCondition/WhenPlatformHasTrait.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "GameSettingCollection.h"
#include "GameSettingValueScalarDynamic.h"
#include "Settings/PXGameSettingRegistry.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/PXSettingsShared.h"
#include "NativeGameplayTags.h"
#include "Player/PXLocalPlayer.h"

class ULocalPlayer;

#define LOCTEXT_NAMESPACE "PX"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsChangingAudioOutputDevice, "Platform.Trait.SupportsChangingAudioOutputDevice");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsBackgroundAudio, "Platform.Trait.SupportsBackgroundAudio");

UGameSettingCollection* UPXGameSettingRegistry::InitializeAudioSettings(UPXLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("AudioCollection"));
	Screen->SetDisplayName(LOCTEXT("AudioCollection_Name", "音频"));
	Screen->Initialize(InLocalPlayer);

	// Volume
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* Volume = NewObject<UGameSettingCollection>();
		Volume->SetDevName(TEXT("VolumeCollection"));
		Volume->SetDisplayName(LOCTEXT("VolumeCollection_Name", "音量控制"));
		Screen->AddSetting(Volume);
	
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("OverallVolume"));
			Setting->SetDisplayName(LOCTEXT("OverallVolume_Name", "总音量"));
			Setting->SetDescriptionRichText(LOCTEXT("OverallVolume_Desc", "调节总体的音量."));
	
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetOverallVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetOverallVolume));
			Setting->SetDefaultValue(GetDefault<UPXSettingsLocal>()->GetOverallVolume());

			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0, 5), 0.01);
			
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
	
			Volume->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MusicVolume"));
			Setting->SetDisplayName(LOCTEXT("MusicVolume_Name", "音乐音量"));
			Setting->SetDescriptionRichText(LOCTEXT("MusicVolume_Desc", "调节背景音乐的音量"));
	
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMusicVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMusicVolume));
			Setting->SetDefaultValue(GetDefault<UPXSettingsLocal>()->GetMusicVolume());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
	
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
	
			Volume->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("SoundEffectsUIVolume"));
			Setting->SetDisplayName(LOCTEXT("SoundEffectsUIVolume_Name", "UI效果音量"));
			Setting->SetDescriptionRichText(LOCTEXT("SoundEffectsUIVolume_Desc", "调节UI音效的音量"));
	
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSoundUIVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSoundUIVolume));
			Setting->SetDefaultValue(GetDefault<UPXSettingsLocal>()->GetSoundUIVolume());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
	
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
	
			Volume->AddSetting(Setting);
		}
		
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("SoundEffectsBattleVolume"));
			Setting->SetDisplayName(LOCTEXT("SoundEffectsBattleVolume_Name", "战斗效果音效"));
			Setting->SetDescriptionRichText(LOCTEXT("SoundEffectsBattleVolume_Desc", "调节战斗音效的音量"));
	
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSoundFXVolume));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSoundFXVolume));
			Setting->SetDefaultValue(GetDefault<UPXSettingsLocal>()->GetSoundFXVolume());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
	
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
	
			Volume->AddSetting(Setting);
		}
	}
	
	{
		UGameSettingCollection* Sound = NewObject<UGameSettingCollection>();
		Sound->SetDevName(TEXT("SoundCollection"));
		Sound->SetDisplayName(LOCTEXT("SoundCollection_Name", "声音处理"));
		Screen->AddSetting(Sound);
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("BackgroundAudio"));
			Setting->SetDisplayName(LOCTEXT("BackgroundAudio_Name", "背景音"));
			Setting->SetDescriptionRichText(LOCTEXT("BackgroundAudio_Desc", "当游戏处于后台时，开启或关闭游戏音频。当开启时，即使游戏被最小化或另一个窗口获得焦点，游戏音频仍会继续播放。"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetAllowAudioInBackgroundSetting));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetAllowAudioInBackgroundSetting));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetAllowAudioInBackgroundSetting());

			Setting->AddEnumOption(EPXAllowBackgroundAudioSetting::Off, LOCTEXT("EPXAllowBackgroundAudioSetting_Off", "关闭"));
			Setting->AddEnumOption(EPXAllowBackgroundAudioSetting::AllSounds, LOCTEXT("EPXAllowBackgroundAudioSetting_AllSounds", "所有音量"));

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(
				TAG_Platform_Trait_SupportsBackgroundAudio, TEXT("Platform does not support background audio"))
			);

			Sound->AddSetting(Setting);
		}
	}

	return Screen;
}

#undef LOCTEXT_NAMESPACE
