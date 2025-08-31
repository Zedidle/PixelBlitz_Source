// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "UI/QTE/ArrowLineWidget.h"
#include "UI/QTE/KeyPressCountDownWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "AbilityComponent.generated.h"


class ABasePXCharacter;
class ABaseEnemy;

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
	UDataTable* AbilityDataTable;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKeyPressCountDownWidget> KeyPressCountdownWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UArrowLineWidget> ArrowLineWidgetClass;

	
	
	UPROPERTY(BlueprintReadOnly)
	AActor* HurtMaker;
	UPROPERTY(BlueprintReadOnly)
	int AcceptDamage;
	
	// 选择了的技能列表
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> ChoicedAbilityIndexes;
	
	// 生效的技能列表（同一系列的技能，高等级会覆盖低等级的）
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> TakeEffectAbilityIndexes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> TempTestAbilityIndexes;

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer CreateGameplayTagContainer(FName TagName, bool WithChildren = false);

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UAbilitySystemComponent* CachedASC;


#pragma region InputAction
	UPROPERTY(EditDefaultsOnly)
	UInputAction* Action_Jump;
	
	
#pragma	endregion
	
	
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
	bool CanLearnAbility(const FName& RowNameIndex, const FAbility& Ability);

	UFUNCTION(BlueprintCallable)
	void OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage);



	UFUNCTION(BlueprintCosmetic)
	void CreateQTE();
	

	UFUNCTION(BlueprintCallable)
	void OnKeyPressed(UInputAction* InputAction, bool& Keep);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnHurtInstigatorDead(ABaseEnemy* DeadEnemy);

	UFUNCTION()
	void ListHurtInstigatorDead();

	UFUNCTION()
	FGameplayAbilitySpecHandle GetGameplayAbilityWithTag(const FGameplayTag& Tag);
	

};

