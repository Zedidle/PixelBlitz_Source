// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "Components/ActorComponent.h"
#include "GAS/PXASComponent.h"
#include "Interfaces/Fight_Interface.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "UI/QTE/ArrowLineWidget.h"
#include "UI/QTE/KeyPressCountDownWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "AbilityComponent.generated.h"


class ABaseDefenseSkill;
class ABasePXCharacter;
class ABaseEnemy;
class UInputAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UAbilityComponent : public UActorComponent, public IFight_Interface
{
	GENERATED_BODY()

	// 黄金品阶技能刷出的概率
	float GoldenRadio = 0.25;
	// 传说品阶技能刷出的概率
	float LegendRadio = 0.1;

	// 后续看如何扩展为怪物，角色通用
	UPROPERTY()
	ABasePXCharacter* PXCharacter;

	UPROPERTY()
	UKeyPressCountDownWidget* KeyPressCountDownWidget;

	UPROPERTY()
	UArrowLineWidget* ArrowLineWidget;
	

	// 当前可选的技能库
	UPROPERTY()
	TMap<FGameplayTag, FAbility> AbilitiesCanChoice;
	// 正在显示让选的技能（三个）
	UPROPERTY()
	TArray<FGameplayTag> AbilitiesShowing;

	bool TalentLoaded = false;

	FVector OwnerPreLocation;
	
	UPROPERTY()
	TArray<ABaseDefenseSkill*> DefenseSkills;

	UPROPERTY()
	TMap<EAbilityTiming, FGameplayTagArray> AbilitiesTiming;

	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseSkill>> SkillsHolding;
	
#pragma region TalentSet


	// 热身
	int WarmUP_Power = 0;
	int WarmUP_MoveDistance = 0;

	
#pragma endregion
	
public:	
	// Sets default values for this component's properties
	UAbilityComponent();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKeyPressCountDownWidget> KeyPressCountdownWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UArrowLineWidget> ArrowLineWidgetClass;

	UPROPERTY()
	FEffectGameplayTags AbilityExtendData;

	UPROPERTY()
	TMap<FGameplayTag, FAttributeEffectArray> AbilityAttributeEffects;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* HurtMaker;
	UPROPERTY(BlueprintReadOnly)
	int AcceptDamage;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> AbilitiesDefaultOwn;
	

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer CreateGameplayTagContainer(FName TagName, bool WithChildren = false);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY()
	UPXASComponent* CachedASC;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 开局初始化角色当前可以选择的技能
	UFUNCTION(BlueprintCallable)
	void RefreshAbilitiesCanChoice();

	UFUNCTION(BlueprintCallable)
	void LoadTalents();

#pragma region Talents专项处理
	
	UFUNCTION()
	void MoveWarmingUP();

	UFUNCTION()
	void MakeMiracleWalker();

	UFUNCTION()
	void MakeImmortalPower(bool First);
	
#pragma endregion 
	
	UFUNCTION()
	void InitTalents();
	
	UFUNCTION()
	void InitAbilities();

	UFUNCTION()
	ABaseSkill* SpawnSkill(TSubclassOf<ABaseSkill> SkillClass, const FTransform& SpawnTransform = FTransform());

	UFUNCTION()
	void RegisterDefenseSkill(ABaseDefenseSkill* Skill);

	UFUNCTION()
	void RemoveDefenseSkill(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintPure, Blueprintable)
	FAbility GetAbilityToLearn();

	UFUNCTION(BlueprintCallable)
	void LearnAbility(const FGameplayTag& AbilityTag);
	
	UFUNCTION(BlueprintCallable)
	void LoadAbilities();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasChosenAbility(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanLearnAbility(const FAbility& Ability);

	UFUNCTION(BlueprintCallable)
	void OnItemRefresh(const FGameplayTag& NewTag, const FGameplayTag& PreShowingAbility, int& RemRefreshPoints);

	
	UFUNCTION(BlueprintCallable)
	bool ChoiceAbility(const FGameplayTag& Tag, int& RemSkillPoints);

	

	UFUNCTION()
	void OnLanding();// 后续可能会补充参数

	UFUNCTION()
	void OnSkillStart();// 后续可能会补充参数

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetAttackDamagePlus();

	UFUNCTION()
	void OnAttackStart(EAttackType Type, FVector Direction);
	
	UFUNCTION()
	void OnAttackDash();// 后续可能会补充参数

	UFUNCTION()
	void OnKillEnemy(); // 后续可能会补充参数

	UFUNCTION(BlueprintCallable)
	void OnBuffCalDamage();
	
	UFUNCTION(BlueprintCallable)
	void OnDying(int& RemReviveTimes);
	
	UFUNCTION()
	void ActivateAbilityByTiming(EAbilityTiming Timing);

	
	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	void CreateQTE(float _SustainTime = 1.5f, float _Scale = 1.0f);

	UFUNCTION(BlueprintCallable)
	void OnInteract(bool& Keep);

	UFUNCTION(BlueprintCallable)
	void OnHurtInstigatorDead(ABaseEnemy* DeadEnemy);

	UFUNCTION()
	void ListenHurtInstigatorDead();

	UFUNCTION()
	FGameplayAbilitySpecHandle GetGameplayAbilityWithTag(const FGameplayTag& Tag);

	FGameplayAbilitySpec* GetAbilitySpec(FName TagName);

	UFUNCTION(BlueprintCallable)
	bool FindExtendData(FGameplayTag Tag, float& Result);

	UFUNCTION(BlueprintCallable)
	bool FindAttributeEffects(FGameplayTag Tag, FAttributeEffectArray& Result);

	UFUNCTION(BlueprintCallable)
	void ApplyAttributeEffects(FGameplayTag Tag);
	
	
#pragma region IFight_Interface
	virtual bool GetIsAttacking() override;
	virtual bool GetIsDefending() override;
	virtual void OnAttackHit_Implementation(AActor* Receiver) override;
	virtual void OnSkillHit_Implementation() override;
	virtual void OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce) override;
	virtual void OnAttackEffect_Implementation() override;
	virtual void OnAttackWeakPoint_Implementation(AActor* Receiver) override;
	virtual void OnPickGold_Implementation() override;
	virtual void OnDashEffectEnd_Implementation() override;
	
#pragma endregion


	
};

inline FGameplayAbilitySpec* UAbilityComponent::GetAbilitySpec(FName TagName)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(CachedASC, nullptr)
	
	const FGameplayAbilitySpecHandle AbilitySpecHandle = GetGameplayAbilityWithTag(FGameplayTag::RequestGameplayTag(TagName));
	if (!AbilitySpecHandle.IsValid()) return nullptr;

	return CachedASC->FindAbilitySpecFromHandle(AbilitySpecHandle);
}

