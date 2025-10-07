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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float FightCameraOffsetFactor = 0.7f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float ViewRotateFactor = 0.15f;
	
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

	// 战斗结束，触发条件之一：玩家死亡 || 玩家逃出 || 怪物清空
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FightEnd();

	
	UPROPERTY(BlueprintReadOnly)
	UPlatformFightCountWidget* PlatformFightCountWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPlatformFightCountWidget> PlatformFightCountWidgetClass;

	UFUNCTION(BlueprintCallable)
	void ShowCountWidget();
	UFUNCTION(BlueprintCallable)
	void HideCountWidget();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ActivateFight(bool bActivate);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerIn();
	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerOut();
};

inline void APlatformFight::OnPlayerOut_Implementation()
{
}

inline void APlatformFight::OnPlayerIn_Implementation()
{
}
