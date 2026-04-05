// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "Fight/Summoned/BaseSummoned.h"
#include "CommonSummonSkill.generated.h"



#pragma region FUIStruct
USTRUCT(BlueprintType)
struct FSummoned : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SummonedID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SummonedName;

	// 召唤术持续时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeSpan = 5.0f;
	
	// 生成点的波动范围
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnRadius = 100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ABaseSummoned> SummonedClass;

	// 生成多个时的间隔
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<float> SpawnIntervalAtLevel;

	// 召唤物持续时间
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<float> SustainTimesAtLevel;

	// 召唤物数目
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int> SpawnNumsAtLevel;

};
#pragma endregion


UCLASS()
class PIXEL2DKIT_API ACommonSummonSkill : public ABaseSkill
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, ExposeOnSpawn = true))
	int SkillLevel = 1;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FSummoned SummonedData;
	
	FTimerHandle SummonTimer;

	UPROPERTY()
	TWeakObjectPtr<AActor> Target;
	
	
	
public:
	// Sets default values for this actor's properties
	ACommonSummonSkill();

	UFUNCTION(BlueprintCallable)
	void LoadData(FName ID, int Level);
	
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn = true))
	FName SummonedID;
	
	// 当前自我管理的召唤物
	UPROPERTY()
	TArray<ABaseSummoned*> ActivatedSummoneds;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void OnDispel(int localnum);
	
	UFUNCTION(BlueprintCallable)
	void OnOwnerDie();
	
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* _Target);

	void Summon();
	
	void End();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSpawnInterval();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSustainTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetSpawnNum();
};
