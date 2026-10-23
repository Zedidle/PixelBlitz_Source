// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/PXTabButtonBase.h"
#include "CommonLazyImage.h"
#include "UI/Common/PXTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXTabButtonBase)

class UObject;
struct FSlateBrush;

void UPXTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UPXTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UPXTabButtonBase::SetTabLabelInfo_Implementation(const FPXTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

