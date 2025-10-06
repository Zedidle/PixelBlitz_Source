// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIManager.h"

#include "CommonActivatableWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/DataTableSubsystem.h"
#include "UI/PXUIRoot.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Tips/CommonTipContainerWidget.h"


UUserWidget* UUIManager::GetCurWidget()
{
	if (SimpleWidgetArray.IsEmpty()) return nullptr;
	if (UUserWidget* Widget = SimpleWidgetArray[SimpleWidgetArray.Num()-1])
	{
		return Widget;
	}
	return nullptr;
}

UUserWidget* UUIManager::GetWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return nullptr;
	return SimpleWidgetMap.FindRef(WidgetClass);
}

UUserWidget* UUIManager::AddWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility,
                                   bool HideCurrentWidget)
{
	if (!WidgetClass) return nullptr;
	
	if (SimpleWidgetMap.Contains(WidgetClass))
	{
		return ShowWidget(WidgetClass, Visibility, HideCurrentWidget);
	}

	if (UUserWidget* Widget = CreateWidget(GetWorld(), WidgetClass))
	{
		SimpleWidgetArray.Add(Widget);
		SimpleWidgetMap.Add(WidgetClass, Widget);
		Widget->AddToViewport(SimpleWidgetArray.Num());
		return Widget;
	}
	
	return nullptr;
	
}

UUserWidget* UUIManager::ShowWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility,
                                    bool HideCurrentWidget)
{
	if (!WidgetClass) return nullptr;

	if (HideCurrentWidget)
	{
		if (SimpleWidgetArray.IsValidIndex(SimpleWidgetArray.Num()-1))
		{
			SimpleWidgetArray[SimpleWidgetArray.Num()-1]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	if (SimpleWidgetMap.Contains(WidgetClass))
	{
		SimpleWidgetMap[WidgetClass]->SetVisibility(Visibility);
		return SimpleWidgetMap[WidgetClass];
	}
	return nullptr;
}

void UUIManager::HideWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return;

	if (SimpleWidgetMap.Contains(WidgetClass))
	{
		SimpleWidgetMap[WidgetClass]->SetVisibility(ESlateVisibility::Collapsed);
	}
}


UUIManager* UUIManager::GetSelfInstance(const UObject* WorldContext)
{
	UUIManager* RetUIManager = UUIManager::GetInstance<UUIManager>(WorldContext);
	return RetUIManager;
}

UUIManager::UUIManager()
{
}

bool UUIManager::ShouldCreateInDedicatedServer() const
{
	return false;
}

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUIManager::PostInitialize()
{
	Super::PostInitialize();
	// const UDataTableSettings& DataTableSettings = UDataTableSettings::Get();
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		UDataTableSubsystem* DataTableManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	
		if (UDataTable* UITable = DataTableManager->GetDataTable("UIConfig"))
		{
			TArray<FUIStruct> UIInfos = DataTableManager->GetRowMap<FUIStruct>(UITable);
			for (auto& Info : UIInfos)
			{
				AddUIConfigData(Info);
			}
		}
	}
	

}

void UUIManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

bool UUIManager::OpenUI(const FName UIType)
{
	if(WidgetMap.Find(UIType))
	{
		ShowUI(UIType);
		return true;
	}

	
	const auto& ConfigDataPtr = ConfigDataMap.Find(UIType);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(ConfigDataPtr, false)

	UClass* UIBPClass = *ConfigDataPtr->UI_WidgetClass;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(UIBPClass, false)
		
	if (Root.IsValid() == false)
	{
		InitRoot();
	}

	const auto& BPWidget = Root->PushWidgetToLayerStack(ConfigDataPtr->LayerTag, UIBPClass);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BPWidget, false)

	WidgetMap.Add(UIType, BPWidget);

	WidgetLayerMap.Add(UIType, ConfigDataPtr->LayerTag);

	if(ConfigDataPtr->LayerTag != FGameplayTag::RequestGameplayTag(FName("UI.Layer.Tips")))
	{
		if (APlayerController* SelfPC = UGameplayStatics::GetPlayerController(this, 0))
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetWidgetToFocus(BPWidget->TakeWidget());
			SelfPC->SetInputMode(InputMode);
		}
	}
	
	return true;
}

bool UUIManager::CloseUI(const FName UIType)
{
	const auto& LayerPtr = WidgetLayerMap.Find(UIType);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(LayerPtr, false)

	const auto& ConfigDataPtr = ConfigDataMap.Find(UIType);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(ConfigDataPtr, false)
	
	const auto& WidgetPtr = WidgetMap.Find(UIType);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(WidgetPtr, false)
	RemoveUIFromRoot(*WidgetPtr, UIType, ConfigDataPtr->LayerTag);

	if(WidgetMap.Num() <= 0)
	{
		if (APlayerController* SelfPC = UGameplayStatics::GetPlayerController(this, 0))
		{
			FInputModeGameOnly InputMode;
			SelfPC->SetInputMode(InputMode);
		}
	}
	
	return true;
}

void UUIManager::CloseUIByLayer(FGameplayTag InLayer)
{
	for(auto It = WidgetLayerMap.CreateIterator(); It; ++It)
	{
		if(It->Value == InLayer)
		{
			CloseUI(It->Key);
		}
	}
}

bool UUIManager::IsOpen(const FName UIType)
{
	return WidgetLayerMap.Contains(UIType);
}

void UUIManager::ShowUI(const FName UIType)
{
	if(UUserWidget* Widget = GetUI(UIType))
	{
		Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UUIManager::HideUI(const FName UIType)
{
	if(UUserWidget* Widget = GetUI(UIType))
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool UUIManager::IsShow(const FName UIType)
{
	if(UUserWidget* Widget = GetUI(UIType))
	{
		return Widget->IsVisible();
	}

	return false;
}

bool UUIManager::IsMouseInVisibleWidgetRange()
{
	const FVector2D MousePos = FSlateApplication::Get().GetCursorPos();
	FWidgetPath WidgetPath = FSlateApplication::Get().LocateWindowUnderMouse(MousePos, FSlateApplication::Get().GetInteractiveTopLevelWindows(), true);

	if(WidgetPath.IsValid())
	{
		TSharedPtr<SWidget> Widget = WidgetPath.GetLastWidget();
		if(Widget.IsValid())
		{
			FName WidgetType = Widget->GetType();
			if(!WidgetType.ToString().Contains(TEXT("Viewport")))
			{
				if(Widget->GetVisibility() == EVisibility::Visible)
				{
					return true;
				}
			}
		}
	}
	return false;
}

UUserWidget* UUIManager::GetUI(const FName& UIType)
{
	if(WidgetMap.Contains(UIType))
	{		
		return WidgetMap[UIType];
	}
	
	return nullptr;
}

bool UUIManager::GetUIConfigData(const FName& UIType, FUIConfigData& ConfigData)
{
	const auto& ConfigDataPtr = ConfigDataMap.Find(UIType);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(ConfigDataPtr, false)
	ConfigData = *ConfigDataPtr;
	return true;
}

void UUIManager::InitRoot()
{
	if (Root.IsValid()) return;
	
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	if (FUIConfigData ConfigData; GetUIConfigData("UIRoot", ConfigData))
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

		if (const auto& UIRoot = UWidgetBlueprintLibrary::Create(World, ConfigData.UI_WidgetClass, nullptr))
		{
			Root = Cast<UPXUIRoot>(UIRoot);
			if (!Root.Get()->IsInViewport())
			{
				Root.Get()->AddToViewport();
				Root.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
}

void UUIManager::PopupTip(const FString& Content, float Time)
{
	OpenUI("UI_CommonTipContainer");

	UCommonTipContainerWidget* CommonTipContainer = Cast<UCommonTipContainerWidget>(GetUI("UI_CommonTipContainer"));
	if(IsValid(CommonTipContainer))
	{
		CommonTipContainer->AddTip(Content, Time);
	}
}

void UUIManager::RemoveUIFromRoot(UUserWidget* Widget, const FName& UIType, const FGameplayTag& Layer)
{
	WidgetLayerMap.Remove(UIType);
	UCommonActivatableWidget* ActivatableWidget = Cast<UCommonActivatableWidget>(Widget);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ActivatableWidget);
	if (Root.IsValid())
	{
		Root.Get()->RootRemoveChild(ActivatableWidget, Layer);
	}
}

bool UUIManager::AddUIConfigData(FUIStruct& UIData)
{
	if (ConfigDataMap.Contains(UIData.UIName))
	{
		return false;
	}
	FUIConfigData UIConfigData;
	UIConfigData.UI_WidgetClass = UIData.WidgetClass;
	UIConfigData.LayerTag = UIData.LayerTag;
	ConfigDataMap.Add(UIData.UIName, UIConfigData);
	return true;
}


