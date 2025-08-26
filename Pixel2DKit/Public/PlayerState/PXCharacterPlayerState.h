// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/PXCharacterASComponent.h"
#include "PlayerState/PXPlayerState.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "GAS/AttributeSet/PXCharacterAttributeSet.h"
#include "Interfaces/Fight_Interface.h"
#include "PXCharacterPlayerState.generated.h"


class UPXAttributeSet;
class URGAbilitySystemComponent;
class UAbilityComponent;

UCLASS()
class PIXEL2DKIT_API APXCharacterPlayerState : public APXPlayerState, public IFight_Interface
{
	GENERATED_BODY()
	APXCharacterPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


public:
	
#pragma region GAS
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

		
	UFUNCTION(BlueprintCallable)
	UPXAttributeSet* GetCharacterAttributeSet() const;




protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPXCharacterASComponent* AbilitySystem = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPXCharacterAttributeSet* CharacterAttributeSet = nullptr;
	

private:

	
#pragma endregion

	
#pragma region IFight_Interface
	virtual bool GetIsAttacking() override;
	virtual bool GetIsDefending() override;
	virtual UAbilityComponent* GetAbilityComponent_Implementation() override;
	virtual APawn* GetPawn_Implementation() override;
#pragma endregion

};
