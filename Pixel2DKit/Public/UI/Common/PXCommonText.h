// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "Utilitys/LocalizationFuncLib.h"
#include "PXCommonText.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXCommonText : public UCommonTextBlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Content, meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	FLocalizedTableData LocalizedTableData;

protected:
	virtual TAttribute<FText> GetDisplayText() override;

	
	
public:
	virtual void PostLoad() override;

	UFUNCTION(BlueprintCallable)
	void UpdateText();

	UFUNCTION(BlueprintCallable)
	void SetLocalizedTableData(const FLocalizedTableData& _LocalizedTableData);
	
};
