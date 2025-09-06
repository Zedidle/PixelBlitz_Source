// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "CommonFuncLib.generated.h"


struct FLocalizedTableData;

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

UENUM(BlueprintType)
enum class EFloatingTextAnimType : uint8
{
	Shaking UMETA(DisplayName = "抖动"),
	Floatup UMETA(DisplayName = "浮动"),
};

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

	UFUNCTION(BlueprintCallable, Category="Common | FloatingText")
	static void SpawnFloatingTextLocalized(FLocalizedTableData LocalizedTableData, FVector WorldLocation,
		FLinearColor TextColor = FLinearColor::White, FVector2D RenderScale = FVector2D(1,1), UPaperSprite* Icon = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Time")
	static float DealDeltaTime(float Seconds);

	UFUNCTION(BlueprintCallable, Category="Common | UserWidget")
	static void SpawnCenterTip(FText Tip, FLinearColor Color, FVector2D Translation = FVector2D(0,0),
		FVector2D Scale = FVector2D(1.0f, 1.0f), float PlaybackSpeed = 1.0f, EFloatingTextAnimType AnimType = EFloatingTextAnimType::Shaking);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Input")
	static FKey GetActionKey(UInputAction* IA);


	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Input")
	static bool IsGamePadControlling();

	UFUNCTION(BlueprintCallable, Category="Common | Input")
	static void SetGamePadControlling(bool On);

	template<typename T>
	static void CalRandomMap(TMap<T, int> M, T& Result);

};

template <typename T>
void UCommonFuncLib::CalRandomMap(TMap<T, int> M, T& Result)
{
	int Total = 0;
	TArray<FName> Keys;
	M.GetKeys(Keys);
	for (auto& Key : Keys)
	{
		if(M.Contains(Key))
		{
			Total += M[Key];
		}
	}

	int R = FMath::RandHelper(Total);
	int v = 0;
	for (auto& Key : Keys)
	{
		if(M.Contains(Key))
		{
			v += M[Key];
			if (v>=R)
			{
				Result = Key;
			}
		}
	}
}
