// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/SettingFuncLib.h"

#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCameraSourceDataAsset.h"
#include "Settings/Config/PXCustomSettings.h"


UTexture* USettingFuncLib::GetCameraFilter(ECameraColorFilter Filter)
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Settings, nullptr);
	
	UPXCameraResourceDataAsset* DataAsset = Settings->CameraResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(DataAsset, nullptr);

	if (DataAsset->CameraColorFilterMap.Contains(Filter))
	{
		return DataAsset->CameraColorFilterMap[Filter].LoadSynchronous();
	}
	return nullptr;
}

