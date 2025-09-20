// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/PXWorldSubsystem.h"

UPXWorldSubsystem::UPXWorldSubsystem()
{
	bCreateInDedicatedServer = true;
	bCreateInClient = true;
}

bool UPXWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const UWorld* OwningWorld = Cast<UWorld>(Outer))
	{
		if (OwningWorld->WorldType == EWorldType::Editor || OwningWorld->WorldType == EWorldType::EditorPreview ||
			OwningWorld->WorldType == EWorldType::GamePreview)
		{
			return false;
		}
		if (OwningWorld->GetNetMode() == NM_DedicatedServer)
		{
			return ShouldCreateInDedicatedServer();
		}
		return ShouldCreateInClient();
	}
	return true;
}

USubsystem* UPXWorldSubsystem::GetInstance(const UObject* WorldContext, TSubclassOf<UPXWorldSubsystem> SubsystemClass)
{
	//check(WorldContext);

	if(WorldContext)
	{
		if (const UWorld* World = WorldContext->GetWorld())
		{
			return World->GetSubsystemBase(SubsystemClass);
		}
	}

	return nullptr;
}

USubsystem* UPXWorldSubsystem::GetInstance(const UWorld* World, TSubclassOf<UPXWorldSubsystem> SubsystemClass)
{
	if (World)
	{
		return World->GetSubsystemBase(SubsystemClass);
	}

	return nullptr;		
}