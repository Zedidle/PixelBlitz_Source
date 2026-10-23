// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"


namespace PXGameplayTags
{
	PIXELBLITZ_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayFlow_OnLevelLoading);
	PIXELBLITZ_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayFlow_OnLevelLoaded);
	PIXELBLITZ_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayFlow_OnLevelStarted);
	PIXELBLITZ_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayFlow_OnEnemyDie);

	







	
	PIXELBLITZ_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
};
