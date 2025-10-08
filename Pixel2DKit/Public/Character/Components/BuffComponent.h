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


class UDataTableSettings;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UBuffComponent : public UActorComponent, public IBuff_Interface
{
	GENERATED_BODY()

	UPROPERTY()
	ACharacter* Owner;
	
	TMap<FGameplayTag, FBuffValueEffect> Tag2BuffEffect_Speed;
	TMap<FGameplayTag, float> Tag2BuffEndTime_Speed;
	TMap<FGameplayTag, FBuffValueEffect> Tag2BuffEffect_Attack;
	TMap<FGameplayTag, float> Tag2BuffEndTime_Attack;
	TMap<FGameplayTag, FBuffValueEffect> Tag2BuffEffect_Sight;
	TMap<FGameplayTag, float> Tag2BuffEndTime_Sight;

	UPROPERTY()
	TMap<FGameplayTag, UNiagaraComponent*> Tag2Niagara;
	
public:	
	// Sets default values for this component's properties
	UBuffComponent();

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

	virtual void EndPlay();
	
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
	virtual void BuffUpdate_Speed_Implementation() override;
	virtual void BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime) override;
	virtual void BuffUpdate_Attack_Implementation() override;
	virtual void BuffEffect_Sight_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime) override;
	virtual void BuffUpdate_Sight_Implementation() override;
	virtual int32 Buff_CalInitDamage_Implementation(int32 InDamage) override;
	// 仅仅用作在界面的显示，后续可能需要改名为 AddBuffOnWidget
	virtual void AddBuff_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor, bool Permanent) override;
	virtual void RemoveBuff_Implementation(FGameplayTag Tag, bool OnlySelf) override;
	virtual float GetShortSightResistancePercent_Implementation() override;
	virtual float GetSlowDownResistancePercent_Implementation() override;
#pragma endregion

	UFUNCTION(BlueprintCallable, Category="Buff | BuffText")
	void AddBuffByTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category="Buff | BuffText")
	void ExpireBuff(FGameplayTag Tag);
	
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

