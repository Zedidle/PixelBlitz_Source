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

#pragma region TalentSet

	// 摇摆拳
	bool SwingFistPower = false;

	// 热身
	int WarmUP_Power = 0;
	int WarmUP_MoveDistance = 0;

	
#pragma endregion

	UFUNCTION()
	void InitTalents();
	
public:	
	// Sets default values for this component's properties
	UTalentComponent();

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
	ABaseSkill* SpawnSkill(TSubclassOf<ABaseSkill> Skill_Class, const FTransform& SpawnTransform = FTransform());

	// 成功造成伤害时……
	UFUNCTION(BlueprintCallable)
	void OnBuffCalDamage();

	UFUNCTION(BlueprintCallable)
	void OnDashEnd();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetAttackDamagePlus();

	
#pragma region Talents
	
	UFUNCTION()
	void MoveWarmingUP();

	UFUNCTION()
	void MakeSwingFistPower();

	UFUNCTION()
	void MakeMiracleWalker();

	UFUNCTION()
	void MakeImmortalPower(bool First);
	
#pragma endregion

};
