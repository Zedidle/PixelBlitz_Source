// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BaseInteractableItem.generated.h"

UCLASS()
class PIXEL2DKIT_API ABaseInteractableItem : public ABaseItem
{
	GENERATED_BODY()

	FGuid Guid;
	FVector CurSimVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	bool bSimPhysical = true;

	float OnSpawnRandomRotateAngle = 20.f;
	float OnSpawnSpeed = 200.0f;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	float SimGravity = -980;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	float Elastic  = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	float GroundHeightCheck = 20.f;
	
public:
	// Sets default values for this actor's properties
	ABaseInteractableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = InteractableItem)
	USoundBase* Sound_OnInteract;
	
	
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetVelocityOnSpawn(float RandomRotateAngle = 20, float Speed = 200);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = InteractableItem)
	bool IsOnGround();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = InteractableItem)
	void OnInteractEffect(AActor* OtherActor);
	
};
