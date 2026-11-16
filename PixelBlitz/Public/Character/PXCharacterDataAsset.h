// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXCharacterInputConfig.h"
#include "Engine/DataAsset.h"
#include "UI/Player/BasePlayerStatusWidget.h"
#include "PXCustomStruct.h"
#include "PXCharacterDataAsset.generated.h"

class ABaseWeapon;

UCLASS()
class PIXELBLITZ_API UPXCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TArray<TSoftObjectPtr<UDataTable>> AbilityDataTables;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<UPXCharacterInputConfig> InputConfig;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> LandedSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> JumpSoundOnGround;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> JumpSoundInAir;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> HurtSound;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	FCharacterAttribute CharacterAttribute;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSubclassOf<UBasePlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSubclassOf<ABaseWeapon> WeaponClass;

	// 死亡复活所需时间
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset | GameplayFlow")
	float ReviveDelayTime = 3.0f;

	// 死亡复活期间过渡时间
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset | GameplayFlow")
	float RevivePassDayTime = 600.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> InitAbilitiesToGive;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> InitEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttackType NormalAttackType;

	// ---- 近战蓄力（RedHair 蓄力剑锋闪 / 旋风斩）----
	// 仅 Melee 攻击类型生效；弓箭手（Projectile）走动画驱动的 AttackHolding 管线，不受这些参数影响

	// 0 = 关闭该角色的攻击蓄力（保持原有按住自动连击行为）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold")
	bool bEnableMeleeAttackHold = false;

	// 蓄力入场阈值（秒，从按下起算）：超过该时长且普攻已结算、按键仍按住 → 进入蓄力态
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold", meta = (ClampMin = "0.05", UIMin = "0.05"))
	float AttackHoldStartTime = 0.8f;

	// 满蓄时长（秒，从按下起算）：伤害缩放封顶并触发满蓄表现
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float AttackHoldFullTime = 1.5f;

	// 蓄力伤害倍率下限（入场瞬间，1.0 = 无增益）：0.8s 蓄力 = 1.3 倍
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float AttackHoldDamageScaleMin = 1.3f;

	// 蓄力伤害倍率上限（满蓄瞬间）：1.5s 满蓄 = 1.5 倍
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float AttackHoldDamageScaleMax = 1.5f;

	// 蓄力冲刺速度/距离倍率下限（入场瞬间，1.0 = 不改变手感）
	// 直接乘在 GetDashSpeedValue 上：冲刺时长恒定，所以速度倍率 = 近战冲刺距离倍率（不影响移动速度）
	// 蓄力期间的移速惩罚是独立机制（bAttackHoldSlowDown / AttackHoldMoveSpeedSub），两者互不影响
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float AttackHoldSpeedScaleMin = 1.0f;

	// 蓄力冲刺速度/距离倍率上限（满蓄瞬间）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float AttackHoldSpeedScaleMax = 1.0f;

	// 蓄力期间移速惩罚开关（Projectile 原路径始终生效，不受此项影响）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeroDataAsset | AttackHold")
	bool bAttackHoldSlowDown = false;
};
