// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "EnemyDataAsset.generated.h"

class USoundCue;
class APaperDeathEffect;

UCLASS(Blueprintable)
class PIXELBLITZ_API UEnemyDataAsset : public UDataAsset
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

	// 死亡表现：纸片撕裂并落地融为颜料滩（撕裂成功会隐藏原尸体）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Death")
	bool bDeathPaperTear = true;

	// 撕裂效果融化后的颜料颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Death")
	FLinearColor DeathPaintColor = FLinearColor(0.55f, 0.1f, 0.12f, 1.f);

	// 自定义撕裂效果类（留空使用 APaperDeathEffect 默认参数）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Death")
	TSubclassOf<APaperDeathEffect> PaperDeathEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	TArray<FGameplayTag> ActionFieldsCanAttack = {
		FGameplayTag::RequestGameplayTag("ActionField.East.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.East.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.East.Far"),
		FGameplayTag::RequestGameplayTag("ActionField.West.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.West.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.West.Far"),
		FGameplayTag::RequestGameplayTag("ActionField.South.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.South.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.North.Near"),
		FGameplayTag::RequestGameplayTag("ActionField.North.Mid"),
		FGameplayTag::RequestGameplayTag("ActionField.North.Far")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> InitAbilitiesToGive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> InitEffects;

};
