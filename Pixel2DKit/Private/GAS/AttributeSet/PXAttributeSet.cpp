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
	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;
	
	if (Attribute == GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(), 0.f, GetCurMaxHP()));
	}
	
	if (Attribute == GetEPAttribute())
	{
		SetEP(FMath::Clamp(GetEP(), 0.f, GetCurMaxEP()));
	}
}

void UPXAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if(Attribute == GetHPAttribute())
	{
		int ChangedValue = NewValue - GetHP();

		if (ChangedValue < 0)
		{
			if (APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld()))
			{
				// 天气的统一影响
				ChangedValue *= 1 + GS->GetDamagePlusPercent();
			}
		}
		
		NewValue = FMath::Clamp(GetHP() + ChangedValue, 0.0f, GetCurMaxHP());
	}
	
	if(Attribute == GetEPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetCurMaxEP());
	}
}

void UPXAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	OnPXAttributeChange.Broadcast(Attribute, OldValue, NewValue);

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
	
	int ChangedValue = NewValue - OldValue;

	if (Attribute == GetHPAttribute())
	{
		if (UStateComponent* StateComponent = Pawn->GetComponentByClass<UStateComponent>())
		{
			StateComponent->OnHPChanged.Broadcast(OldValue, NewValue);
		}
		if (ChangedValue != 0.f)
		{
			EFloatingTextType ChangedType = ChangedValue > 0 ? EFloatingTextType::Heal : EFloatingTextType::Damage;
			UCommonFuncLib::SpawnFloatingCombatText(
				ChangedType,
			FText::AsNumber(FMath::Abs(ChangedValue)),
			nullptr,
				Pawn->GetActorLocation(),
			FVector2D(0.8f, 0.8f));
		}
	}

	if(Attribute == GetEPAttribute())
	{
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

FGameplayAttribute UPXAttributeSet::GetAttributeByName(const FString& AttributeName)
{
	FName NameKey = FName(*AttributeName);
	TMap<FName, FGameplayAttribute>& AttributeMap = GetAttributeMap();
    
	if (AttributeMap.Contains(NameKey))
	{
		return AttributeMap[NameKey];
	}
    
	return FGameplayAttribute();
}

TMap<FName, FGameplayAttribute>& UPXAttributeSet::GetAttributeMap()
{
	InitializeAttributeMap();
	return GetAttributeMapInstance();
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

void UPXAttributeSet::InitializeAttributeMap()
{
	TMap<FName, FGameplayAttribute>& AttributeMap = GetAttributeMapInstance();
	
	if (AttributeMap.IsEmpty())
	{
		// 生命值
		AttributeMap.Add("HP", GetHPAttribute());
		AttributeMap.Add("BasicMaxHP", GetBasicMaxHPAttribute());
		AttributeMap.Add("CurMaxHP", GetCurMaxHPAttribute());
		// 每五秒恢复生命值
		AttributeMap.Add("BasicHPRecoverValue", GetBasicHPRecoverValueAttribute());
		AttributeMap.Add("CurHPRecoverValue", GetCurHPRecoverValueAttribute());

		// 能量值
		AttributeMap.Add("EP", GetEPAttribute());
		AttributeMap.Add("BasicMaxEP", GetBasicMaxEPAttribute());
		AttributeMap.Add("CurMaxEP", GetCurMaxEPAttribute());
		// 能量每五秒能量值
		AttributeMap.Add("BasicEPRecoverValue", GetBasicEPRecoverValueAttribute());
		AttributeMap.Add("CurEPRecoverValue", GetCurEPRecoverValueAttribute());

		// 速度
		AttributeMap.Add("BasicSpeed", GetBasicSpeedAttribute());
		AttributeMap.Add("CurSpeed", GetCurSpeedAttribute());
		// 加速度
		AttributeMap.Add("BasicAcceleration", GetBasicAccelerationAttribute());
		AttributeMap.Add("CurAcceleration", GetCurAccelerationAttribute());
		// Dash速度
		AttributeMap.Add("BasicDashSpeed", GetBasicDashSpeedAttribute());
		AttributeMap.Add("CurDashSpeed", GetCurDashSpeedAttribute());
		
		// 视野
		AttributeMap.Add("BasicSight", GetBasicSightAttribute());
		AttributeMap.Add("CurSight", GetCurSightAttribute());
		// 视野削减抵抗值
		AttributeMap.Add("BasicSightReductionResist", GetBasicSightReductionResistAttribute());
		AttributeMap.Add("CurSightReductionResist", GetCurSightReductionResistAttribute());
		
		
		// 跳跃
		// 跳跃次数
		AttributeMap.Add("BasicMaxJumpCount", GetBasicMaxJumpCountAttribute());
		AttributeMap.Add("CurMaxJumpCount", GetCurMaxJumpCountAttribute());
		// 起跳速度
		AttributeMap.Add("BasicJumpSpeed", GetBasicJumpSpeedAttribute());
		AttributeMap.Add("CurJumpSpeed", GetCurJumpSpeedAttribute());
		// 跳跃上升可控时间
		AttributeMap.Add("BasicJumpMaxHoldTime", GetBasicJumpMaxHoldTimeAttribute());
		AttributeMap.Add("CurJumpMaxHoldTime(", GetCurJumpMaxHoldTimeAttribute());
		AttributeMap.Add("BasicAirControl", GetBasicAirControlAttribute());
		AttributeMap.Add("CurAirControl", GetCurAirControlAttribute());
		
		
		// 攻击
		AttributeMap.Add("BasicAttackValue", GetBasicAttackValueAttribute());
		AttributeMap.Add("CurAttackValue", GetCurAttackValueAttribute());
		// 攻击速度
		AttributeMap.Add("BasicAttackSpeed", GetBasicAttackSpeedAttribute());
		AttributeMap.Add("CurAttackSpeed", GetCurAttackSpeedAttribute());

		// 击退力
		AttributeMap.Add("BasicRepelValue", GetBasicRepelValueAttribute());
		AttributeMap.Add("CurRepelValue", GetCurRepelValueAttribute());
		// 击退抵抗值
		AttributeMap.Add("BasicRepelResist", GetBasicRepelResistAttribute());
		AttributeMap.Add("CurRepelResist", GetCurRepelResistAttribute());

		// 重力
		AttributeMap.Add("BasicGravityScale", GetBasicGravityScaleAttribute());
		AttributeMap.Add("CurGravityScale", GetCurGravityScaleAttribute());

		// 体型
		AttributeMap.Add("BasicBodySizeScale", GetBasicBodySizeScaleAttribute());
		AttributeMap.Add("CurBodySizeScale", GetCurBodySizeScaleAttribute());
	}
}









void UPXAttributeSet::OnRep_HP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, HP, OldValue);
}

void UPXAttributeSet::OnRep_BasicMaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicMaxHP, OldValue);
}

void UPXAttributeSet::OnRep_CurMaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurMaxHP, OldValue);
}

void UPXAttributeSet::OnRep_BasicHPRecoverValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicHPRecoverValue, OldValue);
}

void UPXAttributeSet::OnRep_CurHPRecoverValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurHPRecoverValue, OldValue);
}

void UPXAttributeSet::OnRep_EP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, EP, OldValue);
}

void UPXAttributeSet::OnRep_BasicMaxEP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicMaxEP, OldValue);
}

void UPXAttributeSet::OnRep_CurMaxEP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurMaxEP, OldValue);
}

void UPXAttributeSet::OnRep_BasicEPRecoverValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicEPRecoverValue, OldValue);
}

void UPXAttributeSet::OnRep_CurEPRecoverValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurEPRecoverValue, OldValue);
}

void UPXAttributeSet::OnRep_BasicAcceleration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicAcceleration, OldValue);
}

void UPXAttributeSet::OnRep_BasicSight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicSight, OldValue);
}

void UPXAttributeSet::OnRep_CurSight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurSight, OldValue);
}

void UPXAttributeSet::OnRep_BasicSightReductionResist(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicSightReductionResist, OldValue);
}

void UPXAttributeSet::OnRep_CurSightReductionResist(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurSightReductionResist, OldValue);
}

void UPXAttributeSet::OnRep_BasicBodySizeScale(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicBodySizeScale, OldValue);
}

void UPXAttributeSet::OnRep_CurBodySizeScale(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurBodySizeScale, OldValue);
}

void UPXAttributeSet::OnRep_BasicJumpMaxHoldTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicJumpMaxHoldTime, OldValue);
}

void UPXAttributeSet::OnRep_CurJumpMaxHoldTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurJumpMaxHoldTime, OldValue);
}

void UPXAttributeSet::OnRep_BasicAttackValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicAttackValue, OldValue);
}

void UPXAttributeSet::OnRep_CurAttackValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurAttackValue, OldValue);
}

void UPXAttributeSet::OnRep_BasicAttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicAttackSpeed, OldValue);
}

void UPXAttributeSet::OnRep_CurAttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurAttackSpeed, OldValue);
}

void UPXAttributeSet::OnRep_BasicSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicSpeed, OldValue);
}

void UPXAttributeSet::OnRep_CurSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurSpeed, OldValue);
}

void UPXAttributeSet::OnRep_CurAcceleration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurAcceleration, OldValue);
}

void UPXAttributeSet::OnRep_BasicMaxJumpCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicMaxJumpCount, OldValue);
}

void UPXAttributeSet::OnRep_CurMaxJumpCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurMaxJumpCount, OldValue);
}

void UPXAttributeSet::OnRep_BasicJumpSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicJumpSpeed, OldValue);
}

void UPXAttributeSet::OnRep_CurJumpSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurJumpSpeed, OldValue);
}


void UPXAttributeSet::OnRep_BasicRepelValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicRepelValue, OldValue);
}

void UPXAttributeSet::OnRep_CurRepelValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurRepelValue, OldValue);
}

void UPXAttributeSet::OnRep_BasicRepelResist(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicRepelResist, OldValue);
}

void UPXAttributeSet::OnRep_CurRepelResist(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurRepelResist, OldValue);
}

void UPXAttributeSet::OnRep_BasicGravityScale(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicGravityScale, OldValue);
}

void UPXAttributeSet::OnRep_CurGravityScale(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurGravityScale, OldValue);
}


void UPXAttributeSet::OnRep_BasicAirControl(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicAirControl, OldValue);
}

void UPXAttributeSet::OnRep_CurAirControl(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurAirControl, OldValue);
}


void UPXAttributeSet::OnRep_BasicDashSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, BasicDashSpeed, OldValue);
}
void UPXAttributeSet::OnRep_CurDashSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, CurDashSpeed, OldValue);
}
