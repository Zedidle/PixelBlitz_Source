// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/DebugFuncLab.h"

void UDebugFuncLab::ScreenMessage(FString message, float time, FColor color)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, time, color, message);
}
