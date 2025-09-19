// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/DebugFuncLab.h"

TAutoConsoleVariable<int32> CDebugTraceDuration(
	TEXT("debug_trace_duration"), 0,TEXT("0: disable, 1: Enable"), ECVF_Default);

void UDebugFuncLab::ScreenMessage(FString message, float time, FColor color)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, time, color, message);
}
