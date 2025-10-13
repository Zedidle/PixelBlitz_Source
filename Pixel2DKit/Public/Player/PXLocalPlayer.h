// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonLocalPlayer.h"

#include "PXLocalPlayer.generated.h"

#define UE_API PIXEL2DKIT_API

class APlayerController;
class UInputMappingContext;
class UPXSettingsLocal;
class UPXSettingsShared;
class UObject;
class UWorld;
struct FFrame;
struct FSwapAudioOutputResult;

UCLASS(MinimalAPI)
class UPXLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:

	UE_API UPXLocalPlayer();

	//~UObject interface
	UE_API virtual void PostInitProperties() override;
	//~End of UObject interface

	//~UPlayer interface
	UE_API virtual void SwitchController(class APlayerController* PC) override;
	//~End of UPlayer interface

	//~ULocalPlayer interface
	UE_API virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	UE_API virtual void InitOnlineSession() override;
	//~End of ULocalPlayer interface


	/** Gets the local settings for this player, this is read from config files at process startup and is always valid */
	UFUNCTION()
	UE_API UPXSettingsLocal* GetLocalSettings() const;

	/** Gets the shared setting for this player, this is read using the save game system so may not be correct until after user login */
	UFUNCTION()
	UE_API UPXSettingsShared* GetSharedSettings() const;

	/** Starts an async request to load the shared settings, this will call OnSharedSettingsLoaded after loading or creating new ones */
	UE_API void LoadSharedSettingsFromDisk(bool bForceLoad = false);

protected:
	UE_API void OnSharedSettingsLoaded(UPXSettingsShared* LoadedOrCreatedSettings);

	UE_API void OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId);
	
	UFUNCTION()
	UE_API void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);


private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UPXSettingsShared> SharedSettings;

	FUniqueNetIdRepl NetIdForSharedSettings;

	UPROPERTY(Transient)
	mutable TObjectPtr<const UInputMappingContext> InputMappingContext;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};

#undef UE_API
