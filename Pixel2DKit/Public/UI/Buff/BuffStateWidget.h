// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffFloatingTextWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "BuffStateWidget.generated.h"

class UVerticalBox;
class UUserWidget;
class UBuffPermanentFloatingTextWidget;


UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UBuffStateWidget : public UUserWidget
{
	GENERATED_BODY()


	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> Tag2Widget;

	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UVerticalBox* VerticalBox_BuffIn;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UVerticalBox* VerticalBox_BuffOut;

	UPROPERTY(meta=(BindWidgetOptional))
	UVerticalBox* VerticalBox_BuffPermanent;


	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buff")
	TSubclassOf<UBuffFloatingTextWidget> BuffFloatingText_WidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buff")
	TSubclassOf<UBuffPermanentFloatingTextWidget> BuffFloatingTextPermanent_WidgetClass;

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BuffIn(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BuffOut(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BuffPermanent(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor);

	
};
