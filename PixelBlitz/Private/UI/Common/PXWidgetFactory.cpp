// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/PXWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXWidgetFactory)

class UUserWidget;

TSubclassOf<UUserWidget> UPXWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
