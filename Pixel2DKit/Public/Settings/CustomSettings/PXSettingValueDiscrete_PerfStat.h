// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "PXSettingValueDiscrete_PerfStat.generated.h"

enum class EPXDisplayablePerformanceStat : uint8;
enum class EPXStatDisplayMode : uint8;

class UObject;

UCLASS()
class UPXSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	UPXSettingValueDiscrete_PerfStat();

	void SetStat(EPXDisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;
	
	void AddMode(FText&& Label, EPXStatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<EPXStatDisplayMode> DisplayModes;

	EPXDisplayablePerformanceStat StatToDisplay;
	EPXStatDisplayMode InitialMode;
};
