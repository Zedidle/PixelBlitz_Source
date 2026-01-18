// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/PXAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Pixel2DKit.h"
#include "Core/PXGameState.h"
#include "Fight/Components/StateComponent.h"
#include "Interfaces/Fight_Interface.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/PXGameplayStatics.h"

void UPXAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UPXAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UPXAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(), 0.f, GetMaxHP()));
	}
	
	if (Data.EvaluatedData.Attribute == GetEPAttribute())
	{
		SetEP(FMath::Clamp(GetEP(), 0.f, GetMaxEP()));
	}
	
}

void UPXAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	AActor* Owner = GetOwningActor();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)

	APawn* Pawn;
	if (Owner->Implements<UFight_Interface>())
	{
		Pawn = IFight_Interface::Execute_GetPawn(Owner);
	}
	else
	{
		Pawn = Cast<APawn>(Owner);
	}
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Pawn)	
	
	if(Attribute == GetHPAttribute())
	{
		int ChangedValue = NewValue - GetHP();

		if (ChangedValue < 0)
		{
			if (APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld()))
			{
				// 天气的统一影响
				ChangedValue = ChangedValue * (1 + GS->GetDamagePlusPercent());
			}
		}
		
		NewValue = FMath::Clamp(GetHP() + ChangedValue, 0.f, GetMaxHP());
		
		if (ChangedValue != 0.f)
		{
			EFloatingTextType ChangedType = ChangedValue > 0 ? EFloatingTextType::Health : EFloatingTextType::Damage;
			UCommonFuncLib::SpawnFloatingCombatText(
				ChangedType,
			FText::AsNumber(FMath::Abs(ChangedValue)),
			nullptr,
				Pawn->GetActorLocation(),
			FVector2D(0.8f, 0.8f));
		}

		// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor(255,48,16),
		// 	FString::Printf( TEXT("UPXAttributeSet::PreAttributeChange HP: %f, %f"), NewValue, GetHP() ));
	}
	if(Attribute == GetEPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEP());
		int ChangedValue = NewValue - GetEP();

		if (ChangedValue > 5.f)
		{
			UCommonFuncLib::SpawnFloatingCombatText(
				EFloatingTextType::Energy,
			FText::AsNumber(FMath::Abs(ChangedValue)),
			nullptr,
				 Pawn->GetActorLocation(),
			FVector2D(0.8f, 0.8f));
		}
	}
	
}

void UPXAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	OnPXAttributeChange.Broadcast(Attribute, OldValue, NewValue);
	if (Attribute == GetHPAttribute())
	{
		AActor* Actor = GetOwningActor();
		if (Actor && Actor->Implements<UFight_Interface>())
		{
			if (APawn* Pawn = IFight_Interface::Execute_GetPawn(Actor))
			{
				if (UStateComponent* StateComponent = Pawn->GetComponentByClass<UStateComponent>())
				{
					StateComponent->OnHPChanged.Broadcast(OldValue, NewValue);
				}
			}
		}
	}
}

bool UPXAttributeSet::GetAttrCurrentValueByName(FName AttrName, float& OutValue)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	OutValue = AttrData->GetCurrentValue();

	return true;
}

bool UPXAttributeSet::SetAttrCurrentValueByName(FName AttrName, float Value)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	AttrData->SetCurrentValue(Value);

	return true;	
}

bool UPXAttributeSet::GetAttrBaseValueByName(FName AttrName, float& OutValue)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	OutValue = AttrData->GetBaseValue();
	
	return true;
}

bool UPXAttributeSet::SetAttrBaseValueByName(FName AttrName, float Value)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	AttrData->SetBaseValue(Value);
	
	return true;	
}


FStructProperty* UPXAttributeSet::GetAttrStructProperty(FName AttrName) const
{
	const UClass* SelfCls = GetClass();
	if (SelfCls == nullptr) return nullptr;

	const FName PropertyName = FName(AttrName);
	FProperty* Property = SelfCls->FindPropertyByName(PropertyName);
	if (Property == nullptr) return nullptr;
	if (!Property->IsA(FStructProperty::StaticClass())) return nullptr;

	FStructProperty* StructProperty = CastField<FStructProperty>(Property);
	return StructProperty;
}

FGameplayAttributeData* UPXAttributeSet::GetGameplayAttributeData(FName AttrName)
{
	FStructProperty* StructProperty = GetAttrStructProperty(AttrName);
	if (!StructProperty) return nullptr;
	void* ValuePtr = StructProperty->ContainerPtrToValuePtr<void>(this);
	if (!ValuePtr) return nullptr;

	if (StructProperty->Struct != TBaseStructure<FGameplayAttributeData>::Get()) return nullptr;
	FGameplayAttributeData* GameplayAttributeData = static_cast<FGameplayAttributeData*>(ValuePtr);
	return GameplayAttributeData;
}

void UPXAttributeSet::OnRep_HP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, HP, OldValue);
}

void UPXAttributeSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, MaxHP, OldValue);
}

void UPXAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, EP, OldValue);
}

void UPXAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, MaxEP, OldValue);
}
