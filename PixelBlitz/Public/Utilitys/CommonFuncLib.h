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
	Heal,
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

#pragma region EChineseHour
// 十二时辰：每个时辰 2 小时（子时跨零点）
// 用途：LevelWeatherRate.TimeOfDayRate 的时段权重键；经 ChineseHourToTime 换算成 PassDayTime 的 Time
UENUM(BlueprintType)
enum class EChineseHour : uint8
{
	Zi UMETA(DisplayName = "子时 23:00-01:00"),
	Chou UMETA(DisplayName = "丑时 01:00-03:00"),
	Yin UMETA(DisplayName = "寅时 03:00-05:00"),
	Mao UMETA(DisplayName = "卯时 05:00-07:00"),
	Chen UMETA(DisplayName = "辰时 07:00-09:00"),
	Si UMETA(DisplayName = "巳时 09:00-11:00"),
	Wu UMETA(DisplayName = "午时 11:00-13:00"),
	Wei UMETA(DisplayName = "未时 13:00-15:00"),
	Shen UMETA(DisplayName = "申时 15:00-17:00"),
	You UMETA(DisplayName = "酉时 17:00-19:00"),
	Xu UMETA(DisplayName = "戌时 19:00-21:00"),
	Hai UMETA(DisplayName = "亥时 21:00-23:00"),
};
#pragma endregion

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


	template<typename T>
	static bool CalRandomMap(const TMap<T, int32>& M, T& Result);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Math")
	static bool CalRandMap_Float(const TMap<float, int32>& M, float& Result);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Math")
	static bool CalRandMap_Int(const TMap<int, int32>& M, int& Result);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Math")
	static bool CalRandMap_Vector2D(const TMap<FVector2D, int32>& M, FVector2D& Result);

	// 十二时辰权重表随机
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Weather")
	static bool CalRandMap_ChineseHour(const TMap<EChineseHour, int32>& M, EChineseHour& Result);

	// 十二时辰 → 该时辰在 PassDayTime.Time 刻度（HHMM，如 1930 = 19:30）上的区间
	// 子时跨零点：[23:00, 24:00) 或 [00:00, 01:00)，bRandomRange 决定取哪一半
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Weather")
	static FVector2D GetChineseHourRange(EChineseHour Hour, bool bRandomRange = true);

	// 十二时辰 → 具体时间（HHMM 刻度）；bRandom=true 时段内随机，false 取时段中点
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Weather")
	static float ChineseHourToTime(EChineseHour Hour, bool bRandom = true);

	// 一步到位：按权重抽时辰，再在时段内随机，直接得到 PassDayTime 的 Time
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Weather")
	static float CalRandTimeByChineseHourRate(const TMap<EChineseHour, int32>& M, bool& bSuccess);


	template<typename T>
	static bool CalAverageByArray(const TArray<T>& Array, T& Result);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Common | Time")
	static float CalEndTime(float Duration);
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
	
	T TmpValue = {};
	for (const auto& Item : Array)
	{
		TmpValue += Item;
	}

	
	Result = TmpValue / Array.Num();
	
	return true;
}
