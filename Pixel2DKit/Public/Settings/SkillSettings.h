// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Fight/Skills/BaseSkill.h"

#include "SkillSettings.generated.h"


UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Skill Settings"))
class PIXEL2DKIT_API USkillSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Skill", DisplayName = "月光蛊")
	TSubclassOf<ABaseSkill> MoonLightInsect;

	UPROPERTY(Config, EditAnywhere, Category = "Skill", DisplayName = "魔法护盾")
	TSubclassOf<ABaseSkill> MagicShield;

	UPROPERTY(Config, EditAnywhere, Category = "Skill", DisplayName = "雷公助我")
	TSubclassOf<ABaseSkill> ThunderGod;

	UPROPERTY(Config, EditAnywhere, Category = "Skill", DisplayName = "光合作用")
	TSubclassOf<ABaseSkill> SunHeal;

	UPROPERTY(Config, EditAnywhere, Category = "Skill", DisplayName = "淘金者")
	TSubclassOf<ABaseSkill> GoldRush;

	UPROPERTY(Config, EditAnywhere, Category = "Skill", DisplayName = "静功")
	TSubclassOf<ABaseSkill> StaticPower;

	UPROPERTY(Config, EditAnywhere, Category = "Skill", DisplayName = "飞沙走石")
	TSubclassOf<ABaseSkill> NoSand;

	
};
