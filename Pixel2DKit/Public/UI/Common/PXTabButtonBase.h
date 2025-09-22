// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "PXTabListWidgetBase.h"
#include "UI/Foundation/PXButtonBase.h"

#include "PXTabButtonBase.generated.h"

class UCommonLazyImage;
class UObject;
struct FFrame;
struct FSlateBrush;


UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class PIXEL2DKIT_API UPXTabButtonBase : public UPXButtonBase, public IPXTabButtonInterface
{
	GENERATED_BODY()
public:

	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	void SetIconBrush(const FSlateBrush& Brush);

protected:

	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const FPXTabDescriptor& TabLabelInfo) override;

private:

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};
