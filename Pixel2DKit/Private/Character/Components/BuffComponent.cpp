// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/BuffComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagsManager.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
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
#include "Subsystems/TimerSubsystemFuncLib.h"
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

	TArray<FGameplayTag> RemovedTags;
	
	for (auto& ele : Tag2BuffEffect_Attack)
	{
		if (CurTime > ele.Value.EffectedEndTime)
		{
			RemovedTags.Add(ele.Key);
		}
	}

	for (auto& ele : Tag2BuffEffect_Speed)
	{
		if (CurTime > ele.Value.EffectedEndTime)
		{
			RemovedTags.Add(ele.Key);
		}
	}

	for (auto& ele : Tag2BuffEffect_Sight)
	{
		if (CurTime > ele.Value.EffectedEndTime)
		{
			RemovedTags.Add(ele.Key);
		}
	}

	for (auto& ele : RemovedTags)
	{
		RemoveBuff_EffectAll(ele);
		if (BuffStateWidget)
		{
			BuffStateWidget->BuffExpire(ele);
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
		UAbilitySystemComponent* AbilitySystemComponent = ASI->GetAbilitySystemComponent();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilitySystemComponent);
		AbilitySystemComponent->OnGameplayEffectAppliedDelegateToTarget.AddUObject(this, &UBuffComponent::OnGameplayEffectApplied);
		AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UBuffComponent::OnGameplayEffectRemoved);
	}

	TimerName_CheckBuffEnd = FName("BuffComponent" + FGuid::NewGuid().ToString());
	UTimerSubsystemFuncLib::SetDelayLoopSafe(GetWorld(), TimerName_CheckBuffEnd, this, &UBuffComponent::CheckBuffExpire, 0.1, -1);
}

void UBuffComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UTimerSubsystemFuncLib::CancelDelay(GetWorld(), TimerName_CheckBuffEnd);
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
	return Tag2BuffEffect_Sight.Contains(Tag)
		|| Tag2BuffEffect_Speed.Contains(Tag)
		|| Tag2BuffEffect_Attack.Contains(Tag);
}

void UBuffComponent::RemoveBuff_Attack(FGameplayTag Tag)
{
	if (!Tag2BuffEffect_Attack.Contains(Tag)) return;

	FBuffEffect Effect = Tag2BuffEffect_Attack[Tag];
	EffectedPercent_Attack -= Effect.EffectedPercent;
	EffectedValue_Attack -= Effect.EffectedValue;

	Tag2BuffEffect_Attack.Remove(Tag);

	if (Owner && Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Attack(Owner);
	}
	
}

void UBuffComponent::RemoveBuff_Sight(FGameplayTag Tag)
{
	if (!Tag2BuffEffect_Sight.Contains(Tag)) return;
	
	FBuffEffect Effect = Tag2BuffEffect_Sight[Tag];
	EffectedPercent_Sight -= Effect.EffectedPercent;
	EffectedValue_Sight -= Effect.EffectedValue;

	Tag2BuffEffect_Sight.Remove(Tag);

	if (Owner && Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Sight(Owner);
	}
}

void UBuffComponent::RemoveBuff_Speed(FGameplayTag Tag)
{
	if (!Tag2BuffEffect_Speed.Contains(Tag)) return;
	
	FBuffEffect Effect = Tag2BuffEffect_Speed[Tag];
	EffectedPercent_Speed -= Effect.EffectedPercent;
	EffectedValue_Speed -= Effect.EffectedValue;

	Tag2BuffEffect_Speed.Remove(Tag);

	if (Owner && Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Speed(Owner);
	}
	
}

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
		if (TagString.Contains("Buff"))
		{
			// Execute_RemoveBuff(this, Tag, true);
		}
		else if (TagString.Contains(".CD"))
		{
			FString AbilityBuffTag, _;
			TagString.Split(".CD", &AbilityBuffTag, &_);
			if (!AbilityBuffTag.IsEmpty())
			{
				// Execute_RemoveBuff(this, TAG(*AbilityBuffTag), true);
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

void UBuffComponent::BuffEffect_Speed_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime)
{
	if (!Tag.IsValid()) return;
	
	RemoveBuff_Speed(Tag);
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	if (!Owner->Implements<UBuff_Interface>()) return;
	
	float SlowDownResistancePercent = Execute_GetSlowDownResistancePercent(Owner);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());

	if (Percent < 0)
	{
		Percent = (1 - SlowDownResistancePercent) * Percent;
	}

	if (Value < 0)
	{
		Value = (1 - SlowDownResistancePercent) * Value;
	}

	EffectedPercent_Speed += Percent;
	EffectedValue_Speed += Value;
	Tag2BuffEffect_Speed.Add(Tag, FBuffEffect(Percent, Value, SustainTime + Now));
	
	Execute_BuffUpdate_Speed(Owner);

	FVector Location = Owner->GetActorLocation();
	if (Percent < 0 || Value < 0)
	{
		UCommonFuncLib::SpawnFloatingText( LOCTEXT("BUFF_SLOWDOWN", "减速"),
				Location + FVector(20, 0, 20),
				FLinearColor::Gray);
	}
	else
	{
		UCommonFuncLib::SpawnFloatingText( LOCTEXT("BUFF_SPEEDUP", "加速"),
				Location + FVector(-30, 0, 20),
				FLinearColor::White);
	}


	if (Tag2Niagara.Contains(Tag))
	{
		Tag2Niagara[Tag]->DestroyComponent();
		Tag2Niagara.Remove(Tag);
	}
	
 	if (Percent > 0 && Value > 0)
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
			Tag2Niagara.Add(Tag, NiagaraComponent);
		}
	}
}


void UBuffComponent::BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime)
{
	if (!Tag.IsValid()) return;
	
	RemoveBuff_Attack(Tag);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());

	EffectedPercent_Attack += Percent;
	EffectedValue_Attack += Value;
	
	Tag2BuffEffect_Attack.Add(Tag, FBuffEffect(Percent, Value, SustainTime + Now));

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	if (Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Attack(Owner);
	}
}
void UBuffComponent::BuffEffect_Sight_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime)
{
	if (!Tag.IsValid()) return;
	
	RemoveBuff_Sight(Tag);
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	if (!Owner->Implements<UBuff_Interface>()) return;

	float ShortSightResistancePercent = Execute_GetShortSightResistancePercent(Owner);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());

	if (Percent < 0)
	{
		Percent = (1 - ShortSightResistancePercent) * Percent;
	}

	if (Value < 0)
	{
		Value = (1 - ShortSightResistancePercent) * Value;
	}

	EffectedPercent_Sight += Percent;
	EffectedValue_Sight += Value;
	Tag2BuffEffect_Sight.Add(Tag, FBuffEffect(Percent, Value, SustainTime + Now));
	
	Execute_BuffUpdate_Sight(Owner);

	FVector Location = Owner->GetActorLocation();
	if (Percent < 0 || Value < 0)
	{
		UCommonFuncLib::SpawnFloatingText(LOCTEXT("Buff_Myopia", "短视"),
			Location + FVector(10.0, 0, 20),
			FLinearColor::White,
			FVector2D(0.9, 0.9)
			);
	}
	else
	{
		UCommonFuncLib::SpawnFloatingText(LOCTEXT("Buff_Hyperopia", "远视"),
			Location + FVector(-50, 0, 20),
			FLinearColor::Gray);
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
					// Execute_RemoveBuffOnWidget(this, Tag, true);
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

void UBuffComponent::RemoveBuff_Implementation(const FGameplayTag& Tag, bool OnlySelf)
{
	if (!Tag.IsValid()) return;

	RemoveBuffOnWidget(Tag, OnlySelf);
	// RemoveAttributeEffectsByTag(Tag);
	
	if (OnlySelf)
	{
		RemoveAttributeEffectsByTag(Tag);
		// RemoveBuff_EffectAll(Tag);
	}
	else
	{
		FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
		TArray<FGameplayTag> Tags;
		TagContainer.GetGameplayTagArray(Tags);
	
		for (auto& T : Tags)
		{
			RemoveAttributeEffectsByTag(T);
			// RemoveBuff_EffectAll(T);
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
	
	// Execute_AddBuffOnWidget(this, Tag, Data->BuffName.ToString(), Data->Color, Data->Permanent);
}

void UBuffComponent::ExpireBuff(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;
	
	if (IsValid(BuffStateWidget))
	{
		BuffStateWidget->BuffExpire(Tag);
	}
}

void UBuffComponent::AddAttributeEffect(const FString& AttributeName, const FGameplayTag& Tag, const FBuffEffect& Effect)
{
	FAttributeEffectMap* AttributeEffectMap = AttributeEffects.Find(AttributeName);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AttributeEffectMap)
	
	UPXASComponent* ASC = Cast<UPXASComponent>( UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC)

	const UPXAttributeSet* AttributeSet = ASC->GetSet<UPXAttributeSet>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AttributeSet)

	auto& AttributeNames= Tag2AttributeNames.FindOrAdd(Tag);
	AttributeNames.Add(AttributeName);
	
	float PreValue = ASC->GetPXAttributeValueByName(AttributeName);
	float CurValue = PreValue *  (1 + Effect.EffectedPercent) + Effect.EffectedValue;
	
	ASC->SetPXAttributeValueByName(AttributeName, CurValue);
	AttributeEffectMap->Tag2BuffEffect.Add(Tag, Effect);
}

void UBuffComponent::RemoveAttributeEffect(const FString& AttributeName, const FGameplayTag& Tag)
{
	FAttributeEffectMap* AttributeEffectMap = AttributeEffects.Find(AttributeName);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AttributeEffectMap)

	UPXASComponent* ASC = Cast<UPXASComponent>( UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC)

	const FBuffEffect& Effect = AttributeEffectMap->Tag2BuffEffect[Tag];
	const UPXAttributeSet* AttributeSet = ASC->GetSet<UPXAttributeSet>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AttributeSet)
	
	// 可能要转入 FGameplayAttributeData 的 BaseValue 和 CurrentValue 的处理
	float PreValue = ASC->GetPXAttributeValueByName(AttributeName);
	float CurValue = (PreValue - Effect.EffectedValue) / (1 + Effect.EffectedPercent);
	
	ASC->SetPXAttributeValueByName(AttributeName, CurValue);
	AttributeEffectMap->Tag2BuffEffect.Remove(Tag);
	if (auto T = Tag2AttributeNames.Find(Tag))
	{
		T->Remove(AttributeName);
	}
}

void UBuffComponent::RemoveAttributeEffectsByTag(const FGameplayTag& Tag)
{
	auto Names = Tag2AttributeNames.FindRef(Tag);
	for (auto& Name : Names)
	{
		RemoveAttributeEffect(Name, Tag);
	}
}

#undef LOCTEXT_NAMESPACE
