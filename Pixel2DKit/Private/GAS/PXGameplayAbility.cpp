// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/Components/AbilityComponent.h"
#include "Core/PXGameState.h"
#include "GAS/PXGameplayEffect.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/PXGameplayStatics.h"

#define LOCTEXT_NAMESPACE "PXGameplayAbility"

class UPXGameplayEffect;

float UPXGameplayAbility::GetCooldownDuration_Implementation() const
{
	return CooldownDuration.GetValueAtLevel(GetAbilityLevel());
}

float UPXGameplayAbility::GetCostEP_Implementation() const
{
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Cost.EP");
	if (Cost.Contains(Tag))
	{
		APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld());
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GS, 0);
		
		return Cost[Tag].GetValueAtLevel(GetAbilityLevel()) * (1 + GS->GetEPConsumePlusPercent());
	}

	return 0;
}

void UPXGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CooldownGE);
	
	// 创建 GE 规范（Spec）
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
    
	// 动态注入参数
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Spec);
	
	Spec->DynamicGrantedTags.AppendTags(CooldownTags);
	Spec->AppendDynamicAssetTags(CooldownTags);
	Spec->SetSetByCallerMagnitude(TAG("AbilityCD"), GetCooldownDuration());

	if (UPXGameplayEffect* PXGE = Cast<UPXGameplayEffect>(CooldownGE))
	{
		Spec->AppendDynamicAssetTags(PXGE->AssetTags);
		Spec->DynamicGrantedTags.AppendTags(PXGE->GrantedTags);
	}

	// 应用至所有者
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}



bool UPXGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	check(AbilitySystemComponent != nullptr);

	const UPXAttributeSet* Set = AbilitySystemComponent->GetSet<UPXAttributeSet>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Set, true)
	if (GetCostEP() > Set->GetEP())
	{
		UCommonFuncLib::SpawnCenterTip( LOCTEXT("EPNotEnough", "体力不足！"),
			FLinearColor::White, FVector2D(0, 300)
		);
		return false;
	}
	
	return true;
}

void UPXGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CostGE)
	
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec)
	{
		Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Cost.EP")), -GetCostEP());
	}

	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

}

const FGameplayTagContainer* UPXGameplayAbility::GetCooldownTags() const
{
	TempCooldownTags.Reset();

	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags) TempCooldownTags.AppendTags(*ParentTags);
	TempCooldownTags.AppendTags(CooldownTags);
	
	return &TempCooldownTags;
}

APawn* UPXGameplayAbility::GetPawn() const
{
	AActor* Actor = GetAvatarActorFromActorInfo();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Actor, nullptr)

	if (Actor->Implements<UFight_Interface>())
	{
		return IFight_Interface::Execute_GetPawn(Actor);
	}
	return nullptr;
}

void UPXGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{			
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	if (TriggerEventData && bHasBlueprintActivateFromEvent)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbilityFromEvent(*TriggerEventData);
	}
	else if (bHasBlueprintActivate)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbility();
	}
	
	APawn* Pawn = GetPawn();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Pawn)
	
	if (UAbilityComponent* AbilityComponent = Pawn->GetComponentByClass<UAbilityComponent>())
	{
		const FGameplayTagContainer& Tags = GetAssetTags();
		const TArray<FGameplayTag>& TagArray = Tags.GetGameplayTagArray();
		for (const FGameplayTag& Tag : TagArray)
		{
			AbilityComponent->ApplyAttributeEffects(Tag);
		}
	}
	
	// 4. 【第四步】确保结束瞬时技能
	// 因为你的技能逻辑是瞬时完成的，所以必须调用 EndAbility
	// 参数根据技能是正常结束(false)还是被取消(true)来设置
	constexpr bool bReplicateEndAbility = true;
	constexpr bool bWasCancelled = false; // 假设是正常结束
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

#undef LOCTEXT_NAMESPACE
