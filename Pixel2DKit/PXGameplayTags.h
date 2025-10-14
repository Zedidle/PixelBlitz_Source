// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"


namespace PXGameplayTags
{
	PIXEL2DKIT_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayFlow_OnLoadingLevel);
	PIXEL2DKIT_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayFlow_OnStartLevelSuccess);









	
	PIXEL2DKIT_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
};
