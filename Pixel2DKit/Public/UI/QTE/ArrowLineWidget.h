// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArrowLineBodyWidget.h"
#include "ArrowLineHeadAntiWidget.h"
#include "ArrowLineHeadWidget.h"
#include "Blueprint/UserWidget.h"
#include "ArrowLineWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UArrowLineWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> ArrowParts;
	
	int TmpMaxBodyNum = 0;
	int TmpIndex = 0;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> TmpWidgetObj;
	


	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess, ExposeOnSpawn))
	bool TwoWay = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess, ExposeOnSpawn))
	int MaxArrowBodyNum = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	float MaxDistance = 600;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	FLinearColor Color = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	float LifeSpan = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	FVector2D BodySize = FVector2D(36, 54);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	FVector2D HeadSize = FVector2D(64, 64);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	FVector StartOffset = FVector::Zero();


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UArrowLineHeadWidget> ArrowHeadWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UArrowLineBodyWidget> ArrowBodyWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UArrowLineHeadAntiWidget> ArrowHeadAntiWidgetClass;
	UPROPERTY()
	UArrowLineHeadAntiWidget* ArrowHeadAntiWidget;


	UFUNCTION()
	void Tick_Update();
	
	UFUNCTION()
	void UpdateWidget();

	
public:

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	AActor* StartActor;

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	AActor* EndActor;
	
	UFUNCTION(BlueprintCallable)
	void InitializeData(AActor* _StartActor, AActor* _EndActor, bool _TwoWay = true, int _MaxArrowBodyNum = 10,
		float _MaxDistance = 600, FLinearColor _Color = FLinearColor::White, float _LifeSpan = 0.f,
		FVector2D _BodySize = FVector2D(36, 54), FVector2D _HeadSize = FVector2D(64, 64),
		FVector _StartOffset = FVector(0)
		);
	
	UFUNCTION(BlueprintCallable)
	void RemoveArrowLine();
	
	UFUNCTION()
	void SetHide(bool bHide);
};
