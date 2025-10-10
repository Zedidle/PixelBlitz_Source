// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/CustomSettings/PXSettingValueDiscrete_PerfStat.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "GameSettingCollection.h"
#include "Settings/PXGameSettingRegistry.h"
#include "Performance/PXPerformanceStatTypes.h"

class UPXLocalPlayer;

#define LOCTEXT_NAMESPACE "PX"

//////////////////////////////////////////////////////////////////////

void UPXGameSettingRegistry::AddPerformanceStatPage(UGameSettingCollection* PerfStatsOuterCategory, UPXLocalPlayer* InLocalPlayer)
{
	//----------------------------------------------------------------------------------
	{
		static_assert((int32)EPXDisplayablePerformanceStat::Count == 15, "Consider updating this function to deal with new performance stats");

		UGameSettingCollectionPage* StatsPage = NewObject<UGameSettingCollectionPage>();
		StatsPage->SetDevName(TEXT("PerfStatsPage"));
		StatsPage->SetDisplayName(LOCTEXT("PerfStatsPage_Name", "性能统计"));
		StatsPage->SetDescriptionRichText(LOCTEXT("PerfStatsPage_Description", "显示配置性能统计信息"));
		StatsPage->SetNavigationText(LOCTEXT("PerfStatsPage_Navigation", "Edit"));

		StatsPage->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

		PerfStatsOuterCategory->AddSetting(StatsPage);

		// Performance stats
		////////////////////////////////////////////////////////////////////////////////////
		{
			UGameSettingCollection* StatCategory_Performance = NewObject<UGameSettingCollection>();
			StatCategory_Performance->SetDevName(TEXT("StatCategory_Performance"));
			StatCategory_Performance->SetDisplayName(LOCTEXT("StatCategory_Performance_Name", "性能"));
			StatsPage->AddSetting(StatCategory_Performance);

			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::ClientFPS);
				Setting->SetDisplayName(LOCTEXT("PerfStat_ClientFPS", "客户端帧率"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_ClientFPS", "本地设备每秒渲染的画面帧数，直接影响游戏流畅度（越高越好）。"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::ServerFPS);
				Setting->SetDisplayName(LOCTEXT("PerfStat_ServerFPS", "服务端帧率"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_ServerFPS", "服务器每秒更新和同步游戏状态的次数，影响多人在线游戏的实时性和同步效果"));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::FrameTime);
				Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime", "帧时"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime", "完成一整帧画面所有处理所花费的总时间."));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::IdleTime);
				Setting->SetDisplayName(LOCTEXT("PerfStat_IdleTime", "空闲时间"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_IdleTime", "用于帧调度的空闲等待时间."));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::FrameTime_GameThread);
				Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_GameThread", "CPU游戏线程时间"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_GameThread", "主游戏线程耗时."));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::FrameTime_RenderThread);
				Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_RenderThread", "CPU渲染线程时间"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_RenderThread", "渲染线程耗时."));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::FrameTime_RHIThread);
				Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_RHIThread", "CPU RHI 时间"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_RHIThread", "CPU RHI（渲染硬件接口）线程时间."));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::FrameTime_GPU);
				Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_GPU", "GPU 耗时"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_GPU", "GPU 渲染耗时."));
				StatCategory_Performance->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
		}

		// Network stats
		////////////////////////////////////////////////////////////////////////////////////
		{
			UGameSettingCollection* StatCategory_Network = NewObject<UGameSettingCollection>();
			StatCategory_Network->SetDevName(TEXT("StatCategory_Network"));
			StatCategory_Network->SetDisplayName(LOCTEXT("StatCategory_Network_Name", "网络"));
			StatsPage->AddSetting(StatCategory_Network);

			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::Ping);
				Setting->SetDisplayName(LOCTEXT("PerfStat_Ping", "延迟"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_Ping", "您连接到服务器的往返延迟。"));
				StatCategory_Network->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::PacketLoss_Incoming);
				Setting->SetDisplayName(LOCTEXT("PerfStat_PacketLoss_Incoming", "收包丢失率"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketLoss_Incoming", "接收数据包的丢失率"));
				StatCategory_Network->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::PacketLoss_Outgoing);
				Setting->SetDisplayName(LOCTEXT("PerfStat_PacketLoss_Outgoing", "发包丢失率"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketLoss_Outgoing", "发送数据包的丢失率"));
				StatCategory_Network->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::PacketRate_Incoming);
				Setting->SetDisplayName(LOCTEXT("PerfStat_PacketRate_Incoming", "入包率"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketRate_Incoming", "传入数据包速率（每秒）"));
				StatCategory_Network->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::PacketRate_Outgoing);
				Setting->SetDisplayName(LOCTEXT("PerfStat_PacketRate_Outgoing", "出包率"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketRate_Outgoing", "发送数据包速率（每秒）"));
				StatCategory_Network->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::PacketSize_Incoming);
				Setting->SetDisplayName(LOCTEXT("PerfStat_PacketSize_Incoming", "收包大小"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketSize_Incoming", "上一秒接收数据包的平均大小（字节）."));
				StatCategory_Network->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
			{
				UPXSettingValueDiscrete_PerfStat* Setting = NewObject<UPXSettingValueDiscrete_PerfStat>();
				Setting->SetStat(EPXDisplayablePerformanceStat::PacketSize_Outgoing);
				Setting->SetDisplayName(LOCTEXT("PerfStat_PacketSize_Outgoing", "发包大小"));
				Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketSize_Outgoing", "上一秒发送数据包的平均大小（字节）."));
				StatCategory_Network->AddSetting(Setting);
			}
			//----------------------------------------------------------------------------------
		}
	}
}

#undef LOCTEXT_NAMESPACE
