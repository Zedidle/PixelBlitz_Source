// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "SteamLeaderBoardSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API USteamLeaderBoardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	// class IOnlineSubsystem* OS_Steam;

	IOnlineSessionPtr  _onlineSession;


public:
	USteamLeaderBoardSubsystem();

	
	UFUNCTION(BlueprintCallable)
	void InitOnlineSession();

	
	UFUNCTION(BlueprintCallable)
	void TestLeaderboard();

	UFUNCTION(BlueprintCallable)
	void TestReadLeaderboards();

	UFUNCTION(BlueprintCallable)
	void TestReadLeaderboardsForFriends();

	UFUNCTION(BlueprintCallable)
	void TestReadLeaderboardsAroundRank();

	UFUNCTION(BlueprintCallable)
	void TestReadLeaderboardsAroundUser();

	UFUNCTION(BlueprintCallable)
	void TestFreeStats();

	UFUNCTION(BlueprintCallable)
	bool TestWriteLeaderboards(APlayerController* PlayerController, FName LeaderboardName, int32 value);
	
	UFUNCTION(BlueprintCallable)
	void TestFlushLeaderboards();

	UFUNCTION(BlueprintCallable)
	void TestWriteOnlinePlayerRatings();
	
	
};
