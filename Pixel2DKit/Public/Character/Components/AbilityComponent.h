// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "Components/ActorComponent.h"
#include "Utilitys/PXCustomStruct.h"
#include "AbilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UDataTable* AbilityDataTable;

	UPROPERTY()
	ABasePXCharacter* PXCharacter;
	
public:	
	// Sets default values for this component's properties
	UAbilityComponent();

	// 选择了的技能列表
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> ChoicedAbilityIndexes;
	
	// 生效的技能列表（同一系列的技能，高等级会覆盖低等级的）
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> TakeEffectAbilityIndexes;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> TempTestAbilityIndexes;

	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, float> EffectGameplayTag;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void LearnAbility(FName AbilityIndex);
	
	UFUNCTION(BlueprintCallable)
	void LoadAbility();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasChoiced(FName AbilityIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanLearnAbiliy(const FName& RowNameIndex, const FAbility& Ability);
	
};
