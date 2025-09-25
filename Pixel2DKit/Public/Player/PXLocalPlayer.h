// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "PXLocalPlayer.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

	public:

	UPXLocalPlayer();

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface

	//~UPlayer interface
	virtual void SwitchController(class APlayerController* PC) override;
	//~End of UPlayer interface

	//~ULocalPlayer interface
	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	virtual void InitOnlineSession() override;
	//~End of ULocalPlayer interface

	/** Gets the local settings for this player, this is read from config files at process startup and is always valid */
	UFUNCTION()
	UPXSettingsLocal* GetLocalSettings() const;

	/** Gets the shared setting for this player, this is read using the save game system so may not be correct until after user login */
	UFUNCTION()
	UPXSettingsShared* GetSharedSettings() const;

	/** Starts an async request to load the shared settings, this will call OnSharedSettingsLoaded after loading or creating new ones */
	void LoadSharedSettingsFromDisk(bool bForceLoad = false);

protected:
	void OnSharedSettingsLoaded(UPXSettingsShared* LoadedOrCreatedSettings);

	void OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId);
	
	UFUNCTION()
	void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UPXSettingsShared> SharedSettings;

	FUniqueNetIdRepl NetIdForSharedSettings;

	UPROPERTY(Transient)
	mutable TObjectPtr<const UInputMappingContext> InputMappingContext;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};
