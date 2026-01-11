// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BaseInteractableItem.generated.h"

class UBoxComponent;

UCLASS()
class PIXEL2DKIT_API ABaseInteractableItem : public ABaseItem
{
	GENERATED_BODY()

	FGuid Guid;
	FVector CurSimVelocity;

	bool PreOnGround = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	bool bSimPhysical = true;

	float OnSpawnRandomRotateAngle = 20.f;
	float OnSpawnSpeed = 200.0f;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	float SimGravity = -980;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	float Elastic  = 0.3;

	// 向下检查地面的距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	float GroundHeightCheck = 100.f;

	// 从地面上浮的高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	float GroundHeightFloat = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractableItem, meta = (AllowPrivateAccess = "true"))
	bool bInteracted = false;
	
public:
	// Sets default values for this actor's properties
	ABaseInteractableItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InteractableItem)
	TObjectPtr<UBoxComponent> BoxComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = InteractableItem)
	USoundBase* Sound_OnApproach;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = InteractableItem)
	USoundBase* Sound_OnInteract;
	

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	
	UFUNCTION()
	void SetVelocityOnSpawn(float RandomRotateAngle = 20, float Speed = 200);

	UFUNCTION()
	void SetSimPhysical(bool V);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = InteractableItem)
	bool IsOnGround(float& HighUP);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = InteractableItem)
	void OnApproachEffect(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = InteractableItem)
	void OnLeaveEffect(AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = InteractableItem, meta = (DisplayName = "OnInteractEffect"))
	void BP_OnInteractEffect(AActor* OtherActor);
	UFUNCTION()
	void OnInteractEffect(AActor* OtherActor);
	
};
