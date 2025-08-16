// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Buff_Interface.h"
#include "UI/Buff/BuffStateWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UBuffComponent : public UActorComponent, public IBuff_Interface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBuffStateWidget> BuffStateWidgetClass;
	UPROPERTY()
	UBuffStateWidget* BuffStateWidget = nullptr;
	
	float EffectedValue_Speed;
	float EffectedPercent_Speed;
	TMap<FGameplayTag, FBuffValueEffect> Tag2BuffEffect_Speed;
	TMap<FGameplayTag, float> Tag2BuffEndTime_Speed;

	float EffectedValue_Attack;
	float EffectedPercent_Attack;
	TMap<FGameplayTag, FBuffValueEffect> Tag2BuffEffect_Attack;
	TMap<FGameplayTag, float> Tag2BuffEndTime_Attack;

	float EffectedValue_Sight;
	float EffectedPercent_Sight;
	TMap<FGameplayTag, FBuffValueEffect> Tag2BuffEffect_Sight;
	TMap<FGameplayTag, float> Tag2BuffEndTime_Sight;
	
	
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
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

	
	
#pragma region IBuff_Interface
	virtual void BuffEffect_Speed_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime) override;
	virtual void BuffUpdate_Speed_Implementation() override;
	virtual void BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime) override;
	virtual void BuffUpdate_Attack_Implementation() override;
	virtual void BuffEffect_Sight_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime) override;
	virtual void BuffUpdate_Sight_Implementation() override;
	virtual int32 Buff_CalDamage_Implementation(int32 InDamage) override;
	virtual void AddBuff_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor, bool Permanent) override;
	virtual void RemoveBuff_Implementation(FGameplayTag Tag) override;
	virtual float GetShortSightResistancePercent_Implementation() override;
	virtual float GetSlowDownResistancePercent_Implementation() override;
#pragma endregion
		
};

inline void UBuffComponent::RemoveBuff_EffectAll(FGameplayTag Tag)
{
	RemoveBuff_Attack(Tag);
	RemoveBuff_Sight(Tag);
	RemoveBuff_Speed(Tag);
}

