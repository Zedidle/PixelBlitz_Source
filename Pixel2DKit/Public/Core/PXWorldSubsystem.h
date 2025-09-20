// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PXWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PIXEL2DKIT_API UPXWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPXWorldSubsystem();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual bool ShouldCreateInDedicatedServer() const { return bCreateInDedicatedServer; }
	virtual bool ShouldCreateInClient() const { return bCreateInClient; }

	template <typename TSubsystemClass>
	static FORCEINLINE TSubsystemClass* GetInstance(const UObject* WorldContext)
	{
		return Cast<TSubsystemClass>(GetInstance(WorldContext, TSubsystemClass::StaticClass()));
	}

	template <typename TSubsystemClass>
	static FORCEINLINE TSubsystemClass* GetInstance(const UWorld* World)
	{
		return Cast<TSubsystemClass>(GetInstance(World, TSubsystemClass::StaticClass()));
	};

protected:
	UPROPERTY(EditDefaultsOnly)
	bool bCreateInDedicatedServer;

	UPROPERTY(EditDefaultsOnly)
	bool bCreateInClient;

private:
	static USubsystem* GetInstance(const UObject* WorldContext, TSubclassOf<UPXWorldSubsystem> SubsystemClass);
	static USubsystem* GetInstance(const UWorld* World, TSubclassOf<UPXWorldSubsystem> SubsystemClass);
};
