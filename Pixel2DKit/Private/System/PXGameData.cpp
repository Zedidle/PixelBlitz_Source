// Fill out your copyright notice in the Description page of Project Settings.


#include "System/PXGameData.h"
#include "System/PXAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXGameData)

UPXGameData::UPXGameData()
{
}

const UPXGameData& UPXGameData::UPXGameData::Get()
{
	return UPXAssetManager::Get().GetGameData();
}
