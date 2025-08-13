// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PXCustomStruct.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXCustomStruct : public UObject
{
	GENERATED_BODY()
	
};



#pragma region Struct


#pragma endregion



#pragma region Enum

UENUM(BlueprintType)
enum EWeather : uint8
{
	Overcast UMETA(DisplayName = "阴"),
	Fog UMETA(DisplayName = "雾"),
	Sand UMETA(DisplayName = "沙"),
	Snow UMETA(DisplayName = "雪"),
	Rain UMETA(DisplayName = "雨"),
	Sunny UMETA(DisplayName = "晴")
	
};


#pragma endregion


USTRUCT(BlueprintType)
struct FDrop
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	TMap<FName, int> Items; // 物品及对应概率比率

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	int DropTotalNum = 1;
};


// 相对玩家的行动区间
// UENUM(BlueprintType)
// enum EActionField : uint8
// {
// 	// 玩家无效时
// 	None,
// 	
// 	EastNear,
// 	EastMid,
// 	EastFar,
// 	EastRemote,
//
// 	NorthNear,
// 	NorthMid,
// 	NorthFar,
// 	NorthRemote,
//
// 	WestNear,
// 	WestMid,
// 	WestFar,
// 	WestRemote,
//
// 	SouthNear,
// 	SouthMid,
// 	SouthFar,
// 	SouthRemote
// };