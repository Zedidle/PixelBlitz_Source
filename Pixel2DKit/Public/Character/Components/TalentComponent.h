// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "Components/ActorComponent.h"
#include "Fight/Skills/BaseDefenseSkill.h"
#include "TalentComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UTalentComponent : public UActorComponent
{
	GENERATED_BODY()

	bool Loaded = false;
	
	UPROPERTY()
	ABasePXCharacter* PXCharacter;

	FVector OwnerPreLocation;
	
	UPROPERTY()
	TArray<ABaseDefenseSkill*> DefenseSkills;

	UPROPERTY()
	TMap<EAbilityTiming, FGameplayTagArray> AbilitiesTiming;

	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseSkill>> AbilitiesHolding;
	
#pragma region TalentSet


	// 热身
	int WarmUP_Power = 0;
	int WarmUP_MoveDistance = 0;

	
#pragma endregion


	
public:	
	// Sets default values for this component's properties
	UTalentComponent();

	UFUNCTION()
	void InitTalents();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	UPXASComponent* CachedASC;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Event_OnPlayerAttackStart(EAttackType Type, FVector Direction);

	UFUNCTION()
	void RegisterDefenseSkill(ABaseDefenseSkill* Skill);

	UFUNCTION()
	void RemoveDefenseSkill(FGameplayTag Tag);
	
	UFUNCTION()
	void OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage, bool bForce = false);
	
	
	UFUNCTION(BlueprintCallable)
	void LoadTalents();

	UFUNCTION()
	ABaseSkill* SpawnSkill(TSubclassOf<ABaseSkill> SkillClass, const FTransform& SpawnTransform = FTransform());

	// 成功造成伤害时……
	UFUNCTION(BlueprintCallable)
	void OnBuffCalDamage();

	UFUNCTION(BlueprintCallable)
	void OnDying(int& RemReviveTimes);
	
	UFUNCTION(BlueprintCallable)
	void OnSkillFinish();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetAttackDamagePlus();


#pragma region Talents专项处理
	
	UFUNCTION()
	void MoveWarmingUP();


	UFUNCTION()
	void MakeMiracleWalker();

	UFUNCTION()
	void MakeImmortalPower(bool First);
	
#pragma endregion 

};
