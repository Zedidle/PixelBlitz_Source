// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "PXAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPXAttributeChangeDelegate, const FGameplayAttribute&, Attribute, float,  OldValue, float, NewValue);


UCLASS()
class PIXEL2DKIT_API UPXAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;


public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetAttrCurrentValueByName(FName AttrName, float& OutValue);
	
	UFUNCTION(BlueprintCallable)
	bool SetAttrCurrentValueByName(FName AttrName, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetAttrBaseValueByName(FName AttrName, float& OutValue);

	UFUNCTION(BlueprintCallable)
	bool SetAttrBaseValueByName(FName AttrName, float Value);

	// 静态成员初始化
	static TMap<FName, FGameplayAttribute>& GetAttributeMapInstance()
	{
		static TMap<FName, FGameplayAttribute> AttributeMap;
		return AttributeMap;
	}
	static TMap<FName, FGameplayAttribute>& GetAttributeMap();

	UFUNCTION(BlueprintCallable, Category = "AttributeSet")
	static FGameplayAttribute GetAttributeByName(const FString& AttributeName);
	
	
private:
	FORCEINLINE FStructProperty* GetAttrStructProperty(FName AttrName) const;
	FORCEINLINE FGameplayAttributeData* GetGameplayAttributeData(FName AttrName);

	static void InitializeAttributeMap();
	
	
public:
	UPROPERTY(BlueprintAssignable, Category = "AttributeSet|Prop")
	mutable FOnPXAttributeChangeDelegate OnPXAttributeChange;
	
	// 当前生命值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_HP)
	FGameplayAttributeData HP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, HP);
	UFUNCTION()
	virtual void OnRep_HP(const FGameplayAttributeData& OldValue);
	
	// 最大生命值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicMaxHP)
	FGameplayAttributeData BasicMaxHP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicMaxHP);
	UFUNCTION()
	virtual void OnRep_BasicMaxHP(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurMaxHP)
	FGameplayAttributeData CurMaxHP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurMaxHP);
	UFUNCTION()
	virtual void OnRep_CurMaxHP(const FGameplayAttributeData& OldValue);

	// 生命值（每5秒）恢复值，从自定义恢复曲线中读取
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicHPRecoverValue)
	FGameplayAttributeData BasicHPRecoverValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicHPRecoverValue);
	UFUNCTION()
	virtual void OnRep_BasicHPRecoverValue(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurHPRecoverValue)
	FGameplayAttributeData CurHPRecoverValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurHPRecoverValue);
	UFUNCTION()
	virtual void OnRep_CurHPRecoverValue(const FGameplayAttributeData& OldValue);
	
	// 当前能量值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_EP)
	FGameplayAttributeData EP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, EP);
	UFUNCTION()
	virtual void OnRep_EP(const FGameplayAttributeData& OldValue);

	// 基础最大能量值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicMaxEP)
	FGameplayAttributeData BasicMaxEP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicMaxEP);
	UFUNCTION()
	virtual void OnRep_BasicMaxEP(const FGameplayAttributeData& OldValue);

	// 当前最大能量值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurMaxEP)
	FGameplayAttributeData CurMaxEP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurMaxEP);
	UFUNCTION()
	virtual void OnRep_CurMaxEP(const FGameplayAttributeData& OldValue);
	
	// 能量值（每5秒）恢复值，曲线直接设置的恢复值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicEPRecoverValue)
	FGameplayAttributeData BasicEPRecoverValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicEPRecoverValue);
	UFUNCTION()
	virtual void OnRep_BasicEPRecoverValue(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurEPRecoverValue)
	FGameplayAttributeData CurEPRecoverValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurEPRecoverValue);
	UFUNCTION()
	virtual void OnRep_CurEPRecoverValue(const FGameplayAttributeData& OldValue);
	
	// 基础移动速度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicSpeed)
	FGameplayAttributeData BasicSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicSpeed);
	UFUNCTION()
	virtual void OnRep_BasicSpeed(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurSpeed)
	FGameplayAttributeData CurSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurSpeed);
	UFUNCTION()
	virtual void OnRep_CurSpeed(const FGameplayAttributeData& OldValue);

	// 加速度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicAcceleration)
	FGameplayAttributeData BasicAcceleration;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicAcceleration);
	UFUNCTION()
	virtual void OnRep_BasicAcceleration(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurAcceleration)
	FGameplayAttributeData CurAcceleration;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurAcceleration);
	UFUNCTION()
	virtual void OnRep_CurAcceleration(const FGameplayAttributeData& OldValue);

	// Dash速度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicDashSpeed)
	FGameplayAttributeData BasicDashSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicDashSpeed);
	UFUNCTION()
	virtual void OnRep_BasicDashSpeed(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurDashSpeed)
	FGameplayAttributeData CurDashSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurDashSpeed);
	UFUNCTION()
	virtual void OnRep_CurDashSpeed(const FGameplayAttributeData& OldValue);

	
	// 基础视野
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicSight)
	FGameplayAttributeData BasicSight;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicSight);
	UFUNCTION()
	virtual void OnRep_BasicSight(const FGameplayAttributeData& OldValue);

	// 当前视野，可能后续优化就需要通过 GE 来处理 AttributeData 的 BaseValue 和 CurrentValue  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurSight)
	FGameplayAttributeData CurSight;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurSight);
	UFUNCTION()
	virtual void OnRep_CurSight(const FGameplayAttributeData& OldValue);

	// 视野缩减抵抗值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicSightReductionResist)
	FGameplayAttributeData BasicSightReductionResist;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicSightReductionResist);
	UFUNCTION()
	virtual void OnRep_BasicSightReductionResist(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurSightReductionResist)
	FGameplayAttributeData CurSightReductionResist;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurSightReductionResist);
	UFUNCTION()
	virtual void OnRep_CurSightReductionResist(const FGameplayAttributeData& OldValue);

	
	// 基础最大跳跃次数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicMaxJumpCount)
	FGameplayAttributeData BasicMaxJumpCount;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicMaxJumpCount);
	UFUNCTION()
	virtual void OnRep_BasicMaxJumpCount(const FGameplayAttributeData& OldValue);

	// 当前最大跳跃次数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurMaxJumpCount)
	FGameplayAttributeData CurMaxJumpCount;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurMaxJumpCount);
	UFUNCTION()
	virtual void OnRep_CurMaxJumpCount(const FGameplayAttributeData& OldValue);

	// 跳跃初始速度（决定最终高度）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicJumpSpeed)
	FGameplayAttributeData BasicJumpSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicJumpSpeed);
	UFUNCTION()
	virtual void OnRep_BasicJumpSpeed(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurJumpSpeed)
	FGameplayAttributeData CurJumpSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurJumpSpeed);
	UFUNCTION()
	virtual void OnRep_CurJumpSpeed(const FGameplayAttributeData& OldValue);

	// 基础跳跃上升可控时间
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicJumpMaxHoldTime)
	FGameplayAttributeData BasicJumpMaxHoldTime;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicJumpMaxHoldTime);
	UFUNCTION()
	virtual void OnRep_BasicJumpMaxHoldTime(const FGameplayAttributeData& OldValue);
	
	// 基础跳跃上升可控时间增加
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurJumpMaxHoldTime)
	FGameplayAttributeData CurJumpMaxHoldTime;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurJumpMaxHoldTime);
	UFUNCTION()
	virtual void OnRep_CurJumpMaxHoldTime(const FGameplayAttributeData& OldValue);

	// 基础空中控制效能
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicAirControl)
	FGameplayAttributeData BasicAirControl;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicAirControl);
	UFUNCTION()
	virtual void OnRep_BasicAirControl(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurAirControl)
	FGameplayAttributeData CurAirControl;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurAirControl);
	UFUNCTION()
	virtual void OnRep_CurAirControl(const FGameplayAttributeData& OldValue);
	
	
	// 基础攻击力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicAttackValue)
	FGameplayAttributeData BasicAttackValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicAttackValue);
	UFUNCTION()
	virtual void OnRep_BasicAttackValue(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurAttackValue)
	FGameplayAttributeData CurAttackValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurAttackValue);
	UFUNCTION()
	virtual void OnRep_CurAttackValue(const FGameplayAttributeData& OldValue);
	
	// 基础攻击速度(次/s)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicAttackSpeed)
	FGameplayAttributeData BasicAttackSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicAttackSpeed);
	UFUNCTION()
	virtual void OnRep_BasicAttackSpeed(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurAttackSpeed)
	FGameplayAttributeData CurAttackSpeed;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurAttackSpeed);
	UFUNCTION()
	virtual void OnRep_CurAttackSpeed(const FGameplayAttributeData& OldValue);

	
	// 击退力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicRepelValue)
	FGameplayAttributeData BasicRepelValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicRepelValue);
	UFUNCTION()
	virtual void OnRep_BasicRepelValue(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurRepelValue)
	FGameplayAttributeData CurRepelValue;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurRepelValue);
	UFUNCTION()
	virtual void OnRep_CurRepelValue(const FGameplayAttributeData& OldValue);
	
	// 击退抵抗值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicRepelResist)
	FGameplayAttributeData BasicRepelResist;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicRepelResist);
	UFUNCTION()
	virtual void OnRep_BasicRepelResist(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurRepelResist)
	FGameplayAttributeData CurRepelResist;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurRepelResist);
	UFUNCTION()
	virtual void OnRep_CurRepelResist(const FGameplayAttributeData& OldValue);

	
	// 重力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicGravityScale)
	FGameplayAttributeData BasicGravityScale;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicGravityScale);
	UFUNCTION()
	virtual void OnRep_BasicGravityScale(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurGravityScale)
	FGameplayAttributeData CurGravityScale;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurGravityScale);
	UFUNCTION()
	virtual void OnRep_CurGravityScale(const FGameplayAttributeData& OldValue);

	
	// 体型
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BasicBodySizeScale)
	FGameplayAttributeData BasicBodySizeScale;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, BasicBodySizeScale);
	UFUNCTION()
	virtual void OnRep_BasicBodySizeScale(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurBodySizeScale)
	FGameplayAttributeData CurBodySizeScale;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, CurBodySizeScale);
	UFUNCTION()
	virtual void OnRep_CurBodySizeScale(const FGameplayAttributeData& OldValue);
};
