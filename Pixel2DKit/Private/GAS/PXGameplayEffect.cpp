// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXGameplayEffect.h"

UPXGameplayEffect::UPXGameplayEffect()
{
	CachedAssetTags.AppendTags(AssetTags);
	CachedGrantedTags.AppendTags(GrantedTags);
	
}

void UPXGameplayEffect::PostLoad()
{
	Super::PostLoad();
}
