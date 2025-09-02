// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utilitys/LocalizationFuncLib.h"
#include "AchievementSubsystem.generated.h"

UENUM(BlueprintType)
enum class EAchievementType : uint8
{
	Usual UMETA(DisplayName = "通用"),
	Boss UMETA(DisplayName = "BOSS"),
	Level UMETA(DisplayName = "关卡"),
	Skill UMETA(DisplayName = "技巧"),
	Gold UMETA(DisplayName = "金币")
	
};

USTRUCT(BlueprintType)
struct FAchievement: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FLocalizedTableData AchievementName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FLocalizedTableData AchievementDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	TEnumAsByte<EAchievementType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int CompleteNum; 
};

UCLASS()
class PIXEL2DKIT_API UAchievementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AchievementSubsystem")
	void CompleteAchievement(FName AchievementRowName);
};
