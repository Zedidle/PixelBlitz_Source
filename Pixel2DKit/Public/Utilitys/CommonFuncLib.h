// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "CommonFuncLib.generated.h"


UENUM(BlueprintType)
enum class EFloatingTextType : uint8
{
	Damage,
	Health,
	Energy,
	Gold,
	Item,
	Experience
};

/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class UCommonFuncLib : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="Common | FloatingText")
	static void SpawnFloatingCombatText(EFloatingTextType TextType, FText Text,
		UPaperSprite* Icon, FVector WorldLocation, FVector2D Scale = FVector2D(1.0f, 1.0f), FLinearColor ColorMulti = FLinearColor::White);

	UFUNCTION(BlueprintCallable, Category="Common | FloatingText")
	static void SpawnFloatingText(FText Text, FVector WorldLocation, FLinearColor TextColor = FLinearColor::White, FVector2D RenderScale = FVector2D(1,1),
		UPaperSprite* Icon = nullptr);

	UFUNCTION(BlueprintCallable, Category="Common | FloatingText")
	static void SpawnFloatingTextDefault(const FString InTableName, const FName InRowName, FVector WorldLocation,
		FLinearColor TextColor = FLinearColor::White, FVector2D RenderScale = FVector2D(1,1), UPaperSprite* Icon = nullptr);
	
};
