// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PixelAnimSubsystem.generated.h"

class UPaperZDAnimInstance;


/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPixelAnimSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:

	static void SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName, bool V);
	static void SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName, float V);
	static void SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName, int V);
};
