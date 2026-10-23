// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/PXListView.h"
#include "UI/Common/PXWidgetFactory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXListView)

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif

#define LOCTEXT_NAMESPACE "PXListView"

UPXListView::UPXListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR

void UPXListView::ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const
{
	Super::ValidateCompiledDefaults(InCompileLog);

	if (FactoryRules.Num() == 0)
	{
		InCompileLog.Error(FText::Format(FText::FromString("{0} has no Factory Rules defined, can't create widgets without them."), FText::FromString(GetName())));
	}
}

#endif

UUserWidget& UPXListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSubclassOf<UUserWidget> WidgetClass = DesiredEntryClass;
	
	for (const UPXWidgetFactory* Rule : FactoryRules)
	{
		if (Rule)
		{
			if (const TSubclassOf<UUserWidget> EntryClass = Rule->FindWidgetClassForData(Item))
			{
				WidgetClass = EntryClass;
				break;
			}
		}
	}
	
	UUserWidget& EntryWidget = GenerateTypedEntry<UUserWidget>(WidgetClass, OwnerTable);

	return EntryWidget;
}

#undef LOCTEXT_NAMESPACE
