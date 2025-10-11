// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/PXGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "Settings/CustomSettings/PXSettingValueDiscrete_Language.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "Player/PXLocalPlayer.h"
#include "Settings/PXSettingsShared.h"


#define LOCTEXT_NAMESPACE "PX"

UGameSettingCollection* UPXGameSettingRegistry::InitializeGameplaySettings(UPXLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("GameplayCollection"));
	Screen->SetDisplayName(LOCTEXT("GameplayCollection_Name", "游戏"));
	Screen->Initialize(InLocalPlayer);

	{
		UGameSettingCollection* LanguageSubsection = NewObject<UGameSettingCollection>();
		LanguageSubsection->SetDevName(TEXT("CultureCollection"));
		LanguageSubsection->SetDisplayName(LOCTEXT("CultureCollection_Name", "文化"));
		Screen->AddSetting(LanguageSubsection);

		//----------------------------------------------------------------------------------
		{
			UPXSettingValueDiscrete_Language* Setting = NewObject<UPXSettingValueDiscrete_Language>();
			Setting->SetDevName(TEXT("Culture_Language"));
			Setting->SetDisplayName(LOCTEXT("Culture_Language_Name", "语言"));
			Setting->SetDescriptionRichText(LOCTEXT("Culture_Language_Desc", "选择游戏语言"));

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			LanguageSubsection->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}

	{
		UGameSettingCollection* GameplayTextCollection = NewObject<UGameSettingCollection>();
		GameplayTextCollection->SetDevName(TEXT("GameplayTextCollection"));
		GameplayTextCollection->SetDisplayName(LOCTEXT("GameplayTextCollection_Name", "文本与UI"));
		Screen->AddSetting(GameplayTextCollection);

		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("GameplayText_ShowHP"));
			Setting->SetDisplayName(LOCTEXT("GameplayText_ShowHP_Name", "显示小怪生命值"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayText_ShowHP_Desc", "显示小怪生命值血条的开关."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetShowHP));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetShowHP));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetShowHP());

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			GameplayTextCollection->AddSetting(Setting);
		}
		
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("GameplayText_ShowDamage"));
			Setting->SetDisplayName(LOCTEXT("GameplayText_ShowDamage_Name", "显示伤害值"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayText_ShowDamage_Desc", "显示伤害值文本的开关."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetShowDamageText));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetShowDamageText));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetShowDamageText());

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			GameplayTextCollection->AddSetting(Setting);
		}

		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("GameplayText_ShowRecover"));
			Setting->SetDisplayName(LOCTEXT("GameplayText_ShowRecover_Name", "显示恢复值"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayText_ShowRecover_Desc", "显示生命值与体力值恢复量文本的开关."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetShowRecoverText));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetShowRecoverText));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetShowRecoverText());

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			GameplayTextCollection->AddSetting(Setting);
		}
		
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("GameplayText_ShowGolds"));
			Setting->SetDisplayName(LOCTEXT("GameplayText_ShowGolds_Name", "显示金币值"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayText_ShowGolds_Desc", "显示获取金币值文本的开关."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetShowDamageText));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetShowDamageText));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetShowDamageText());

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			
			GameplayTextCollection->AddSetting(Setting);
		}
	}


	{
		UGameSettingCollection* GameplayVFXCollection = NewObject<UGameSettingCollection>();
		GameplayVFXCollection->SetDevName(TEXT("GameplayVFXCollection"));
		GameplayVFXCollection->SetDisplayName(LOCTEXT("GameplayVFXCollection_Name", "视觉特效"));
		Screen->AddSetting(GameplayVFXCollection);

		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("GameplayVFX_ShowBlood"));
			Setting->SetDisplayName(LOCTEXT("GameplayVFX_ShowBlood_Name", "显示掉血效果"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayVFX_ShowBlood_Desc", "任意生物在血量降低时触发的视觉特效"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetShowBloodVFX));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetShowBloodVFX));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetShowBloodVFX());
			
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			GameplayVFXCollection->AddSetting(Setting);
		}

		{
			const FText EDrawStyleText[] = {
				LOCTEXT("EStyleType_Normal", "正常"),
				LOCTEXT("EStyleType_Cartoon", "卡通"),
				LOCTEXT("EStyleType_Hero", "英雄"),
				LOCTEXT("EStyleType_BlackWhite", "黑白")
			};

			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("GameplayVFX_DrawStyle"));
			Setting->SetDisplayName(LOCTEXT("GameplayVFX_DrawStyle_Name", "画风"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayVFX_DrawStyle_Desc", "切换不同的画风，可能会有焕然一新的感觉，是个值得的尝试！"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetDrawStyle));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetDrawStyle));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetDrawStyle());

			for (int32 PresetIndex = 0; PresetIndex < (int32)EStyleType::MAX; ++PresetIndex)
			{
				Setting->AddEnumOption(static_cast<EStyleType>(PresetIndex), EDrawStyleText[PresetIndex]);
			}
			
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			GameplayVFXCollection->AddSetting(Setting);
		}

		{
			const FText ECameraColorFilterText[] = {
				LOCTEXT("ECameraColorFilter_None", "无"),
				LOCTEXT("ECameraColorFilter_SkyBlue", "天青"),
				LOCTEXT("ECameraColorFilter_Retro", "怀旧"),
				LOCTEXT("ECameraColorFilter_Pink", "粉"),
				LOCTEXT("ECameraColorFilter_MoonlitNight", "清辉夜凝"),
				LOCTEXT("ECameraColorFilter_GrayWhite", "灰白"),
				LOCTEXT("ECameraColorFilter_Ocean", "海水"),
				LOCTEXT("ECameraColorFilter_Bright", "明亮"),
				LOCTEXT("ECameraColorFilter_Twilight", "黄昏"),
				LOCTEXT("ECameraColorFilter_Dawn", "清晨"),
				LOCTEXT("ECameraColorFilter_FairyTale", "童话"),
				LOCTEXT("ECameraColorFilter_Soft", "柔和")
			};

			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("GameplayVFX_CameraColorFilter"));
			Setting->SetDisplayName(LOCTEXT("GameplayVFX_CameraColorFilter_Name", "滤镜"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayVFX_CameraColorFilter_Desc", "给您的镜头打上一层滤镜，说不定别有风味！"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetCameraColorFilter));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetCameraColorFilter));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetCameraColorFilter());

			for (int32 PresetIndex = 0; PresetIndex < (int32)ECameraColorFilter::MAX; ++PresetIndex)
			{
				Setting->AddEnumOption(static_cast<ECameraColorFilter>(PresetIndex), ECameraColorFilterText[PresetIndex]);
			}
			
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			GameplayVFXCollection->AddSetting(Setting);
		}
	}

	{
		UGameSettingCollection* GameplayCameraCollection = NewObject<UGameSettingCollection>();
		GameplayCameraCollection->SetDevName(TEXT("GameplayCameraCollection"));
		GameplayCameraCollection->SetDisplayName(LOCTEXT("GameplayCameraCollection_Name", "镜头控制"));
		Screen->AddSetting(GameplayCameraCollection);

		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("GameplayCamera_FollowMode"));
			Setting->SetDisplayName(LOCTEXT("GameplayCamera_FollowMode_Name", "镜头跟随模式"));
			Setting->SetDescriptionRichText(LOCTEXT("GameplayCamera_FollowMode_Desc", "当角色进行移动，镜头的跟随方式"));
			
			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetCameraFollowMode));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetCameraFollowMode));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetCameraFollowMode());
			Setting->AddEnumOption(ECameraFollowMode::Preview, LOCTEXT("GameplayCamera_FollowModePreView", "前瞻"));
			Setting->AddEnumOption(ECameraFollowMode::Behind, LOCTEXT("GameplayCamera_FollowModeBehind", "尾随"));
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			GameplayCameraCollection->AddSetting(Setting);
		}
	}
	return Screen;
}

#undef LOCTEXT_NAMESPACE
