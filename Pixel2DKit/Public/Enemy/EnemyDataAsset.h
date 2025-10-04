// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "EnemyDataAsset.generated.h"


UCLASS(Blueprintable)
class PIXEL2DKIT_API UEnemyDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EnemyName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> EnemyLevelDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> DieSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UNiagaraSystem> NS_Die;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UNiagaraSystem> NS_Hurt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> HurtSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	TArray<FGameplayTag> ActionFieldsCanAttack = {
		FGameplayTag::RequestGameplayTag("ActionField.East.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.East.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.East.Remote"),
		FGameplayTag::RequestGameplayTag("ActionField.East.Far"),
		FGameplayTag::RequestGameplayTag("ActionField.West.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.West.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.West.Remote"),
		FGameplayTag::RequestGameplayTag("ActionField.West.Far"),
		FGameplayTag::RequestGameplayTag("ActionField.South.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.South.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.North.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.North.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.North.Remote"),
		FGameplayTag::RequestGameplayTag("ActionField.North.Far")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> InitAbilitiesToGive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> InitEffects;

};
