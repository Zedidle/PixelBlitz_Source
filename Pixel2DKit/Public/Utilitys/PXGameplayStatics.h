// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/PXGameInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PXGameplayStatics.generated.h"


class UPXSettingsShared;
class APXGameState;
class APXPlayerController;

UCLASS()
class PIXEL2DKIT_API UPXGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayStatics", meta=(WorldContext="WorldContextObject"))
	static UPXGameInstance* GetGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayStatics", meta=(WorldContext="WorldContextObject"))
	static APXGameMode* GetGameMode(const UObject* WorldContextObject);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayStatics", meta=(WorldContext="WorldContextObject"))
	static APXGameState* GetGameState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayStatics", meta=(WorldContext="WorldContextObject"))
	static APXPlayerController* GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayStatics", meta=(WorldContext="WorldContextObject"))
	static ABasePXCharacter* GetPlayerCharacter(const UObject* WorldContextObject, int32 PlayerIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayStatics", meta=(WorldContext="WorldContextObject"))
	static UPXLocalPlayer* GetLocalPlayer(const UObject* WorldContextObject, int32 PlayerIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayStatics", meta=(WorldContext="WorldContextObject"))
	static UPXSettingsShared* GetSettingsShared(const UObject* WorldContextObject);
};
