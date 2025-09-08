// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/DropSubsystem.h"
#include "BaseItem.generated.h"


UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API ABaseItem : public AActor
{
	GENERATED_BODY()

	FItemData Data;
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetData(const FItemData& ItemData);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
	void Use();
	virtual void Use_Implementation();
};
