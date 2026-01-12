// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GAS/PXASComponent.h"
#include "Interfaces/Fight_Interface.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "UI/QTE/ArrowLineWidget.h"
#include "UI/QTE/KeyPressCountDownWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "AbilityComponent.generated.h"


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


	
	
public:	
	// Sets default values for this component's properties
	UAbilityComponent();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKeyPressCountDownWidget> KeyPressCountdownWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UArrowLineWidget> ArrowLineWidgetClass;


	UPROPERTY(BlueprintReadOnly)
	AActor* HurtMaker;
	UPROPERTY(BlueprintReadOnly)
	int AcceptDamage;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> AbilitiesDefaultOwn;
	

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer CreateGameplayTagContainer(FName TagName, bool WithChildren = false);

	UPROPERTY()
	TMap<EAbilityTiming, FGameplayTagArray> AbilitiesTiming;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UPXASComponent* CachedASC;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 开局初始化角色当前可以选择的技能
	UFUNCTION(BlueprintCallable)
	void RefreshAbilitiesCanChoice();
	
	UFUNCTION()
	void InitAbilities();

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


	
	UFUNCTION()
	void OnAttackSkill();// 后续可能会补充参数

	UFUNCTION()
	void OnKillEnemy(); // 后续可能会补充参数

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


#pragma region IFight_Interface
	virtual bool GetIsAttacking() override;
	virtual bool GetIsDefending() override;
	virtual void OnSkillHit_Implementation() override;
	virtual void OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce) override;
	virtual void OnAttackEffect_Implementation() override;
	virtual void OnAttackWeakPoint_Implementation(AActor* Receiver) override;

#pragma endregion


	
};

inline FGameplayAbilitySpec* UAbilityComponent::GetAbilitySpec(FName TagName)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(CachedASC, nullptr)
	
	const FGameplayAbilitySpecHandle AbilitySpecHandle = GetGameplayAbilityWithTag(FGameplayTag::RequestGameplayTag(TagName));
	if (!AbilitySpecHandle.IsValid()) return nullptr;

	return CachedASC->FindAbilitySpecFromHandle(AbilitySpecHandle);
}

