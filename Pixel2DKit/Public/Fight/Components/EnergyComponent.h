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
	ABasePXCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RecoverLevel;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	bool DecreaseEnergy(int32 Amount, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
	void IncreaseEnergy(int32 Amount, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
	void SetEnergy(int32 NewValue, AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentEnergy();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetMaxEnergy();

	UFUNCTION(BlueprintCallable)
	void SetMaxEnergy(int32 NewValue);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEnergyEnough(int32 Cost);
};
