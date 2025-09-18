// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Basic/PXGameState.h"
#include "GAS/PXGameplayEffect.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "opensubdiv/far/error.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/LocalizationFuncLib.h"
#include "Utilitys/PXGameplayStatics.h"

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
	SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags); // 添加技能专属标签
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("AbilityCD")), // 自定义 SetByCaller 标签
		GetCooldownDuration()
	);

	if (UPXGameplayEffect* PXGE = Cast<UPXGameplayEffect>(CooldownGE))
	{
		if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
		{
			Spec->AppendDynamicAssetTags(PXGE->AssetTags);
			Spec->DynamicGrantedTags.AppendTags(PXGE->GrantedTags);
		}
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
		UCommonFuncLib::SpawnCenterTipLocalized(
			FLocalizedTableData("Basic/Tips", "EPNotEnough"),
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
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // 清空临时容器（避免 CDO 残留旧标签）
    
	// 合并父类标签与技能专属标签
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags) MutableTags->AppendTags(*ParentTags);
	MutableTags->AppendTags(CooldownTags); 
	return MutableTags;
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

