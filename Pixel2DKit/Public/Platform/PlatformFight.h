// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "Enemy/BaseEnemy.h"
#include "Enemy/EnemySpawner.h"
#include "Platform/BasePlatform.h"
#include "UI/Platform/PlatformFightCountWidget.h"
#include "PlatformFight.generated.h"


UCLASS()
class PIXEL2DKIT_API APlatformFight : public ABasePlatform
{
	GENERATED_BODY()

	float InRangeA = 0.0f;
	float OutRangeB = -0.36f;

public:

	UPROPERTY(BlueprintReadWrite)
	ABasePXCharacter* PXCharacter;

	UPROPERTY(BlueprintReadOnly)
	TArray<ABaseEnemy*> Enemies;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AEnemySpawner*> EnemySpawners;

	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RegisterEnemies();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnEnemyDie(ABaseEnemy* enemy);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FightEnd();

	
	UPROPERTY(BlueprintReadOnly)
	UPlatformFightCountWidget* PlatformFightCountWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPlatformFightCountWidget> PlatformFightCountWidgetClass;

	UFUNCTION(BlueprintCallable)
	void ShowCountWidget(bool bShow);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ActivateFight(bool bActivate);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
