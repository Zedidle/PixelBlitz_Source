// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/PXWorldSubsystem.h"
#include "UI/Player/ComboWeakPointCountWidget.h"
#include "ComboSubsystem.generated.h"


UCLASS()
class PIXEL2DKIT_API UComboSubsystem : public UPXWorldSubsystem
{
	GENERATED_BODY()



	float GetDamagePlusPercent();

	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;
	
	UPROPERTY()
	UComboWeakPointCountWidget* ComboWeakPointCountWidget;
	
	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	int MaxComboWeakPoint = 12;
	
	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	int CurComboWeakPoint = 0;

	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	float ComboWeakPointSustainTime = 5;
	
	// 当命中弱点时，累计弱点击破连击次数
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ComboSubsystem")
	int CalDamageByComboWeakPoint(int InitDamage);
	
};
