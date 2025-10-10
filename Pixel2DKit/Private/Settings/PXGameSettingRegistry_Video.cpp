// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/CustomSettings/PXSettingAction_SafeZoneEditor.h"
#include "Settings/CustomSettings/PXSettingValueDiscrete_MobileFPSType.h"
#include "Settings/CustomSettings/PXSettingValueDiscrete_OverallQuality.h"
#include "Settings/CustomSettings/PXSettingValueDiscrete_Resolution.h"
#include "EditCondition/WhenCondition.h"
#include "EditCondition/WhenPlatformHasTrait.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "Settings/PXGameSettingRegistry.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/PXSettingsShared.h"
#include "NativeGameplayTags.h"
#include "Performance/PXPerformanceSettings.h"
#include "Player/PXLocalPlayer.h"

#define LOCTEXT_NAMESPACE "PX"

UE_DEFINE_GAMEPLAY_TAG_STATIC(GameSettings_Action_EditSafeZone, "GameSettings.Action.EditSafeZone");
UE_DEFINE_GAMEPLAY_TAG_STATIC(GameSettings_Action_EditBrightness, "GameSettings.Action.EditBrightness");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsWindowedMode, "Platform.Trait.SupportsWindowedMode");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_NeedsBrightnessAdjustment, "Platform.Trait.NeedsBrightnessAdjustment");

//////////////////////////////////////////////////////////////////////

enum class EFramePacingEditCondition
{
	EnableIf,
	DisableIf
};

// Checks the platform-specific value for FramePacingMode
class FGameSettingEditCondition_FramePacingMode : public FGameSettingEditCondition
{
public:
	FGameSettingEditCondition_FramePacingMode(EPXFramePacingMode InDesiredMode, EFramePacingEditCondition InMatchMode = EFramePacingEditCondition::EnableIf)
		: DesiredMode(InDesiredMode)
		, MatchMode(InMatchMode)
	{
	}

	virtual void GatherEditState(const ULocalPlayer * InLocalPlayer, FGameSettingEditableState & InOutEditState) const override
	{
		const EPXFramePacingMode ActualMode = UPXPlatformSpecificRenderingSettings::Get()->FramePacingMode;
		
		const bool bMatches = (ActualMode == DesiredMode);
		const bool bMatchesAreBad = (MatchMode == EFramePacingEditCondition::DisableIf);

		if (bMatches == bMatchesAreBad)
		{
			InOutEditState.Kill(FString::Printf(TEXT("Frame pacing mode %d didn't match requirement %d"), (int32)ActualMode, (int32)DesiredMode));
		}
	}
private:
	EPXFramePacingMode DesiredMode;
	EFramePacingEditCondition MatchMode;
};

//////////////////////////////////////////////////////////////////////

// Checks the platform-specific value for bSupportsGranularVideoQualitySettings
class FGameSettingEditCondition_VideoQuality : public FGameSettingEditCondition
{
public:
	FGameSettingEditCondition_VideoQuality(const FString& InDisableString)
		: DisableString(InDisableString)
	{
	}

	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override
	{
		if (!UPXPlatformSpecificRenderingSettings::Get()->bSupportsGranularVideoQualitySettings)
		{
			InOutEditState.Kill(DisableString);
		}
	}

	virtual void SettingChanged(const ULocalPlayer* LocalPlayer, UGameSetting* Setting, EGameSettingChangeReason Reason) const override
	{
		// TODO for now this applies the setting immediately
		const UPXLocalPlayer* PXLocalPlayer = CastChecked<UPXLocalPlayer>(LocalPlayer);
		PXLocalPlayer->GetLocalSettings()->ApplyScalabilitySettings();
	}

private:
	FString DisableString;
};

////////////////////////////////////////////////////////////////////////////////////

UGameSettingCollection* UPXGameSettingRegistry::InitializeVideoSettings(UPXLocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("VideoCollection"));
	Screen->SetDisplayName(LOCTEXT("VideoCollection_Name", "视频"));
	Screen->Initialize(InLocalPlayer);

	UGameSettingValueDiscreteDynamic_Enum* WindowModeSetting = nullptr;
	UGameSetting* MobileFPSType = nullptr;

	// Display
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* Display = NewObject<UGameSettingCollection>();
		Display->SetDevName(TEXT("DisplayCollection"));
		Display->SetDisplayName(LOCTEXT("DisplayCollection_Name", "显示"));
		Screen->AddSetting(Display);

		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Enum* Setting = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			Setting->SetDevName(TEXT("WindowMode"));
			Setting->SetDisplayName(LOCTEXT("WindowMode_Name", "窗口模式"));
			Setting->SetDescriptionRichText(LOCTEXT("WindowMode_Desc", "在窗口化模式下，您可以更轻松地与其他窗口交互，并可拖拽窗口边缘调整尺寸。在窗口化全屏模式下，您可以轻松切换不同应用程序。在独占全屏模式下，虽然与其他窗口的交互相对不便，但游戏运行速度会稍快一些。"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetFullscreenMode));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetFullscreenMode));
			Setting->AddEnumOption(EWindowMode::Fullscreen, LOCTEXT("WindowModeFullscreen", "独占全屏"));
			Setting->AddEnumOption(EWindowMode::WindowedFullscreen, LOCTEXT("WindowModeWindowedFullscreen", "窗口化全屏"));
			Setting->AddEnumOption(EWindowMode::Windowed, LOCTEXT("WindowModeWindowed", "窗口"));

			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_SupportsWindowedMode, TEXT("Platform does not support window mode")));

			WindowModeSetting = Setting;

			Display->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UPXSettingValueDiscrete_Resolution* Setting = NewObject<UPXSettingValueDiscrete_Resolution>();
			Setting->SetDevName(TEXT("Resolution"));
			Setting->SetDisplayName(LOCTEXT("Resolution_Name", "分辨率"));
			Setting->SetDescriptionRichText(LOCTEXT("Resolution_Desc", "显示分辨率在窗口化模式下决定窗口的尺寸。在全屏模式下，显示分辨率决定显卡的输出分辨率，根据显示器与显卡的匹配情况，可能会产生黑边。在窗口化全屏模式下，显示分辨率的设置不生效。"));

			Setting->AddEditDependency(WindowModeSetting);
			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_SupportsWindowedMode, TEXT("Platform does not support window mode")));
			Setting->AddEditCondition(MakeShared<FWhenCondition>([WindowModeSetting](const ULocalPlayer*, FGameSettingEditableState& InOutEditState)
			{
				if (WindowModeSetting->GetValue<EWindowMode::Type>() == EWindowMode::WindowedFullscreen)
				{
					InOutEditState.Disable(LOCTEXT("ResolutionWindowedFullscreen_Disabled", "当显示模式设置为<strong>窗口化全屏</strong>时，分辨率必须与桌面的原生分辨率保持一致。"));
				}
			}));

			Display->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			// AddPerformanceStatPage(Display, InLocalPlayer);
		}
		//----------------------------------------------------------------------------------
	}

	// Graphics
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* Graphics = NewObject<UGameSettingCollection>();
		Graphics->SetDevName(TEXT("GraphicsCollection"));
		Graphics->SetDisplayName(LOCTEXT("GraphicsCollection_Name", "图形"));
		Screen->AddSetting(Graphics);

		//----------------------------------------------------------------------------------
		{
			UGameSettingValueScalarDynamic* Setting = NewObject<UGameSettingValueScalarDynamic>();
			Setting->SetDevName(TEXT("Brightness"));
			Setting->SetDisplayName(LOCTEXT("Brightness_Name", "亮度"));
			Setting->SetDescriptionRichText(LOCTEXT("Brightness_Desc", "调整屏幕亮度"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetDisplayGamma));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetDisplayGamma));
			Setting->SetDefaultValue(2.2);
			Setting->SetDisplayFormat([](double SourceValue, double NormalizedValue) {
				return FText::Format(LOCTEXT("BrightnessFormat", "{0}%"), (int32)FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(50, 150), NormalizedValue));
			});
			Setting->SetSourceRangeAndStep(TRange<double>(1.7, 2.7), 0.01);

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
			Setting->AddEditCondition(FWhenPlatformHasTrait::KillIfMissing(TAG_Platform_Trait_NeedsBrightnessAdjustment, TEXT("Platform does not require brightness adjustment.")));

			Graphics->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UPXSettingAction_SafeZoneEditor* Setting = NewObject<UPXSettingAction_SafeZoneEditor>();
			Setting->SetDevName(TEXT("SafeZone"));
			Setting->SetDisplayName(LOCTEXT("SafeZone_Name", "安全区域"));
			Setting->SetDescriptionRichText(LOCTEXT("SafeZone_Desc", "设置UI的安全区域"));
			Setting->SetActionText(LOCTEXT("SafeZone_Action", "设置安全区"));
			Setting->SetNamedAction(GameSettings_Action_EditSafeZone);

			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());
			Setting->AddEditCondition(MakeShared<FWhenCondition>([](const ULocalPlayer*, FGameSettingEditableState& InOutEditState) {
				FDisplayMetrics Metrics;
				FSlateApplication::Get().GetCachedDisplayMetrics(Metrics);
				if (Metrics.TitleSafePaddingSize.Size() == 0)
				{
					InOutEditState.Kill(TEXT("Platform does not have any TitleSafePaddingSize configured in the display metrics."));
				}
			}));

			Graphics->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}


	// Graphics Quality
	////////////////////////////////////////////////////////////////////////////////////
	{
		UGameSettingCollection* GraphicsQuality = NewObject<UGameSettingCollection>();
		GraphicsQuality->SetDevName(TEXT("GraphicsQuality"));
		GraphicsQuality->SetDisplayName(LOCTEXT("GraphicsQuality_Name", "图形质量"));
		Screen->AddSetting(GraphicsQuality);

		UGameSetting* AutoSetQuality = nullptr;
		UGameSetting* GraphicsQualityPresets = nullptr;

		//----------------------------------------------------------------------------------
		{
			// Console-style device profile selection
			UGameSettingValueDiscreteDynamic* Setting = NewObject<UGameSettingValueDiscreteDynamic>();
			Setting->SetDevName(TEXT("DeviceProfileSuffix"));
			Setting->SetDisplayName(LOCTEXT("DeviceProfileSuffix_Name", "质量预设"));
			Setting->SetDescriptionRichText(LOCTEXT("DeviceProfileSuffix_Desc", "在不同的质量预设之间进行选择，以在质量和速度之间进行权衡。"));
			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetDesiredDeviceProfileQualitySuffix));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetDesiredDeviceProfileQualitySuffix));

			const UPXPlatformSpecificRenderingSettings* PlatformSettings = UPXPlatformSpecificRenderingSettings::Get();

			Setting->SetDefaultValueFromString(PlatformSettings->DefaultDeviceProfileSuffix);
			for (const FPXQualityDeviceProfileVariant& Variant : PlatformSettings->UserFacingDeviceProfileOptions)
			{
				if (FPlatformMisc::GetMaxRefreshRate() >= Variant.MinRefreshRate)
				{
					Setting->AddDynamicOption(Variant.DeviceProfileSuffix, Variant.DisplayName);
				}
			}

			if (Setting->GetDynamicOptions().Num() > 1)
			{
				GraphicsQuality->AddSetting(Setting);
			}
		}

		//----------------------------------------------------------------------------------
		{
			// Mobile style frame rate selection
			UPXSettingValueDiscrete_MobileFPSType* Setting = NewObject<UPXSettingValueDiscrete_MobileFPSType>();
			MobileFPSType = Setting;

			Setting->SetDevName(TEXT("FrameRateLimit_Mobile"));
			Setting->SetDisplayName(LOCTEXT("FrameRateLimit_Mobile_Name", "帧率限制"));
			Setting->SetDescriptionRichText(LOCTEXT("FrameRateLimit_Mobile_Desc", "选择期望的帧率，用于微调设备性能。"));

			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(EPXFramePacingMode::MobileStyle));

			GraphicsQuality->AddSetting(Setting);
		}

		//----------------------------------------------------------------------------------
		{
			UGameSettingAction* Setting = NewObject<UGameSettingAction>();
			Setting->SetDevName(TEXT("AutoSetQuality"));
			Setting->SetDisplayName(LOCTEXT("AutoSetQuality_Name", "自动设置"));
			Setting->SetDescriptionRichText(LOCTEXT("AutoSetQuality_Desc", "根据硬件基准测试自动配置图形质量选项。"));

			Setting->SetDoesActionDirtySettings(true);
			Setting->SetActionText(LOCTEXT("AutoSetQuality_Action", "自动设置"));
			Setting->SetCustomAction([](ULocalPlayer* LocalPlayer)
			{
				const UPXPlatformSpecificRenderingSettings* PlatformSettings = UPXPlatformSpecificRenderingSettings::Get();
				if (PlatformSettings->FramePacingMode == EPXFramePacingMode::MobileStyle)
				{
					UPXSettingsLocal::Get()->ResetToMobileDeviceDefaults();
				}
				else
				{
					const UPXLocalPlayer* PXLocalPlayer = CastChecked<UPXLocalPlayer>(LocalPlayer);
					// We don't save state until users apply the settings.
					constexpr bool bImmediatelySaveState = false;
					PXLocalPlayer->GetLocalSettings()->RunAutoBenchmark(bImmediatelySaveState);
				}
			});

			Setting->AddEditCondition(MakeShared<FWhenCondition>([](const ULocalPlayer* LocalPlayer, FGameSettingEditableState& InOutEditState)
			{
				const UPXPlatformSpecificRenderingSettings* PlatformSettings = UPXPlatformSpecificRenderingSettings::Get();
				const bool bCanUseDueToMobile = (PlatformSettings->FramePacingMode == EPXFramePacingMode::MobileStyle);

				const UPXLocalPlayer* PXLocalPlayer = CastChecked<UPXLocalPlayer>(LocalPlayer);
				const bool bCanBenchmark = PXLocalPlayer->GetLocalSettings()->CanRunAutoBenchmark();

				if (!bCanUseDueToMobile && !bCanBenchmark)
				{
					InOutEditState.Kill(TEXT("Auto quality not supported"));
				}
			}));

			if (MobileFPSType != nullptr)
			{
				MobileFPSType->AddEditDependency(Setting);
			}

			GraphicsQuality->AddSetting(Setting);

			AutoSetQuality = Setting;
		}
		//----------------------------------------------------------------------------------
		{
			UPXSettingValueDiscrete_OverallQuality* Setting = NewObject<UPXSettingValueDiscrete_OverallQuality>();
			Setting->SetDevName(TEXT("GraphicsQualityPresets"));
			Setting->SetDisplayName(LOCTEXT("GraphicsQualityPresets_Name", "质量预设"));
			Setting->SetDescriptionRichText(LOCTEXT("GraphicsQualityPresets_Desc", "质量预设允许您一次性调整多个视频选项。尝试几种选项，找到最符合您偏好和设备性能的设置。"));

			Setting->AddEditDependency(AutoSetQuality);

			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(EPXFramePacingMode::ConsoleStyle, EFramePacingEditCondition::DisableIf));

			if (MobileFPSType != nullptr)
			{
				Setting->AddEditDependency(MobileFPSType);
				MobileFPSType->AddEditDependency(Setting);
			}

			GraphicsQuality->AddSetting(Setting);

			GraphicsQualityPresets = Setting;
		}

		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("GlobalIlluminationQuality"));
			Setting->SetDisplayName(LOCTEXT("GlobalIlluminationQuality_Name", "全局光照"));
			Setting->SetDescriptionRichText(LOCTEXT("GlobalIlluminationQuality_Desc", "全局光照控制着动态计算的间接光照反弹、天空阴影以及环境光遮蔽的质量。“高”及以上级别的设置会采用更精确的光线追踪方法来计算光照，但这可能会影响性能。"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetGlobalIlluminationQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetGlobalIlluminationQuality));
			Setting->AddOption(0, LOCTEXT("VisualEffectQualityLow", "低"));
			Setting->AddOption(1, LOCTEXT("VisualEffectQualityMedium", "中"));
			Setting->AddOption(2, LOCTEXT("VisualEffectQualityHigh", "高"));
			Setting->AddOption(3, LOCTEXT("VisualEffectQualityEpic", "优异"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support GlobalIlluminationQuality")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("Shadows"));
			Setting->SetDisplayName(LOCTEXT("Shadows_Name", "阴影"));
			Setting->SetDescriptionRichText(LOCTEXT("Shadows_Desc", "阴影质量决定动态阴影的分辨率与可视距离。阴影能提升视觉品质并增强场景深度感知，但可能会影响性能。"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetShadowQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetShadowQuality));
			Setting->AddOption(0, LOCTEXT("ShadowLow", "关闭"));
			Setting->AddOption(1, LOCTEXT("ShadowMedium", "中"));
			Setting->AddOption(2, LOCTEXT("ShadowHigh", "高"));
			Setting->AddOption(3, LOCTEXT("ShadowEpic", "优异"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support Shadows")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("AntiAliasing"));
			Setting->SetDisplayName(LOCTEXT("AntiAliasing_Name", "抗锯齿"));
			Setting->SetDescriptionRichText(LOCTEXT("AntiAliasing_Desc", "抗锯齿可减少几何边缘的锯齿瑕疵。提高此项设置会使边缘看起来更平滑，但可能会影响性能。设置越高意味着抗锯齿处理强度越大。"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetAntiAliasingQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetAntiAliasingQuality));
			Setting->AddOption(0, LOCTEXT("AntiAliasingLow", "关闭"));
			Setting->AddOption(1, LOCTEXT("AntiAliasingMedium", "中"));
			Setting->AddOption(2, LOCTEXT("AntiAliasingHigh", "高"));
			Setting->AddOption(3, LOCTEXT("AntiAliasingEpic", "最高"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support Anti-Aliasing")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("ViewDistance"));
			Setting->SetDisplayName(LOCTEXT("ViewDistance_Name", "视距"));
			Setting->SetDescriptionRichText(LOCTEXT("ViewDistance_Desc", "视距设置决定物体在多大距离处被剔除以提升性能。"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetViewDistanceQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetViewDistanceQuality));
			Setting->AddOption(0, LOCTEXT("ViewDistanceNear", "近"));
			Setting->AddOption(1, LOCTEXT("ViewDistanceMedium", "中"));
			Setting->AddOption(2, LOCTEXT("ViewDistanceFar", "远"));
			Setting->AddOption(3, LOCTEXT("ViewDistanceEpic", "很远"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support View Distance")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
		{
			UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			Setting->SetDevName(TEXT("TextureQuality"));
			Setting->SetDisplayName(LOCTEXT("TextureQuality_Name", "纹理"));

			Setting->SetDescriptionRichText(LOCTEXT("TextureQuality_Desc", "纹理质量决定游戏中纹理的分辨率。提高此项设置会使物体表面细节更丰富，但可能会影响性能。"));

			Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetTextureQuality));
			Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetTextureQuality));
			Setting->AddOption(0, LOCTEXT("TextureQualityLow", "低"));
			Setting->AddOption(1, LOCTEXT("TextureQualityMedium", "中"));
			Setting->AddOption(2, LOCTEXT("TextureQualityHigh", "高"));
			Setting->AddOption(3, LOCTEXT("TextureQualityEpic", "最高"));

			Setting->AddEditDependency(AutoSetQuality);
			Setting->AddEditDependency(GraphicsQualityPresets);
			Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_VideoQuality>(TEXT("Platform does not support Texture quality")));

			// When this setting changes, it can GraphicsQualityPresets to be set to custom, or a particular preset.
			GraphicsQualityPresets->AddEditDependency(Setting);

			GraphicsQuality->AddSetting(Setting);
		}

	}

	return Screen;
}

void AddFrameRateOptions(UGameSettingValueDiscreteDynamic_Number* Setting)
{
	const FText FPSFormat = LOCTEXT("FPSFormat", "{0} FPS");
	for (int32 Rate : GetDefault<UPXPerformanceSettings>()->DesktopFrameRateLimits)
	{
		Setting->AddOption((float)Rate, FText::Format(FPSFormat, Rate));
	}
	Setting->AddOption(0.0f, LOCTEXT("UnlimitedFPS", "不限"));
}

void UPXGameSettingRegistry::InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, UPXLocalPlayer* InLocalPlayer)
{


	//----------------------------------------------------------------------------------
	{
		UGameSettingValueDiscreteDynamic_Number* Setting = NewObject<UGameSettingValueDiscreteDynamic_Number>();
		Setting->SetDevName(TEXT("FrameRateLimit_Always"));
		Setting->SetDisplayName(LOCTEXT("FrameRateLimit_Always_Name", "帧率限制"));
		Setting->SetDescriptionRichText(LOCTEXT("FrameRateLimit_Always_Desc", "帧率上限设置了允许的最高帧率。调低此值可使帧率更稳定，调高则能让性能更强的设备获得最佳体验。"));

		Setting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetFrameRateLimit_Always));
		Setting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetFrameRateLimit_Always));
		Setting->SetDefaultValue(GetDefault<UPXSettingsLocal>()->GetFrameRateLimit_Always());
		Setting->AddEditCondition(MakeShared<FGameSettingEditCondition_FramePacingMode>(EPXFramePacingMode::DesktopStyle));

		AddFrameRateOptions(Setting);

		Screen->AddSetting(Setting);
	}
}

#undef LOCTEXT_NAMESPACE
