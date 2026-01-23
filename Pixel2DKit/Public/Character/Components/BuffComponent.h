// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Buff_Interface.h"
#include "UI/Buff/BuffStateWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "GameplayEffect.h"
#include "NiagaraComponent.h"
#include "BuffComponent.generated.h"

// 针对PXAttributeSet设定的
USTRUCT(BlueprintType)
struct FAttributeEffectMap
{
	GENERATED_BODY()

	TMap<FGameplayTag, FBuffEffect> Tag2BuffEffect;

	void AddBuffEffect(const FGameplayTag& Tag, const FBuffEffect& Effect)
	{
		Tag2BuffEffect.Add(Tag, Effect);
	}

	void RemoveBuffEffect(const FGameplayTag& Tag)
	{
		Tag2BuffEffect.Remove(Tag);
	}
};


class UDataTableSettings;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UBuffComponent : public UActorComponent, public IBuff_Interface
{
	GENERATED_BODY()

	UPROPERTY()
	ACharacter* Owner;

	TMap<FString, FAttributeEffectMap> AttributeEffects;
	TMap<FGameplayTag, TArray<FString>> Tag2AttributeNames;
	
	// 扩展BuffComponent为支持所有 UPXAttributeSet 的属性
	TMap<FGameplayTag, FBuffEffect> Tag2BuffEffect_Speed;
	TMap<FGameplayTag, FBuffEffect> Tag2BuffEffect_Attack;
	TMap<FGameplayTag, FBuffEffect> Tag2BuffEffect_Sight;

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

	UPROPERTY(BlueprintReadOnly)
	float EffectedValue_Speed;
	UPROPERTY(BlueprintReadOnly)
	float EffectedPercent_Speed;

	UPROPERTY(BlueprintReadOnly)
	float EffectedValue_Attack;
	UPROPERTY(BlueprintReadOnly)
	float EffectedPercent_Attack;

	UPROPERTY(BlueprintReadOnly)
	float EffectedValue_Sight;
	UPROPERTY(BlueprintReadOnly)
	float EffectedPercent_Sight;

	FName TimerName_CheckBuffEnd;
	UFUNCTION()
	void CheckBuffExpire();
	
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool BuffExist(FGameplayTag Tag) const;


	// 相对重要的属性
	UFUNCTION()
	void RemoveBuff_Attack(FGameplayTag Tag);
	UFUNCTION()
	void RemoveBuff_Sight(FGameplayTag Tag);
	UFUNCTION()
	void RemoveBuff_Speed(FGameplayTag Tag);
	UFUNCTION()
	void RemoveBuff_EffectAll(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	void SetBuffStateWidgetVisibility(ESlateVisibility InVisibility);

	UFUNCTION()
	void OnGameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle);

	UFUNCTION()
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& GameplayEffect);
	
#pragma region IBuff_Interface
	virtual void BuffEffect_Speed_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime) override;
	virtual void BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime) override;
	virtual void BuffEffect_Sight_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime) override;
	virtual void RemoveBuff_Implementation(const FGameplayTag& Tag, bool OnlySelf=true);
#pragma endregion

	virtual void AddBuffOnWidget(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor, bool Permanent);
	virtual void RemoveBuffOnWidget(FGameplayTag Tag, bool OnlySelf);

	
	UFUNCTION(BlueprintCallable, Category="Buff | BuffText")
	void AddBuffByTag(FGameplayTag Tag, bool bNeedPermanent = false);

	UFUNCTION(BlueprintCallable, Category="Buff | BuffText")
	void ExpireBuff(FGameplayTag Tag);


	UFUNCTION(BlueprintCallable)
	void AddAttributeEffect(const FString& AttributeName, const FGameplayTag& Tag, const FBuffEffect& Effect);
	
	UFUNCTION(BlueprintCallable)
	void RemoveAttributeEffect(const FString& AttributeName, const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveAttributeEffectsByTag(const FGameplayTag& Tag);

	
};


inline void UBuffComponent::RemoveBuff_EffectAll(FGameplayTag Tag)
{
	RemoveBuff_Attack(Tag);
	RemoveBuff_Sight(Tag);
	RemoveBuff_Speed(Tag);
	
	if (Tag2Niagara.Contains(Tag))
	{
		Tag2Niagara[Tag]->DestroyComponent();
		Tag2Niagara.Remove(Tag);
	}
}

