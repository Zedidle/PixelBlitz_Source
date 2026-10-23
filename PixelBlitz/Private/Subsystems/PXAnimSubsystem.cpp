// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PXAnimSubsystem.h"

UPXAnimSubsystem* UPXAnimSubsystem::GetInstance(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;
	
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return nullptr;
	
	return GameInstance->GetSubsystem<UPXAnimSubsystem>();
}
