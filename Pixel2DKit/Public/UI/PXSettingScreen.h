// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameSettingScreen.h"
#include "PXSettingScreen.generated.h"

class UPXTabListWidgetBase;
/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXSettingScreen : public UGameSettingScreen
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UGameSettingRegistry* CreateRegistry() override;

	void HandleBackAction();
	void HandleApplyAction();
	void HandleCancelChangesAction();

	virtual void OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (BindWidget, OptionalWidget = true, AllowPrivateAccess = true))
	TObjectPtr<UPXTabListWidgetBase> TopSettingsTabs;
	
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle BackInputActionData;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle ApplyInputActionData;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle CancelChangesInputActionData;

	FUIActionBindingHandle BackHandle;
	FUIActionBindingHandle ApplyHandle;
	FUIActionBindingHandle CancelChangesHandle;
};
