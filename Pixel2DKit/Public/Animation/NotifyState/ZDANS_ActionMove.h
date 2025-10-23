// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimNotifyState.h"
#include "ZDANS_ActionMove.generated.h"

UENUM(BlueprintType)
enum class EActionMoveDirectionType: uint8
{
	FromSelf UMETA(DisplayName = "从自己开始算"),
	FromTarget UMETA(DisplayName = "从目标开始算"),
};



UCLASS()
class PIXEL2DKIT_API UZDANS_ActionMove : public UPaperZDAnimNotifyState
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FName CurveName = FName("None");

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
	


	
	void OnNotifyBegin_Implementation(UPaperZDAnimInstance* OwningInstance) const;
	void OnNotifyEnd_Implementation(UPaperZDAnimInstance* OwningInstance) const;
	
};
