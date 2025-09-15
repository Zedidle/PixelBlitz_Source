// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "Components/ActorComponent.h"
#include "EnergyComponent.generated.h"




DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnEnergyChanged,
	int32,
	NewValue,
	EStatChange,
	Change,
	AActor*,
	Instigator
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMaxEnergyChanged,
	int32,
	NewValue,
	EStatChange,
	Change
);





UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UEnergyComponent : public UActorComponent
{
	GENERATED_BODY()


	void Recover();
	
	
public:	
	// Sets default values for this component's properties
	UEnergyComponent();

	UPROPERTY(BlueprintReadWrite)
	ABasePXCharacter* PXCharacter;

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int32 CalEPCustomInFact(int32 Amount);
	
	UFUNCTION(BlueprintCallable)
	bool DecreaseEP(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncreaseEP(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void SetEP(int32 NewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentEP();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetMaxEP();

	UFUNCTION(BlueprintCallable)
	void SetMaxEP(int32 NewValue);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEPEnough(int32 Cost, bool bNeedTip = false);
};
