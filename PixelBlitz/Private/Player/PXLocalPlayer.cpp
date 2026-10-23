#include "Player/PXLocalPlayer.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/PXSettingsShared.h"
#include "CommonUserSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXLocalPlayer)

class UObject;

UPXLocalPlayer::UPXLocalPlayer()
{
}

void UPXLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (UPXSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &UPXLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

void UPXLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);

}

bool UPXLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	return bResult;
}

void UPXLocalPlayer::InitOnlineSession()
{
	Super::InitOnlineSession();
}


UPXSettingsLocal* UPXLocalPlayer::GetLocalSettings() const
{
	return UPXSettingsLocal::Get();
}

UPXSettingsShared* UPXLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		// On PC it's okay to use the sync load because it only checks the disk
		// This could use a platform tag to check for proper save support instead
		bool bCanLoadBeforeLogin = PLATFORM_DESKTOP;
		
		if (bCanLoadBeforeLogin)
		{
			SharedSettings = UPXSettingsShared::LoadOrCreateSettings(this);
		}
		else
		{
			// We need to wait for user login to get the real settings so return temp ones
			SharedSettings = UPXSettingsShared::CreateTemporarySettings(this);
		}
	}

	return SharedSettings;
}

void UPXLocalPlayer::LoadSharedSettingsFromDisk(bool bForceLoad)
{
	FUniqueNetIdRepl CurrentNetId = GetCachedUniqueNetId();
	if (!bForceLoad && SharedSettings && CurrentNetId == NetIdForSharedSettings)
	{
		// Already loaded once, don't reload
		return;
	}

	ensure(UPXSettingsShared::AsyncLoadOrCreateSettings(this, UPXSettingsShared::FOnSettingsLoadedEvent::CreateUObject(this, &UPXLocalPlayer::OnSharedSettingsLoaded)));
}

void UPXLocalPlayer::OnSharedSettingsLoaded(UPXSettingsShared* LoadedOrCreatedSettings)
{
	// The settings are applied before it gets here
	if (ensure(LoadedOrCreatedSettings))
	{
		// This will replace the temporary or previously loaded object which will GC out normally
		SharedSettings = LoadedOrCreatedSettings;

		NetIdForSharedSettings = GetCachedUniqueNetId();
	}
}

void UPXLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void UPXLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}

