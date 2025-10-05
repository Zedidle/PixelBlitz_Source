// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tips/PressAnyKey.h"

#include "Animation/WidgetAnimation.h"
#include "Subsystems/TimerSubsystemFuncLib.h"

void UPressAnyKey::NativeOnActivated()
{
	Super::NativeOnActivated();
	OnKeySelected.AddUObject(this, &ThisClass::HandleKeyPressed);
}

void UPressAnyKey::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
}

void UPressAnyKey::HandleKeyPressed_Implementation(FKey InKey)
{
	RemoveFromParent();

}


