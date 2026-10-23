// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingAction.h"
#include "GameSettingValueScalarDynamic.h"

#include "PXSettingAction_SafeZoneEditor.generated.h"

class UGameSetting;
class UObject;


UCLASS()
class UPXSettingValueScalarDynamic_SafeZoneValue : public UGameSettingValueScalarDynamic
{
	GENERATED_BODY()

public:
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;
};

UCLASS()
class UPXSettingAction_SafeZoneEditor : public UGameSettingAction
{
	GENERATED_BODY()
	
public:
	UPXSettingAction_SafeZoneEditor();
	virtual TArray<UGameSetting*> GetChildSettings() override;

private:
	UPROPERTY()
	TObjectPtr<UPXSettingValueScalarDynamic_SafeZoneValue> SafeZoneValueSetting;
};
