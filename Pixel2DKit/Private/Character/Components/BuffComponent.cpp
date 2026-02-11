// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/BuffComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagsManager.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "NiagaraCullProxyComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Core/PXGameInstance.h"
#include "GameFramework/Character.h"
#include "GAS/PXASComponent.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "Utilitys/CommonFuncLib.h"
#include "UI/Buff/BuffStateWidget.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/PXGameplayStatics.h"

#define LOCTEXT_NAMESPACE "PX"


class UPXAttributeSet;
class UDataTableSubsystem;
// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UBuffComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

}

void UBuffComponent::InitData()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)

	UGameInstance* GameInstance = Owner->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);
	
	

	// 其它 Data 
	
}


void UBuffComponent::CheckBuffExpire()
{
	float CurTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());

	TMap<FGameplayTag, TArray<EPXAttribute>> RemovedTagsEffect;

	for (auto& ele : AttributeEffects)
	{
		FAttributeEffectData& EffectMap = ele.Value;

		for (auto& BuffEffect : EffectMap.Tag2BuffEffect)
		{
			if (CurTime > BuffEffect.Value.EffectedEndTime)
			{
				TArray<EPXAttribute>& Attributes = RemovedTagsEffect.FindOrAdd(BuffEffect.Key);
				Attributes.Add(ele.Key);
			}
		}
	}

	RemoveBuff(RemovedTagsEffect);

	if (BuffStateWidget)
	{
		for (auto& ele : RemovedTagsEffect)
		{
			if (Tag2AttributeEnums.Contains(ele.Key)) continue;
			BuffStateWidget->BuffExpire(ele.Key);
		}
	}
}

// Called when the game starts
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ACharacter>(GetOwner());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner->GetController())
	
	if (Owner->GetController()->IsLocalPlayerController())
	{
		InitData();
	
		if (BuffStateWidgetClass)
		{
			if (UWorld* World = GetWorld())
			{
				BuffStateWidget = Cast<UBuffStateWidget>(CreateWidget(World, BuffStateWidgetClass));
				BuffStateWidget->SetVisibility(ESlateVisibility::Hidden);
				BuffStateWidget->AddToViewport(1);
			}
		}
	}

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
	{
		CachedASC = Cast<UPXASComponent>(ASI->GetAbilitySystemComponent());
		CachedASC->OnGameplayEffectAppliedDelegateToTarget.AddUObject(this, &UBuffComponent::OnGameplayEffectApplied);
		CachedASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UBuffComponent::OnGameplayEffectRemoved);
	}

	TimerName_CheckBuffEnd = FName("BuffComponent" + FGuid::NewGuid().ToString());
	UTimerManagerFuncLib::SetDelayLoopSafe(GetWorld(), TimerName_CheckBuffEnd, this, &UBuffComponent::CheckBuffExpire, 0.1, -1);
}

void UBuffComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UTimerManagerFuncLib::CancelDelay(GetWorld(), TimerName_CheckBuffEnd);
	if (BuffStateWidget)
	{
		BuffStateWidget->RemoveFromParent();
	}
}


// Called every frame
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UBuffComponent::BuffExist(FGameplayTag Tag) const
{
	return Tag2AttributeEnums.Contains(Tag);
}

// void UBuffComponent::RemoveBuff_Attack(FGameplayTag Tag)
// {
// 	if (!Tag2BuffEffect_Attack.Contains(Tag)) return;
//
// 	FBuffEffect Effect = Tag2BuffEffect_Attack[Tag];
// 	EffectedPercent_Attack -= Effect.EffectedPercent;
// 	EffectedValue_Attack -= Effect.EffectedValue;
//
// 	Tag2BuffEffect_Attack.Remove(Tag);
//
// 	if (Owner && Owner->Implements<UBuff_Interface>())
// 	{
// 		Execute_BuffUpdate_Attack(Owner);
// 	}
// 	
// }
//
// void UBuffComponent::RemoveBuff_Sight(FGameplayTag Tag)
// {
// 	if (!Tag2BuffEffect_Sight.Contains(Tag)) return;
// 	
// 	FBuffEffect Effect = Tag2BuffEffect_Sight[Tag];
// 	EffectedPercent_Sight -= Effect.EffectedPercent;
// 	EffectedValue_Sight -= Effect.EffectedValue;
//
// 	Tag2BuffEffect_Sight.Remove(Tag);
//
// 	if (Owner && Owner->Implements<UBuff_Interface>())
// 	{
// 		Execute_BuffUpdate_Sight(Owner);
// 	}
// }
//
// void UBuffComponent::RemoveBuff_Speed(FGameplayTag Tag)
// {
// 	if (!Tag2BuffEffect_Speed.Contains(Tag)) return;
// 	
// 	FBuffEffect Effect = Tag2BuffEffect_Speed[Tag];
// 	EffectedPercent_Speed -= Effect.EffectedPercent;
// 	EffectedValue_Speed -= Effect.EffectedValue;
//
// 	Tag2BuffEffect_Speed.Remove(Tag);
//
// 	if (Owner && Owner->Implements<UBuff_Interface>())
// 	{
// 		Execute_BuffUpdate_Speed(Owner);
// 	}
// }

void UBuffComponent::SetBuffStateWidgetVisibility(ESlateVisibility InVisibility)
{
	if (BuffStateWidget)
	{
		BuffStateWidget->SetVisibility(InVisibility);
	}
}

void UBuffComponent::OnGameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                             const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	UPXGameInstance* GameInstance = UPXGameplayStatics::GetGameInstance(Owner->GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	const FGameplayTagContainer GameplayTagContainer = GameplayEffectSpec.GetDynamicAssetTags();
	TArray<FGameplayTag> Tags = GameplayTagContainer.GetGameplayTagArray();
	for (const FGameplayTag& Tag : Tags)
	{
		FString TagString = Tag.ToString();
		if (TagString.Contains(".CD"))
		{
			FString AbilityBuffTag, _;
			TagString.Split(".CD", &AbilityBuffTag, &_);
			if (!AbilityBuffTag.IsEmpty())
			{
				RemoveBuff(TAG(*AbilityBuffTag), true);
			}
		}
	}
}

void UBuffComponent::OnGameplayEffectRemoved(const FActiveGameplayEffect& GameplayEffect)
{
	const FGameplayTagContainer GameplayTagContainer = GameplayEffect.Spec.GetDynamicAssetTags();
	TArray<FGameplayTag> Tags = GameplayTagContainer.GetGameplayTagArray();
	for (const FGameplayTag& Tag : Tags)
	{
		FString TagString = Tag.ToString();
		if (TagString.Contains("Buff"))
		{
			AddBuffByTag(Tag);
		}
		else if (TagString.Contains(".CD"))
		{
			FString AbilityBuffTag, _;
			TagString.Split(".CD", &AbilityBuffTag, &_);
			if (!AbilityBuffTag.IsEmpty())
			{
				AddBuffByTag(TAG(*AbilityBuffTag));
			}
		}
	}
}

void UBuffComponent::RemoveBuff(const FGameplayTag& Tag, bool OnlySelf)
{
	if (!Tag.IsValid()) return;

	RemoveBuffOnWidget(Tag, OnlySelf);
	
	if (OnlySelf)
	{
		RemoveAttributeEffectsByTag(Tag);
	}
	else
	{
		FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
		TArray<FGameplayTag> Tags;
		TagContainer.GetGameplayTagArray(Tags);
	
		for (auto& T : Tags)
		{
			RemoveAttributeEffectsByTag(T);
		}
	}
}

void UBuffComponent::RemoveBuff(TMap<FGameplayTag, TArray<EPXAttribute>>& RemoveEffects)
{
	for (auto& Effect : RemoveEffects)
	{
		for (auto& AttributeName : Effect.Value)
		{
			RemoveAttributeEffect(AttributeName, Effect.Key);
		}
	}
}

void UBuffComponent::AddBuffOnWidget(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor, bool Permanent)
{
	if (!Tag.IsValid()) return;
	
	// 显示到界面
	if (IsValid(BuffStateWidget))
	{
		if (Permanent)
		{
			BuffStateWidget->BuffPermanent(Tag, BuffName, TextColor);
		}
		else
		{
			if (UBuffFloatingTextWidget* Widget = BuffStateWidget->Tag2Widget_In.FindRef(Tag))
			{
				if (Widget->BuffName != BuffName)
				{
					RemoveBuffOnWidget(Tag, Permanent);
				}
			}
			
			BuffStateWidget->BuffIn(Tag, BuffName, TextColor);
		}
	}
}

void UBuffComponent::RemoveBuffOnWidget(FGameplayTag Tag, bool OnlySelf)
{
	if (OnlySelf)
	{
		if (IsValid(BuffStateWidget))
		{
			BuffStateWidget->BuffOut(Tag);
		}
	}
	else
	{
		FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
		TArray<FGameplayTag> Tags;
		TagContainer.GetGameplayTagArray(Tags);
	
		for (auto& T : Tags)
		{
			if (IsValid(BuffStateWidget))
			{
				BuffStateWidget->BuffOut(T);
			}
		}
	}
}

void UBuffComponent::AddBuffByTag(FGameplayTag Tag, bool bNeedPermanent)
{
	if (!Tag.IsValid()) return;

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	UPXGameInstance* GameInstance = UPXGameplayStatics::GetGameInstance(Owner->GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	const FBuffOnWidget* Data = DataTableSubsystem->GetBuffOnWidgetDataByTag(Tag);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Data)

	if (bNeedPermanent && !Data->Permanent) return;
	
	AddBuffOnWidget(Tag, Data->BuffName.ToString(), Data->Color, Data->Permanent);
}

void UBuffComponent::ExpireBuff(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;
	
	if (IsValid(BuffStateWidget))
	{
		BuffStateWidget->BuffExpire(Tag);
	}
}

void UBuffComponent::AddAttributeEffect(const FGameplayTag& Tag, EPXAttribute Attribute, float Percent, float Value,
	float Duration)
{
	FAttributeEffect Effect = FAttributeEffect(Attribute, Percent, Value, Duration);
	AddAttributeEffect(Tag, Effect);
}

void UBuffComponent::AddAttributeEffect(const FGameplayTag& Tag, const FAttributeEffect& Effect)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	
	EPXAttribute AttributeEnum = Effect.EffectedAttribute;
	FString AttributeName = GetAttributeNameByEnum(AttributeEnum);
	if (AttributeName.IsEmpty()) return;
	
	if (AttributeName.StartsWith("Basic") || AttributeName.StartsWith("Cur"))
	{
		FAttributeEffectData& AttributeEffectData = AttributeEffects.FindOrAdd(AttributeEnum);
		AttributeEffectData.AddBuffEffect(Tag, Effect);

		TArray<EPXAttribute>& AttributeEnums = Tag2AttributeEnums.FindOrAdd(Tag);
		AttributeEnums.Add(AttributeEnum);
		
		UpdateAttribute(AttributeName);
	}
	else
	{
		float PreValue = CachedASC->GetAttributeValue(AttributeEnum);
		float CurValue = PreValue * (1 + Effect.EffectedPercent) + Effect.EffectedValue;
		
		CachedASC->SetAttributeValue(AttributeEnum, CurValue);
	}
}

void UBuffComponent::AddAttributeEffects(const FGameplayTag& Tag, const TArray<FAttributeEffect>& Effects)
{
	for (auto& Effect : Effects)
	{
		AddAttributeEffect(Tag, Effect);
	}
}

void UBuffComponent::RemoveAttributeEffect(EPXAttribute AttributeEnum, const FGameplayTag& Tag)
{
	FAttributeEffectData* AttributeEffectData = AttributeEffects.Find(AttributeEnum);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AttributeEffectData)
	
	AttributeEffectData->RemoveBuffEffect(Tag);
	if (auto Attributes = Tag2AttributeEnums.Find(Tag))
	{
		Attributes->Remove(AttributeEnum);
		if (Attributes->IsEmpty())
		{
			Tag2AttributeEnums.Remove(Tag);
		}
	}

	UpdateAttribute( AttributeEnum );
}

void UBuffComponent::RemoveAttributeEffectsByTag(const FGameplayTag& Tag)
{
	if (TArray<EPXAttribute>* AttributeEnumsPtr = Tag2AttributeEnums.Find(Tag))
	{
		TArray<EPXAttribute>& AttributeEnums = *AttributeEnumsPtr;
		for (int32 i = AttributeEnums.Num() - 1; i >= 0; --i)
		{
			RemoveAttributeEffect(AttributeEnums[i], Tag);
		}
	}
}

void UBuffComponent::UpdateAttribute(const FString& AttributeName)
{
	FString _, InitAttributeName;
	FString BasicAttributeName, CurAttributeName;
	if (AttributeName.Split("Basic", &_, &InitAttributeName) || AttributeName.Split("Cur", &_, &InitAttributeName))
	{
		BasicAttributeName = "Basic" + InitAttributeName;
		CurAttributeName = "Cur" + InitAttributeName;
	}

	EPXAttribute BasicAttributeEnum = AttributeNameToEnumMap.FindRef(BasicAttributeName);
	EPXAttribute CurAttributeEnum = AttributeNameToEnumMap.FindRef(CurAttributeName);
	FAttributeEffectData BasicAttributeEffectData = AttributeEffects.FindRef(BasicAttributeEnum);
	FAttributeEffectData CurAttributeEffectData = AttributeEffects.FindRef(CurAttributeEnum);
	
	float BasicAttributeValue = CachedASC->GetAttributeValue(BasicAttributeName);
	float BasicEffectedPercent = BasicAttributeEffectData.Percent;
	float BasicEffectedValue = BasicAttributeEffectData.Value;
	float CurEffectedPercent = CurAttributeEffectData.Percent;
	float CurEffectedValue = CurAttributeEffectData.Value;
		
	float CurAttributeValue = (BasicAttributeValue *  (1 + BasicEffectedPercent) + BasicEffectedValue) * (1 + CurEffectedPercent) + CurEffectedValue;
	CachedASC->SetAttributeValue(CurAttributeEnum, CurAttributeValue);

	if (CurAttributeValue < BasicAttributeValue * 1.05 && CurAttributeValue > BasicAttributeValue * 0.95)
	{
		RemoveBuffNS(CurAttributeEnum, true);
		RemoveBuffNS(CurAttributeEnum, false);
	}
	else if (CurAttributeValue > BasicAttributeValue * 1.05)
	{
		RemoveBuffNS(CurAttributeEnum, false);
		AddBuffNS(CurAttributeEnum);
	}
	else if (CurAttributeValue < BasicAttributeValue * 0.95)
	{
		RemoveBuffNS(CurAttributeEnum, true);
		AddBuffNS(CurAttributeEnum, false);
	}
}

void UBuffComponent::UpdateAttribute(EPXAttribute AttributeEnum)
{
	UpdateAttribute(GetAttributeNameByEnum(AttributeEnum));
}

FString UBuffComponent::GetAttributeNameByEnum(EPXAttribute AttributeName) const
{
	for (auto& V : AttributeNameToEnumMap)
	{
		if (V.Value == AttributeName) return V.Key;
	}
	return FString();
}

void UBuffComponent::AddBuffNS(EPXAttribute AttributeEnum, bool bPlus)
{
	if (bPlus)
	{
		if (AttributeEnum == EPXAttribute::CurSpeed)
		{
			const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

			const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)

			if (ResourceDataAsset && ResourceDataAsset->NS_SpeedUP.LoadSynchronous())
			{
				UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
					ResourceDataAsset->NS_SpeedUP.LoadSynchronous(),
					Owner->GetRootComponent(),
					FName(""),
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::Type::KeepRelativeOffset,
					false,
					true,
					ENCPoolMethod::None,
					true
				);
				// 后续看如何补充管理
				AttributeEnum2Niagara.Add(AttributeEnum, NiagaraComponent);
			}
		}
	}
	else
	{
		
	}
}

void UBuffComponent::RemoveBuffNS(EPXAttribute AttributeEnum, bool bPlus)
{
	TObjectPtr<UNiagaraComponent> NiagaraPtr = AttributeEnum2Niagara.FindRef(AttributeEnum);
	if (UNiagaraComponent* Niagara = NiagaraPtr.Get())  // FindRef返回值，不是指针
	{
		Niagara->DestroyComponent();
		AttributeEnum2Niagara.Remove(AttributeEnum);
	}
}

#undef LOCTEXT_NAMESPACE
