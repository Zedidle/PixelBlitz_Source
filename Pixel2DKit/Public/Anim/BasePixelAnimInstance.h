// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "BasePixelAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PIXEL2DKIT_API UBasePixelAnimInstance : public UPaperZDAnimInstance
{
	GENERATED_BODY()


public:
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bDead;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bHurt;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bMoving;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bJumping;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bLanding;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bFalling;
	
	// 整体上是否处于攻击状态（起手预备 - 生效 - 后摇）
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bInAttackState; 
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackAnimToggle;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bInAttackEffect; // 处于攻击状态的效应时间 （例如近战碰撞盒子的生效期间）
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackHolding;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackFire;
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bDashing;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	int eMoveStatus;
	

	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetActionField(const EActionField field);

	// 一般是怪物专用
	// 当前相对于玩家角色的行动区间
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TEnumAsByte<EActionField> CurActionFiled;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bInDefendState; // 是否处于防御状态

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bDefendStart; // 防御状态的开始，用于触发防御动画

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bDefendHurt;  // 处于防御状态被攻击


	
	
	virtual void Tick(float DeltaTime) override;

};

