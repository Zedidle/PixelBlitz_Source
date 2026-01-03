// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "Fight/Skills/BaseSkill.h"
#include "BaseDefenseSkill.generated.h"

UCLASS()
class PIXEL2DKIT_API UDefenseSkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DefenseSkill")
	FGameplayTagContainer AbilityTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DefenseSkill")
	int Priority = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DefenseSkill")
	bool StopPropagation = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DefenseSkill")
	TSoftObjectPtr<UNiagaraSystem> NS_OnEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DefenseSkill")
	TSoftObjectPtr<USoundCue> SC_OnEffect;
};


UCLASS()
class PIXEL2DKIT_API ABaseDefenseSkill : public ABaseSkill
{
	GENERATED_BODY()


	
public:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DefenseSkill")
	UDefenseSkillDataAsset* DataAsset;


	UPROPERTY(BlueprintReadOnly, Category = "DefenseSkill")
	bool StopPropagation = false;
	
	// 技能自身的防御 Box碰撞触发
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DefenseSkill")
	void OnDefenseEffect(AActor* Actor);
};
