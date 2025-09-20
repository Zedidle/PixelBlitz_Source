// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "UIDefine.h"
#include "PXUIRoot.generated.h"


USTRUCT(BlueprintType)
struct FActivatableWidgetStackToRegister
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "UI.Layer"))
	FGameplayTag Tag;

	// 在蓝图Widget界面中赋值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCommonActivatableWidgetStack* WidgetStack; 
};

class UCanvasPanel;

UCLASS()
class PIXEL2DKIT_API UPXUIRoot : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadWrite, Category = RootUI, meta = (Categories = "UI.Layer"))
	TArray<FActivatableWidgetStackToRegister> WidgetStacksToRegister;
	
public:
	UFUNCTION(BlueprintCallable)
	void RootRemoveChild(UCommonActivatableWidget* ActivatableWidget, const FGameplayTag& LayerTag);

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerName))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}
	
	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName);

protected:
	
	UFUNCTION(BlueprintCallable, Category="Layer")
	void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	UPROPERTY()
	TMap<EUILayerType, UCanvasPanel*> LayerMap;
	static void SetCanvas(UUserWidget* UIWidget);

private:
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;

};
