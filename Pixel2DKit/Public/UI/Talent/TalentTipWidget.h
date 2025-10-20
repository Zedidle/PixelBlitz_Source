// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "TalentTipWidget.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UTalentTipWidget : public UUserWidget
{
	GENERATED_BODY()


protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TalentTipWidget", meta = (BindWidgetOptional))
	UCommonTextBlock* TextBlock_Desc;
	
	
public:

	FTalent TalentData;

	UFUNCTION(BlueprintCallable, Category = "TalentTipWidget")
	void SetTalentData(const FTalent& Data);

	UFUNCTION(BlueprintCallable, Category = "TalentTipWidget")
	void UpdateData();
	
};
