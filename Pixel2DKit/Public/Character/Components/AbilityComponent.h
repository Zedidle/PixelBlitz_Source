// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GAS/PXASComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "UI/QTE/ArrowLineWidget.h"
#include "UI/QTE/KeyPressCountDownWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "AbilityComponent.generated.h"


class ABasePXCharacter;
class ABaseEnemy;
class UInputAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()



	UPROPERTY()
	ABasePXCharacter* PXCharacter;

	UPROPERTY()
	UKeyPressCountDownWidget* KeyPressCountDownWidget;

	UPROPERTY()
	UArrowLineWidget* ArrowLineWidget;
	
	
public:	
	// Sets default values for this component's properties
	UAbilityComponent();

	UPROPERTY()
	TArray<UDataTable*> AbilityDataTables;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKeyPressCountDownWidget> KeyPressCountdownWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UArrowLineWidget> ArrowLineWidgetClass;

	
	
	UPROPERTY(BlueprintReadOnly)
	AActor* HurtMaker;
	UPROPERTY(BlueprintReadOnly)
	int AcceptDamage;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> TempTestAbilities;
	
	// 选择了的技能列表
	UPROPERTY(BlueprintReadWrite)
	TArray<FGameplayTag> ChosenAbilities;
	
	// 生效的技能列表（同一系列的技能，高等级会覆盖低等级的）
	UPROPERTY(BlueprintReadWrite)
	TArray<FGameplayTag> TakeEffectAbilities;

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer CreateGameplayTagContainer(FName TagName, bool WithChildren = false);

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UPXASComponent* CachedASC;


#pragma region InputAction
	UPROPERTY(EditDefaultsOnly)
	UInputAction* Action_Jump;
	
	
#pragma	endregion
	
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void InitAbilities();
	
	UFUNCTION(BlueprintCallable)
	void LearnAbility(const FGameplayTag& AbilityTag);
	
	UFUNCTION(BlueprintCallable)
	void LoadAbility();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasChosenAbility(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanLearnAbility(const FAbility& Ability);

	UFUNCTION(BlueprintCallable)
	void OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage);



	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	void CreateQTE(float _SustainTime = 1.5f, float _Scale = 1.0f);

	UFUNCTION(BlueprintCallable)
	void OnKeyPressed(const FName& TagName, bool& Keep);

	UFUNCTION(BlueprintCallable)
	void OnHurtInstigatorDead(ABaseEnemy* DeadEnemy);

	UFUNCTION()
	void ListenHurtInstigatorDead();

	UFUNCTION()
	FGameplayAbilitySpecHandle GetGameplayAbilityWithTag(const FGameplayTag& Tag);

	FGameplayAbilitySpec* GetAbilitySpec(FName TagName);

};

inline FGameplayAbilitySpec* UAbilityComponent::GetAbilitySpec(FName TagName)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(CachedASC, nullptr)
	
	const FGameplayAbilitySpecHandle AbilitySpecHandle = GetGameplayAbilityWithTag(FGameplayTag::RequestGameplayTag(TagName));
	if (!AbilitySpecHandle.IsValid()) return nullptr;

	return CachedASC->FindAbilitySpecFromHandle(AbilitySpecHandle);
}

