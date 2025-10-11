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


	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Time")
	static float DealDeltaTime(float Seconds);

	UFUNCTION(BlueprintCallable, Category="Common | UserWidget")
	static void SpawnCenterTip(FText _Tip, FLinearColor _Color = FLinearColor::White, FVector2D _Translation = FVector2D(0,0),
		FVector2D _Scale = FVector2D(1.0f, 1.0f), float _PlaybackSpeed = 1.0f, EFloatingTextAnimType _AnimType = EFloatingTextAnimType::Shaking);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Input")
	static FKey GetActionKey(UInputAction* IA, bool IsGamePad);
	

	template<typename T>
	static bool CalRandomMap(const TMap<T, int32>& M, T& Result);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Math")
	static bool CalRandMap_Float(const TMap<float, int32>& M, float& Result);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Math")
	static bool CalRandMap_Int(const TMap<int, int32>& M, int& Result);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Math")
	static bool CalRandMap_Vector2D(const TMap<FVector2D, int32>& M, FVector2D& Result);


	template<typename T>
	static bool CalAverageByArray(const TArray<T>& Array, T& Result);
	
};

template <typename T>
bool UCommonFuncLib::CalRandomMap(const TMap<T, int32>& M, T& Result)
{
	if (M.IsEmpty()) return false;
	
	int Total = 0;
	for (const auto& Pair : M)
	{
		Total += Pair.Value;
	}
	if (Total <= 0) return false;

	const int32 R = FMath::RandHelper(Total);
	int V = 0;
	for (const auto& Pair : M)
	{
		V += Pair.Value;
		if (V>R)
		{
			Result = Pair.Key;
			return true;
		}
	}
	return false;
}

template <typename T>
bool UCommonFuncLib::CalAverageByArray(const TArray<T>& Array, T& Result)
{
	if (Array.IsEmpty()) return false;
	
	T TmpValue;
	for (const auto& Item : Array)
	{
		TmpValue += Item;
	}

	Result = TmpValue / Array.Num();
	return true;
}
