// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LocalizationFuncLib.generated.h"


UENUM(BlueprintType)
enum class ELanguageEnum : uint8
{
	Chinese     UMETA(DisplayName = "中文（简体）"),
	English     UMETA(DisplayName = "英语"),
	OldChinese  UMETA(DisplayName = "中文（繁体）"),
	Japanese    UMETA(DisplayName = "日语"),
	Korean      UMETA(DisplayName = "韩语"),
	Russian     UMETA(DisplayName = "俄语"),
	Spanish     UMETA(DisplayName = "西班牙语"),
	Portuguese  UMETA(DisplayName = "葡萄牙语"),
	German      UMETA(DisplayName = "德语"),
	French      UMETA(DisplayName = "法语")
};



USTRUCT(BlueprintType)
struct FLocalizedStringData: public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Chinese;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString English;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OldChinese;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Japanese;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Korean;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Russian;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Spanish;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Portuguese;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString German;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString French;
	
};


USTRUCT(BlueprintType)
struct FLocationTableData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RowName;
	
	
};


UCLASS(Abstract, MinimalAPI)
class ULocalizationFuncLib : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LocalizationFuncLib")
	static FString GetLocalizedString(const FLocationTableData& Data);

	
	
};
