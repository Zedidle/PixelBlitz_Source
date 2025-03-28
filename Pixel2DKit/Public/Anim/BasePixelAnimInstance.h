// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "AnimNodes/PaperZDAnimNode_Base.h"
#include "BasePixelAnimInstance.generated.h"

enum EActionField : uint8;
struct FPaperZDAnimDataLink;
class UPaperZDAnimSequence_Flipbook;

USTRUCT(BlueprintType)
struct FPaperZDAnimDataLinks
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPaperZDAnimSequence_Flipbook*> AnimSequences;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> AnimSequencesRate;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PIXEL2DKIT_API UBasePixelAnimInstance : public UPaperZDAnimInstance
{
	GENERATED_BODY()

	// 怪物各方位的攻击动画概率分布
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<EActionField>, FPaperZDAnimDataLinks> ActionFieldToAnimData;

	// 攻击间隔待机动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UPaperZDAnimSequence_Flipbook* AttackIdleAnim;


	
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

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bInAttackState;  // 整体上是否处于攻击状态（起手预备 - 生效 - 后摇）
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackStart;

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
	virtual void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetHurt(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetMoving(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetJumping(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetFalling(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetLanding(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetInAttackState(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetAttackStart(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetInAttackEffect(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetAttackHolding(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetAttackFire(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual void SetDashing(const bool V);


	



	// 一般是怪物专用
	// 当前相对于玩家角色的行动区间
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TEnumAsByte<EActionField> CurActionFiled;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bAttackStartX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bAttackStartY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bInDefendState; // 是否处于防御状态

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bDefendStart; // 防御状态的开始，用于触发防御动画

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bDefendHurt;  // 处于防御状态被攻击

	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetActionField(const EActionField field);

	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetAttackStartX(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetAttackStartY(const bool V);

	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetDefendState(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetDefendStart(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetDefendHurt(const bool V);
	

	
	
	virtual void Tick(float DeltaTime) override;

};

