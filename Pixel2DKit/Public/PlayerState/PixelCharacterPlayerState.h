// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerState/PixelPlayerState.h"
#include "PixelCharacterPlayerState.generated.h"

class UPixelAttributeSet;
class URGAbilitySystemComponent;

UCLASS()
class PIXEL2DKIT_API APixelCharacterPlayerState : public APixelPlayerState
{
	GENERATED_BODY()
	APixelCharacterPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


public:
	
#pragma region GAS
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

		
	UFUNCTION(BlueprintCallable)
	UPixelAttributeSet* GetCharacterAttributeSet() const;




protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPixelAbilitySystemComponent* AbilitySystem = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPixelAttributeSet* CharacterAttributeSet = nullptr;


	

private:

	
#pragma endregion

};
