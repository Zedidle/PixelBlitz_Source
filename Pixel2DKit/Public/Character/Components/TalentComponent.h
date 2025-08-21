// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "Components/ActorComponent.h"
#include "TalentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UTalentComponent : public UActorComponent
{
	GENERATED_BODY()


	UPROPERTY()
	ABasePXCharacter* PXCharacter;

	FVector OwnerPreLocation;

	TMap<FGameplayTag, float> Effect_GameplayTag;
	
public:	
	// Sets default values for this component's properties
	UTalentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Event_OnPlayerAttackStart();
};
