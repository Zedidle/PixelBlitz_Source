// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimNotify.h"
#include "ZDAN_ActionMove.generated.h"


UENUM(BlueprintType)
enum class EActionMoveDirectionType: uint8
{
	FromSelf UMETA(DisplayName = "从自己开始算"),
	FromTarget UMETA(DisplayName = "从目标开始算"),
};




UCLASS()
class PIXEL2DKIT_API UZDAN_ActionMove : public UPaperZDAnimNotify
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FName CurveName = FName("None");

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float SustainTime = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bFloat = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Distance = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EActionMoveDirectionType ActionMoveDirectionType = EActionMoveDirectionType::FromSelf;

	// 偏转角度
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float DirectionYaw = 0.0f;

	// 是否打断其它ActionMove来执行这个
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bInterrupt = true;	
	
	
	void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) const;
};
