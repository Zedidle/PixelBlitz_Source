// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PXSkillsDataAsset.generated.h"

class ABaseSkill;
/**
 * 
 */
UCLASS(BlueprintType)
class PIXEL2DKIT_API UPXSkillsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, Category = "Skill", DisplayName = "月光蛊")
	TSubclassOf<ABaseSkill> MoonLightInsect;

	UPROPERTY( EditAnywhere, Category = "Skill", DisplayName = "魔法护盾")
	TSubclassOf<ABaseSkill> MagicShield;

	UPROPERTY( EditAnywhere, Category = "Skill", DisplayName = "雷公助我")
	TSubclassOf<ABaseSkill> Thor;

	UPROPERTY( EditAnywhere, Category = "Skill", DisplayName = "光合作用")
	TSubclassOf<ABaseSkill> SunHeal;

	UPROPERTY( EditAnywhere, Category = "Skill", DisplayName = "淘金者")
	TSubclassOf<ABaseSkill> GoldRush;

	UPROPERTY( EditAnywhere, Category = "Skill", DisplayName = "静功")
	TSubclassOf<ABaseSkill> StaticPower;

	UPROPERTY( EditAnywhere, Category = "Skill", DisplayName = "飞沙走石")
	TSubclassOf<ABaseSkill> NoSand;

	
	
};
