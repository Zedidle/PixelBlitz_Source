// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/BuffComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "NiagaraCommon.h"
#include "Blueprint/UserWidget.h"
#include "Character/BasePXCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/CommonFuncLib.h"
#include "UI/Buff/BuffStateWidget.h"

// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UBuffComponent::InitData()
{
	const UDataTableSettings* Settings = GetDefault<UDataTableSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings);
	
	UDataTable* DataTable = Settings->GetBuffOnWidgetData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable);

	TArray<FBuffOnWidget*> Rows;
	DataTable->GetAllRows<FBuffOnWidget>(TEXT("GetBuffRownameByTag ALL"), Rows);
	if (Rows.Num() >= 0)
	{
		for (auto* Row : Rows)
		{
			if (Row && Row->Tag.IsValid())
			{
				Tag2BuffOnWidgetData.Add(Row->Tag, *Row);
			}
		}
	}


	// 其它 Data 
	
}


// Called when the game starts
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	InitData();
	
	if (BuffStateWidgetClass)
	{
		if (UWorld* World = GetWorld())
		{
			BuffStateWidget = Cast<UBuffStateWidget>(CreateWidget(World, BuffStateWidgetClass));
			BuffStateWidget->AddToViewport(100);
		}

	}

	ABasePXCharacter* Owner = Cast<ABasePXCharacter>(GetOwner());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner);

	UAbilitySystemComponent* AbilitySystemComponent = Owner->GetAbilitySystemComponent();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilitySystemComponent);
	AbilitySystemComponent->OnGameplayEffectAppliedDelegateToTarget.AddUObject(this, &UBuffComponent::OnGameplayEffectApplied);
	AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UBuffComponent::OnGameplayEffectRemoved);
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
	AActor* Owner = GetOwner();
	if (Owner && Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Speed(Owner);
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
	AActor* Owner = GetOwner();
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
	AActor* Owner = GetOwner();
	if (Owner && Owner->Implements<UBuff_Interface>())
	{
		Execute_BuffUpdate_Sight(Owner);
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
	const FGameplayTagContainer GameplayTagContainer = GameplayEffectSpec.GetDynamicAssetTags();
	TArray<FGameplayTag> Tags = GameplayTagContainer.GetGameplayTagArray();
	for (const FGameplayTag& Tag : Tags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("AbilityCD")) || 
			Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Buff")))
		{
			if(Tag2BuffOnWidgetData.Contains(Tag))
			{
				RemoveBuff(Tag);
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
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("AbilityCD")) || 
			Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Buff")))
		{
			AddBuffByTag(Tag);
		}
	}
}

void UBuffComponent::BuffEffect_Speed_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime)
{
	IBuff_Interface::BuffEffect_Speed_Implementation(Tag, Percent, Value, SustainTime);

	RemoveBuff_Speed(Tag);

	AActor* Owner = GetOwner();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	if (!Owner->Implements<UBuff_Interface>()) return;
	
	float SlowDownResistancePercent = IBuff_Interface::Execute_GetSlowDownResistancePercent(Owner);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	Tag2BuffEndTime_Speed.Add( Tag, SustainTime + Now);

	if (Tag2BuffEffect_Speed.Contains(Tag)) return;

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
		UCommonFuncLib::SpawnFloatingText(
		FText::FromString(ULocalizationFuncLib::GetBuffText("Buff_SpeedUp")),
			Location + FVector(-30, 0, 20),
			FLinearColor::White);
	}
	else
	{
		UCommonFuncLib::SpawnFloatingText(
		FText::FromString(ULocalizationFuncLib::GetBuffText("Buff_SlowDown")),
			Location + FVector(20, 0, 20),
			FLinearColor::Gray);
	}
}

void UBuffComponent::BuffUpdate_Speed_Implementation()
{
	IBuff_Interface::BuffUpdate_Speed_Implementation();
}

void UBuffComponent::BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime)
{
	IBuff_Interface::BuffEffect_Attack_Implementation(Tag, Percent, Value, SustainTime);

	RemoveBuff_Attack(Tag);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	Tag2BuffEndTime_Attack.Add(Tag, SustainTime + Now);

	if (Tag2BuffEffect_Attack.Contains(Tag)) return;

	EffectedPercent_Attack += Percent;
	EffectedValue_Attack += Value;

	Tag2BuffEffect_Attack.Add(Tag, FBuffValueEffect(Percent, Value));

	AActor* Owner = GetOwner();
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
	IBuff_Interface::BuffEffect_Sight_Implementation(Tag, Percent, Value, SustainTime);
	
	RemoveBuff_Sight(Tag);
	
	AActor* Owner = GetOwner();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	if (!Owner->Implements<UBuff_Interface>()) return;

	float ShortSightResistancePercent = IBuff_Interface::Execute_GetShortSightResistancePercent(Owner);
	float Now = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	Tag2BuffEndTime_Sight.Add( Tag, SustainTime + Now);

	if (Tag2BuffEndTime_Sight.Contains(Tag)) return;

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
		UCommonFuncLib::SpawnFloatingText(
		FText::FromString(ULocalizationFuncLib::GetBuffText("Buff_Myopia")),
			Location + FVector(10.0, 0, 20),
			FLinearColor::White,
			FVector2D(0.9, 0.9)
			);
	}
	else
	{
		UCommonFuncLib::SpawnFloatingText(
		FText::FromString(ULocalizationFuncLib::GetBuffText("Buff_Hyperopia")),
			Location + FVector(-50, 0, 20),
			FLinearColor::Gray);
	}
}

void UBuffComponent::BuffUpdate_Sight_Implementation()
{
	IBuff_Interface::BuffUpdate_Sight_Implementation();
}

int32 UBuffComponent::Buff_CalDamage_Implementation(int32 InDamage)
{
	return IBuff_Interface::Buff_CalDamage_Implementation(InDamage);
}

void UBuffComponent::AddBuff_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor,
	bool Permanent)
{
	IBuff_Interface::AddBuff_Implementation(Tag, BuffName, TextColor, Permanent);
	if (IsValid(BuffStateWidget))
	{
		if (Permanent)
		{
			BuffStateWidget->BuffPermanent(Tag, BuffName, TextColor);
		}
		else
		{
			BuffStateWidget->BuffIn(Tag, BuffName, TextColor);
		}
	}
	
}

void UBuffComponent::RemoveBuff_Implementation(FGameplayTag Tag, bool OnlySelf)
{
	IBuff_Interface::RemoveBuff_Implementation(Tag, OnlySelf);

	if (OnlySelf)
	{
		if (IsValid(BuffStateWidget))
		{
			BuffStateWidget->BuffOut(Tag);
		}

		RemoveBuff_EffectAll(Tag);
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
	FName BuffName = GetBuffRownameByTag(Tag);
	if(Tag2BuffOnWidgetData.Contains(Tag))
	{
		FBuffOnWidget Data = Tag2BuffOnWidgetData[Tag];
		IBuff_Interface::Execute_AddBuff(this, Tag, ULocalizationFuncLib::GetBuffText(BuffName), Data.Color, Data.Permanent);
	}
}

