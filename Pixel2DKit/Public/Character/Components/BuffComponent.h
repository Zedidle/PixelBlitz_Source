// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/Buff/BuffStateWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "GameplayEffect.h"
#include "NiagaraComponent.h"
#include "Character/BasePXCharacter.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "BuffComponent.generated.h"

class UPXASComponent;
// 针对PXAttributeSet设定的
USTRUCT(BlueprintType)
struct FAttributeEffectData
{
	GENERATED_BODY()

	TMap<FGameplayTag, FAttributeEffect> Tag2BuffEffect;

	float Percent;
	float Value;
	
	void AddBuffEffect(const FGameplayTag& Tag, const FAttributeEffect& Effect)
	{
		Tag2BuffEffect.Add(Tag, Effect);
		Percent += Effect.EffectedPercent;
		Value += Effect.EffectedValue;
	}

	void RemoveBuffEffect(const FGameplayTag& Tag)
	{
		if (FAttributeEffect* Effect = Tag2BuffEffect.Find(Tag))
		{
			Percent -= Effect->EffectedPercent;
			Value -= Effect->EffectedValue;
			Tag2BuffEffect.Remove(Tag);
		}
	}
};


class UDataTableSettings;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	ACharacter* Owner;

	// Buff所影响的属性列表，这里没有的Key，AttributeEffects 则没影响
	TMap<FGameplayTag, TArray<EPXAttribute>> Tag2AttributeNames;
	// 属性中被不同Buff的影响值
	TMap<EPXAttribute, FAttributeEffectData> AttributeEffects;
	

	UPROPERTY()
	TMap<FGameplayTag, UNiagaraComponent*> Tag2Niagara;
	
public:	
	// Sets default values for this component's properties
	UBuffComponent();

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	UFUNCTION()
	void InitData();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBuffStateWidget> BuffStateWidgetClass;
	UPROPERTY()
	UBuffStateWidget* BuffStateWidget = nullptr;

	FName TimerName_CheckBuffEnd;
	UFUNCTION()
	void CheckBuffExpire();
	
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	UPXASComponent* CachedASC;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool BuffExist(FGameplayTag Tag) const;


	UFUNCTION(BlueprintCallable)
	void SetBuffStateWidgetVisibility(ESlateVisibility InVisibility);

	UFUNCTION()
	void OnGameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle);

	UFUNCTION()
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& GameplayEffect);
	
	UFUNCTION(BlueprintCallable)
	void RemoveBuff(const FGameplayTag& Tag, bool OnlySelf=true);
	void RemoveBuff(TMap<FGameplayTag, TArray<EPXAttribute>>& RemoveEffects);

	UFUNCTION(BlueprintCallable)
	void AddBuffOnWidget(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor, bool Permanent);
	void RemoveBuffOnWidget(FGameplayTag Tag, bool OnlySelf);

	
	UFUNCTION(BlueprintCallable, Category="Buff | BuffText")
	void AddBuffByTag(FGameplayTag Tag, bool bNeedPermanent = false);

	UFUNCTION(BlueprintCallable, Category="Buff | BuffText")
	void ExpireBuff(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	void AddAttributeEffect(const FGameplayTag& Tag, EPXAttribute Attribute, float Percent = 0.0f, float Value = 1.f, float Duration = 9999.f);
	
	void AddAttributeEffect(const FGameplayTag& Tag, const FAttributeEffect& Effect);
	void AddAttributeEffects(const FGameplayTag& Tag, const TArray<FAttributeEffect>& Effects);
	
	UFUNCTION(BlueprintCallable)
	void RemoveAttributeEffect(EPXAttribute AttributeName, const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveAttributeEffectsByTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetAttributeNameByEnum(EPXAttribute AttributeName) const;
	
};
