// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Foundation/PXButtonBase.h"
#include "Utilitys/PXCustomStruct.h"
#include "TalentSelectItemWidget.generated.h"


class UTalentTipWidget;
class UCommonTextBlock;

UCLASS()
class PIXEL2DKIT_API UTalentSelectItemWidget : public UPXButtonBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TalentSelectItemWidget", meta = (AllowPrivateAccess = "true"))
	FVector2D TipOffset = {-420, -400};

	UPROPERTY(BlueprintReadOnly, Category = "TalentSelectItemWidget", meta = (AllowPrivateAccess = "true"))
	UTalentTipWidget* TalentTipWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TalentSelectItemWidget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UTalentTipWidget> TalentTipWidgetClass;

	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCommonTextBlock* Text_TalentName;
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "TalentSelectItemWidget", meta = (ExposeOnSpawn = true))
	FTalent TalentData;



	UFUNCTION()
	void Event_OnHovered();
	UFUNCTION()
	void Event_OnUnhovered();
	
	
};
