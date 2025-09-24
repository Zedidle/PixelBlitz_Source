// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIEventCenter.h"

UUIEventCenter* UUIEventCenter::Get(const UObject* WorldContextObject)
{
	if (ensure(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				return GameInstance->GetSubsystem<UUIEventCenter>();
			}
		}
	}
	return nullptr;
}
