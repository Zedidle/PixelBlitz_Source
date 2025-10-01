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
	Screen->SetDisplayName(LOCTEXT("GameplayCollection_Name", "游戏内容"));
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
		GameplayTextCollection->SetDisplayName(LOCTEXT("GameplayTextCollection_Name", "文本"));
		Screen->AddSetting(GameplayTextCollection);
		
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
