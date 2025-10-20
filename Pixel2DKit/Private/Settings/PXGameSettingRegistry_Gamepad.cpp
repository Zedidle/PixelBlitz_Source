// Copyright Epic Games, Inc. All Rights Reserved.

#include "CommonInputBaseTypes.h"
#include "DataSource/GameSettingDataSource.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"
#include "Settings/PXGameSettingRegistry.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/PXSettingsShared.h"
#include "NativeGameplayTags.h"
#include "Player/PXLocalPlayer.h"

#define LOCTEXT_NAMESPACE "PX"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_SupportsGamepad, "Platform.Trait.Input.SupportsGamepad");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_SupportsTriggerHaptics, "Platform.Trait.Input.SupportsTriggerHaptics");

UGameSettingCollection* UPXGameSettingRegistry::InitializeGamepadSettings(UPXLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("GamepadCollection"));
	Screen->SetDisplayName(LOCTEXT("GamepadCollection_Name", "手柄"));
	Screen->Initialize(InLocalPlayer);

	// Hardware
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* Hardware = NewObject<UGameSettingCollection>();
		Hardware->SetDevName(TEXT("HardwareCollection"));
		Hardware->SetDisplayName(LOCTEXT("HardwareCollection_Name", "设备"));
		Screen->AddSetting(Hardware);
			
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic* Setting = NewObject<UGameSettingValueDiscreteDynamic>();
			Setting->SetDevName(TEXT("ControllerHardware"));
			Setting->SetDisplayName(LOCTEXT("ControllerHardware_Name", "控制器设备"));
			Setting->SetDescriptionRichText(LOCTEXT("ControllerHardware_Description", "你正在使用的控制器类型"));
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetControllerPlatform));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetControllerPlatform));
			
			if (UCommonInputPlatformSettings* PlatformInputSettings = UPlatformSettingsManager::Get().GetSettingsForPlatform<UCommonInputPlatformSettings>())
			{
				const TArray<TSoftClassPtr<UCommonInputBaseControllerData>>& ControllerDatas = PlatformInputSettings->GetControllerData();
				for (TSoftClassPtr<UCommonInputBaseControllerData> ControllerDataPtr : ControllerDatas)
				{
					if (TSubclassOf<UCommonInputBaseControllerData> ControllerDataClass = ControllerDataPtr.LoadSynchronous())
					{
						const UCommonInputBaseControllerData* ControllerData = ControllerDataClass.GetDefaultObject();
						if (ControllerData->InputType == ECommonInputType::Gamepad)
						{
							Setting->AddDynamicOption(ControllerData->GamepadName.ToString(), ControllerData->GamepadDisplayName);
						}
					}
				}

				// Add the setting if we can select more than one game controller type on this platform
				// and we are allowed to change it
				if (Setting->GetDynamicOptions().Num() > 1 && PlatformInputSettings->CanChangeGamepadType())
				{
					Hardware->AddSetting(Setting);

					const FName CurrentControllerPlatform = GetDefault<UPXSettingsLocal>()->GetControllerPlatform();
					if (CurrentControllerPlatform == NAME_None)
					{
						Setting->SetDiscreteOptionByIndex(0);
					}
					else
					{
						Setting->SetDefaultValueFromString(CurrentControllerPlatform.ToString());
					}
				}
			}
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("GamepadVibration"));
			Setting->SetDisplayName(LOCTEXT("GamepadVibration_Name", "震动"));
			Setting->SetDescriptionRichText(LOCTEXT("GamepadVibration_Description", "手柄震动开关"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetForceFeedbackEnabled));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetForceFeedbackEnabled));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetForceFeedbackEnabled());

			Hardware->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertVerticalAxis_Gamepad"));
			Setting->SetDisplayName(LOCTEXT("InvertVerticalAxis_Gamepad_Name", "反转垂直旋转"));
			Setting->SetDescriptionRichText(LOCTEXT("InvertVerticalAxis_Gamepad_Description", "是否反转垂直方向上的视角旋转"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertVerticalAxis_Gamepad));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertVerticalAxis_Gamepad));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetInvertVerticalAxis_Gamepad());

			Hardware->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Bool* Setting = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
			Setting->SetDevName(TEXT("InvertHorizontalAxis_Gamepad"));
			Setting->SetDisplayName(LOCTEXT("InvertHorizontalAxis_Gamepad_Name", "反转水平旋转"));
			Setting->SetDescriptionRichText(LOCTEXT("InvertHorizontalAxis_Gamepad_Description", "是否反转水平方向上的视角旋转."));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetInvertHorizontalAxis_Gamepad));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetInvertHorizontalAxis_Gamepad));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetInvertHorizontalAxis_Gamepad());

			Hardware->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}

	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* GamepadBinding = NewObject<UGameSettingCollection>();
		GamepadBinding->SetDevName(TEXT("GamepadBindingCollection"));
		GamepadBinding->SetDisplayName(LOCTEXT("GamepadBindingCollection_Name", "控制设置"));
		Screen->AddSetting(GamepadBinding);
	}

	// Basic - Look Sensitivity
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* BasicSensitivity = NewObject<UGameSettingCollection>();
		BasicSensitivity->SetDevName(TEXT("BasicSensitivityCollection"));
		BasicSensitivity->SetDisplayName(LOCTEXT("BasicSensitivityCollection_Name", "灵敏度"));
		Screen->AddSetting(BasicSensitivity);


		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("GamepadLookSensitivityPreset"));
			Setting->SetDisplayName(LOCTEXT("GamepadLookSensitivityPreset_Name", "视角灵敏度"));
			Setting->SetDescriptionRichText(LOCTEXT("GamepadLookSensitivityPreset_Description", "视角的旋转速度"));
			
			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetGamepadLookSensitivity));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetGamepadLookSensitivity));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetGamepadLookSensitivity());

			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			Setting->SetSourceRangeAndStep(TRange<double>(0.1, 5), 0.01);
			
			Setting->SetMinimumLimit(0.1);
			
			BasicSensitivity->AddSetting(Setting);
		}
	}

	// Dead Zone
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* DeadZone = NewObject<UGameSettingCollection>();
		DeadZone->SetDevName(TEXT("DeadZoneCollection"));
		DeadZone->SetDisplayName(LOCTEXT("DeadZoneCollection_Name", "控制器死区"));
		Screen->AddSetting(DeadZone);

		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("MoveStickDeadZone"));
			Setting->SetDisplayName(LOCTEXT("MoveStickDeadZone_Name", "左摇杆死区"));
			Setting->SetDescriptionRichText(LOCTEXT("MoveStickDeadZone_Description", "增加或减小摇杆周围不会识别输入的区域范围。若将该数值设置得过低，可能导致手指离开摇杆后角色仍持续移动。"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetGamepadMoveStickDeadZone));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetGamepadMoveStickDeadZone));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetGamepadMoveStickDeadZone());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
			Setting->SetMinimumLimit(0.05);
			Setting->SetMaximumLimit(0.95);

			DeadZone->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("LookStickDeadZone"));
			Setting->SetDisplayName(LOCTEXT("LookStickDeadZone_Name", "右摇杆死区"));
			Setting->SetDescriptionRichText(LOCTEXT("LookStickDeadZone_Description", "调整摇杆周围不被识别输入的死区范围。若将该值设置得过低，可能导致手指离开摇杆后相机视角仍继续移动。"));

			Setting->SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetGamepadLookStickDeadZone));
			Setting->SetDynamicSetter(GET_SHARED_SETTINGS_FUNCTION_PATH(SetGamepadLookStickDeadZone));
			Setting->SetDefaultValue(GetDefault<UPXSettingsShared>()->GetGamepadLookStickDeadZone());
			Setting->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
			Setting->SetMinimumLimit(0.05);
			Setting->SetMaximumLimit(0.95);

			DeadZone->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}

	return Screen;
}

#undef LOCTEXT_NAMESPACE
