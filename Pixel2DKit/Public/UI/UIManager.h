// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/PXWorldSubsystem.h"
#include "UIDefine.h"
#include "Components/SlateWrapperTypes.h"
#include "Utilitys/PXCustomStruct.h"
#include "UIManager.generated.h"


class UPXUIRoot;

UCLASS()
class PIXEL2DKIT_API UUIManager : public UPXWorldSubsystem
{
	GENERATED_BODY()

	TMap<TSubclassOf<UUserWidget>, UUserWidget*> SimpleWidgetMap;
	TArray<UUserWidget*> SimpleWidgetArray;
	
public:

#pragma region SimpleOperator
	// 简单通用 UserWidget写法，不走CommonUI流程，不用继承CommonActivatableWidget的UserWidget也能用，但是就缺少对应的控制手段
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UIManager", meta = (DeterminesOutputType = "WidgetClass"))
	UUserWidget* GetCurWidget();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UIManager", meta = (DeterminesOutputType = "WidgetClass"))
	UUserWidget* GetWidget(TSubclassOf<UUserWidget> WidgetClass);
	
	UFUNCTION(BlueprintCallable, Category = "UIManager", meta = (DeterminesOutputType = "WidgetClass"))
	UUserWidget* AddWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility = ESlateVisibility::Visible, bool HideCurrentWidget = true);

	UFUNCTION(BlueprintCallable, Category = "UIManager", meta = (DeterminesOutputType = "WidgetClass"))
	UUserWidget* ShowWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility = ESlateVisibility::Visible, bool HideCurrentWidget = true);
	
	UFUNCTION(BlueprintCallable, Category = "UIManager")
	void HideWidget(TSubclassOf<UUserWidget> WidgetClass);
#pragma endregion 



	static UUIManager* GetSelfInstance(const UObject* WorldContext);
	
	UUIManager();

	virtual bool ShouldCreateInDedicatedServer() const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UIManager")
	TSubclassOf<UUserWidget> GetWidgetClassByName(const FName& UIName);


	// 走CommonActivatableWidget流程，需要先在 DT_UIConfig 中配置好
	UFUNCTION(BlueprintCallable)
	bool OpenUI(const FName UIType);
	
	
	UFUNCTION(BlueprintCallable)
	bool CloseUI(const FName UIType);

	UFUNCTION(BlueprintCallable)
	void CloseUIByLayer(FGameplayTag InLayer);

	UFUNCTION(BlueprintCallable)
	bool IsOpen(const FName UIType);

	UFUNCTION(BlueprintCallable)
	void ShowUI(const FName UIType);

	UFUNCTION(BlueprintCallable)
	void HideUI(const FName UIType);

	UFUNCTION(BlueprintCallable)
	bool IsShow(const FName UIType);

	UFUNCTION(BlueprintCallable)
	bool IsMouseInVisibleWidgetRange();
	
	UFUNCTION(BlueprintCallable)
	UUserWidget* GetUI(const FName& UIType);

	bool GetUIConfigData(const FName& UIType, FUIConfigData& ConfigData);

	void InitRoot();

	UFUNCTION(BlueprintCallable)
	void PopupTip(const FString& Content, float Time = 1.f);
	
	// UFUNCTION(BlueprintCallable)
	// void HideItemTip();
	
protected:
	void RemoveUIFromRoot(UUserWidget* Widget, const FName& UIType, const FGameplayTag& Layer);
	bool AddUIConfigData(FUIStruct& UIData);

	UPROPERTY()
	TMap<FName, FUIConfigData> ConfigDataMap;
	
	UPROPERTY()
	TMap<FName, UUserWidget*> WidgetMap;

	TMap<FName, FGameplayTag> WidgetLayerMap;
	
	UPROPERTY()
	TWeakObjectPtr<UPXUIRoot> Root;
	
};

inline TSubclassOf<UUserWidget> UUIManager::GetWidgetClassByName(const FName& UIName)
{
	FUIConfigData ConfigData = ConfigDataMap.FindRef(UIName);
	return ConfigData.UI_WidgetClass;
}
