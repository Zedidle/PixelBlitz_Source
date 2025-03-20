// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamLeaderBoardSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "OnlineStats.h"
#include "OnlineSubsystemSteamModule.h"
#include "GameFramework/GameSession.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineLeaderboardInterface.h"


 class IOnlineLeaderboards;
 class FUniqueNetId;
 class IOnlineIdentity;

USteamLeaderBoardSubsystem::USteamLeaderBoardSubsystem()
{
}

void Log(FString S)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, S);
	}
}

void USteamLeaderBoardSubsystem::InitOnlineSession()
{
	if (_onlineSession)
	{
		// 在线子系统已经初始化
		return;
	}

	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem)
	{
		_onlineSession = onlineSubsystem->GetSessionInterface();
		if (GEngine)
		{
			Log(FString::Printf(TEXT("当前网络子系统为: %s"), *onlineSubsystem->GetSubsystemName().ToString()));
		}
	}
	else
	{
		Log(TEXT("获取在线子系统失败"));
	}
}

void USteamLeaderBoardSubsystem::TestLeaderboard()
{
	UE_LOG(LogTemp, Log, TEXT("USteamLeaderBoardSubsystem::UpdateLeaderboard"));
	// IModuleInterface* Module = FModuleManager::Get().GetModule(FName("OnlineSubsystemSteam")); // Module Valid
}


void USteamLeaderBoardSubsystem::TestReadLeaderboards()
{
	IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (!SteamSubsystem)
    {
    	Log(TEXT("Steam Subsystem not available."));
        return;
    }

	
    IOnlineLeaderboardsPtr Leaderboards = SteamSubsystem->GetLeaderboardsInterface();
    if (!Leaderboards.IsValid())
    {
    	Log(TEXT("Leaderboards Interface not available."));
        return;
    }

    // 创建排行榜读取对象
    FOnlineLeaderboardReadRef LeaderboardReadObject = MakeShared<FOnlineLeaderboardRead>();
    LeaderboardReadObject->LeaderboardName = TEXT("Fastest"); // 替换为你的排行榜名称
    LeaderboardReadObject->SortedColumn = TEXT("Score");         // 替换为你的统计列名

    // 玩家列表（可选）
    TArray<TSharedRef<const FUniqueNetId>> Players;

    // 提交排行榜读取请求
    if (Leaderboards->ReadLeaderboards(Players, LeaderboardReadObject))
    {
    	Log(TEXT("Leaderboard read request submitted."));

        Leaderboards->AddOnLeaderboardReadCompleteDelegate_Handle(
            FOnLeaderboardReadCompleteDelegate::CreateLambda(
                [LeaderboardReadObject](bool bWasSuccessful)
                {
                    if (bWasSuccessful)
                    {
						Log(TEXT("Leaderboard read completed successfully."));
                        // 遍历排行榜数据
                        for (const FOnlineStatsRow& Row : LeaderboardReadObject->Rows)
                        {
                            FString PlayerName = Row.NickName; // 玩家昵称
                            FString PlayerId = Row.PlayerId->ToString(); // 玩家 ID

                            // 提取统计数据
                            const FVariantData* StatData = Row.Columns.Find(TEXT("YourStatColumn"));
                            if (StatData)
                            {
                                int32 Score = 0;
                                StatData->GetValue(Score);
                            	Log(FString::Printf(TEXT("Player: %s (ID: %s), Score: %d"), *PlayerName, *PlayerId, Score));
                            }
                            else
                            {
                            	Log(FString::Printf(TEXT("Stat data not found for player: %s"), *PlayerName));
                            }
                        }
                    }
                    else
                    {
                    	Log(TEXT("Leaderboard read failed."));
                    }
                }
            )
        );
    }
    else
    {
    	Log(TEXT("Failed to submit leaderboard read request."));
    }

}

void USteamLeaderBoardSubsystem::TestReadLeaderboardsForFriends()
{
}

void USteamLeaderBoardSubsystem::TestReadLeaderboardsAroundRank()
{
}

void USteamLeaderBoardSubsystem::TestReadLeaderboardsAroundUser()
{
}

void USteamLeaderBoardSubsystem::TestFreeStats()
{
}

bool USteamLeaderBoardSubsystem::TestWriteLeaderboards(APlayerController* PlayerController, FName LeaderboardName, int32 value)
{
	if (PlayerController == nullptr)
	{
		Log(TEXT("PlayerController invalid!"));
		return false;
	}
	
	APlayerState* PlayerState = ToRawPtr(PlayerController->PlayerState);
	FUniqueNetIdPtr PlayerId = PlayerState->GetUniqueId().GetUniqueNetId();
	if (!PlayerId.IsValid())
	{
		Log(TEXT("Invalid player ID"));
		return false;
	}
	
	
	Log(FString::Printf(TEXT("PlayerState->SessionName: %s"), *PlayerState->SessionName.ToString()));


	const IOnlineSubsystem* OS_Steam = Online::GetSubsystem(GetWorld());
	if (!OS_Steam)
	{
		Log(TEXT("OSS not available"));
		return false;
	}
	else
	{
		Log(FString::Printf(TEXT("OSS is available, AppId: %s"), *OS_Steam->GetAppId()));
		Log(FString::Printf(TEXT("OSS is available, OnlineServiceName: %s"), *OS_Steam->GetOnlineServiceName().ToString()));
	}
	
	const IOnlineLeaderboardsPtr Leaderboards = OS_Steam->GetLeaderboardsInterface();

	if (!Leaderboards.IsValid())
	{
		Log(TEXT("Identity or Leaderboards interface not available"));
		return false;
	}
	
	FOnlineLeaderboardWrite WriteObject;
	WriteObject.LeaderboardNames.Add(LeaderboardName);
	WriteObject.RatedStat = LeaderboardName;
	WriteObject.DisplayFormat = ELeaderboardFormat::Number;
	WriteObject.SortMethod = ELeaderboardSort::Descending;
	WriteObject.UpdateMethod = ELeaderboardUpdateMethod::KeepBest;
	WriteObject.SetIntStat(LeaderboardName, value);
	
	bool bResult = Leaderboards->WriteLeaderboards(PlayerState->SessionName, *PlayerId, WriteObject);
	bool bFlushResult = Leaderboards->FlushLeaderboards(LeaderboardName);

	return bResult && bFlushResult;
}

void USteamLeaderBoardSubsystem::TestFlushLeaderboards()
{
}

void USteamLeaderBoardSubsystem::TestWriteOnlinePlayerRatings()
{
}

