// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "PXHUD.generated.h"

namespace EEndPlayReason { enum Type : int; }

class AActor;
class UObject;

UCLASS(Config = Game)
class PIXEL2DKIT_API APXHUD : public AHUD
{
	GENERATED_BODY()
	
	public:
    	APXHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    
    protected:
    
    	//~UObject interface
    	virtual void PreInitializeComponents() override;
    	//~End of UObject interface
    
    	//~AActor interface
    	virtual void BeginPlay() override;
    	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    	//~End of AActor interface
    
    	//~AHUD interface
    	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;
};
