// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 相对玩家的行动区间
UENUM(BlueprintType)
enum EActionField : uint8
{
	// 玩家无效时
	None,
	
	EastNear,
	EastMid,
	EastFar,
	EastRemote,

	NorthNear,
	NorthMid,
	NorthFar,
	NorthRemote,

	WestNear,
	WestMid,
	WestFar,
	WestRemote,

	SouthNear,
	SouthMid,
	SouthFar,
	SouthRemote
};