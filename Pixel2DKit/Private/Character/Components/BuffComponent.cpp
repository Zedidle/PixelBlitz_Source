// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/BuffComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Character/BasePXCharacter.h"
#include "Core/PXGameInstance.h"
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
	
	for (auto& ele : Tag2BuffEndTime_Attack)
	{
		if (CurTime > ele.Value)
		{
			RemovedTags.Add(ele.Key);
		}
	}

	for (auto& ele : Tag2BuffEndTime_Speed)
	{
		if (CurTime > ele.Value)
		{
			RemovedTags.Add(ele.Key);
		}
	}

	for (auto& ele : Tag2BuffEndTime_Sight)
	{
		if (CurTime > ele.Value)
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
	Owner = Cast<ABasePXCharacter>(GetOwner());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner);
	
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

	FBuffValueEffect Effect = Tag2BuffEffect_Attack[Tag];
	EffectedPercent_Attack -= Effect.EffectedPercent;
	EffectedValue_Attack -= Effect.EffectedValue;

	Tag2BuffEffect_Attack.Remove(Tag);
	Tag2BuffEndTime_Attack.Remove(Tag);

	if (Owner && Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Attack(Owner);
	}
	
}

void UBuffComponent::RemoveBuff_Sight(FGameplayTag Tag)
{
	if (!Tag2BuffEffect_Sight.Contains(Tag)) return;
	
	FBuffValueEffect Effect = Tag2BuffEffect_Sight[Tag];
	EffectedPercent_Sight -= Effect.EffectedPercent;
	EffectedValue_Sight -= Effect.EffectedValue;

	Tag2BuffEffect_Sight.Remove(Tag);
	Tag2BuffEndTime_Sight.Remove(Tag);

	if (Owner && Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Sight(Owner);
	}
}

void UBuffComponent::RemoveBuff_Speed(FGameplayTag Tag)
{
	if (!Tag2BuffEffect_Speed.Contains(Tag)) return;
	
	FBuffValueEffect Effect = Tag2BuffEffect_Speed[Tag];
	EffectedPercent_Speed -= Effect.EffectedPercent;
	EffectedValue_Speed -= Effect.EffectedValue;

	Tag2BuffEffect_Speed.Remove(Tag);
	Tag2BuffEndTime_Speed.Remove(Tag);

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
			Execute_RemoveBuff(this, Tag, true);
		}
		else if (TagString.Contains(".CD"))
		{
			FString AbilityBuffTag, _;
			TagString.Split(".CD", &AbilityBuffTag, &_);
			if (!AbilityBuffTag.IsEmpty())
			{
				Execute_RemoveBuff(this, TAG(*AbilityBuffTag), true);
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
	Tag2BuffEndTime_Speed.Add( Tag, SustainTime + Now);

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
	Tag2BuffEffect_Speed.Add(Tag, FBuffValueEffect(Percent, Value));
	
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

void UBuffComponent::BuffUpdate_Speed_Implementation()
{
	IBuff_Interface::BuffUpdate_Speed_Implementation();
}

void UBuffComponent::BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime)
{
	if (!Tag.IsValid()) return;
	
	RemoveBuff_Attack(Tag);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	Tag2BuffEndTime_Attack.Add(Tag, SustainTime + Now);

	EffectedPercent_Attack += Percent;
	EffectedValue_Attack += Value;
	
	Tag2BuffEffect_Attack.Add(Tag, FBuffValueEffect(Percent, Value));

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	if (Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Attack(Owner);
	}
}

void UBuffComponent::BuffUpdate_Attack_Implementation()
{
	IBuff_Interface::BuffUpdate_Attack_Implementation();
}

void UBuffComponent::BuffEffect_Sight_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime)
{
	if (!Tag.IsValid()) return;
	
	RemoveBuff_Sight(Tag);
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	if (!Owner->Implements<UBuff_Interface>()) return;

	float ShortSightResistancePercent = Execute_GetShortSightResistancePercent(Owner);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	Tag2BuffEndTime_Sight.Add( Tag, SustainTime + Now);

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
	Tag2BuffEffect_Sight.Add(Tag, FBuffValueEffect(Percent, Value));
	
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

void UBuffComponent::BuffUpdate_Sight_Implementation()
{
	IBuff_Interface::BuffUpdate_Sight_Implementation();
}

int32 UBuffComponent::Buff_CalInitDamage_Implementation(int32 InDamage)
{
	return InDamage;
}

void UBuffComponent::AddBuffOnWidget_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor,
	bool Permanent)
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
					Execute_RemoveBuffOnWidget(this, Tag, true);
				}
			}
			
			BuffStateWidget->BuffIn(Tag, BuffName, TextColor);
		}
	}
}

void UBuffComponent::RemoveBuffOnWidget_Implementation(FGameplayTag Tag, bool OnlySelf)
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

void UBuffComponent::RemoveBuff_Implementation(FGameplayTag Tag, bool OnlySelf)
{
	if (!Tag.IsValid()) return;

	Execute_RemoveBuffOnWidget(this, Tag, OnlySelf);
	
	if (OnlySelf)
	{
		RemoveBuff_EffectAll(Tag);
	}
	else
	{
		FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
		TArray<FGameplayTag> Tags;
		TagContainer.GetGameplayTagArray(Tags);
	
		for (auto& T : Tags)
		{
			RemoveBuff_EffectAll(T);
		}
	}
}


float UBuffComponent::GetShortSightResistancePercent_Implementation()
{
	return IBuff_Interface::GetShortSightResistancePercent_Implementation();
}

float UBuffComponent::GetSlowDownResistancePercent_Implementation()
{
	return IBuff_Interface::GetSlowDownResistancePercent_Implementation();
}

void UBuffComponent::AddBuffByTag(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	UPXGameInstance* GameInstance = UPXGameplayStatics::GetGameInstance(Owner->GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	const FBuffOnWidget* Data = DataTableSubsystem->GetBuffOnWidgetDataByTag(Tag);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Data)
	
	Execute_AddBuffOnWidget(this, Tag, Data->BuffName.ToString(), Data->Color, Data->Permanent);
}

void UBuffComponent::ExpireBuff(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;
	
	if (IsValid(BuffStateWidget))
	{
		BuffStateWidget->BuffExpire(Tag);
	}
}

#undef LOCTEXT_NAMESPACE