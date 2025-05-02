// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "BasePixelCharacter.generated.h"

class UInputAction;
struct FInputActionValue;
class UAbilityComponent;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API ABasePixelCharacter : public APaperZDCharacter, public IFight_Interface
{
	GENERATED_BODY()

	float FallingStartTime;
	bool PreFrameFalling = false;
	bool PreSpriteLeft = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector FightCenterForCameraOffset = FVector(0.0f);
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector CurCameraOffset = FVector(0.0f);
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector SurCameraOffset = FVector(0.0f);

	FTimerHandle ScaleTimerHandle;
	FTimerHandle AttackHitTimerHandle;
	int AttackHitComboNum = 0;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int AttackHitDashing = 0; // 冲刺中命中

	float ScaleLerpValue = 0;
	float ScaleCurValue = 1.0f;
	FVector InitScale;
	
	
	virtual void Tick_SaveFallingStartTime();
	virtual void Tick_SpriteRotation(); // 漂移式偏转
	virtual void Tick_SpringArmMotivation();
	virtual void Tick_CalCameraOffset();

	void SetLanding(const bool V, const float time = 0.1f);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category = Fight)
	TObjectPtr<UHealthComponent> HealthComponent_CPP;
	
public:
	ABasePixelCharacter(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Fight)
	UHealthComponent* GetHealthComponent() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName="FightComp", Category = Fight)
	TObjectPtr<UFightComponent> FightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName="AbilityComp", Category = Fight)
	TObjectPtr<UAbilityComponent> AbilityComponent;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Fight)
	UFightComponent* GetFightComponent() const;
	
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Falling() override;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float BasicMoveSpeed = 200.f;
	
	UPROPERTY(BlueprintReadOnly, Category = View)
	float CurBlendPitch = -20.0f;

	// 玩家视角水平转移角度
	UPROPERTY(BlueprintReadWrite, Category = View)
	float DeltaBlendYaw = 0.0f;
	// 玩家视角水平转移
	UPROPERTY(BlueprintReadOnly, Category = View)
	float CurBlendYaw = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float BasicSpringArmLength = 280.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float CurSpringArmLength = 280.0f;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetMoveForwardVector();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetFaceToCamera();
	UFUNCTION(BlueprintCallable, Category = View)
	void AddCameraOffset(const FVector& V);

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bDead;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bHurt;

	// 处于击退的硬直状态
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bRepulsion;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bJumping;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bLanding;
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bDashing;

	// 用于管理攻击冷却, 目前暂无用于动画控制
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool bInAttackStatus;

	// 用于管理攻击效果生效，暂不用于动画控制
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool bInAttackEffect;

	// 仅仅用于控制攻击动画开关
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackAnimToggle;  

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackHolding;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackFire;
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bMoving;

	UFUNCTION(BlueprintCallable, Category = View)
	void SetBlendPitch(float V);
	UFUNCTION(BlueprintCallable, Category = View)
	void SetBlendYaw(float V);
	UFUNCTION(BlueprintCallable, Category = View)
	void AddBlendYaw(float V);
	
	// 近战攻击生效碰撞期
	virtual bool GetIsAttacking() override;
	virtual bool CanAttack_Implementation() override;
	
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetHurt(const bool V, const float duration);
	
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetJumping(const bool V, const float time = 0.2f);
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetFalling(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetDashing(const bool V);

	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetAttackAnimToggle(const bool V);

	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetAttackHolding(const bool V);

	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetAttackFire(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetMoving(const bool V);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
	float GetFallingTime();

	
	
	virtual void Tick(float DeltaSeconds) override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character | Movement")
	virtual FVector GetDashDirection();


	// IFight_Interface
	virtual bool IsAlive_Implementation() override;
	virtual FGameplayTagContainer GetOwnCamp_Implementation() override;
	virtual FGameplayTagContainer GetEnemyCamp_Implementation() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual void OnAttackHiting_Implementation() override;
	virtual void PowerRepulsion_Implementation(float Power) override;
	virtual void OnBeAttacked_Invulnerable_Implementation() override;
	virtual bool OnBeAttacked_Implementation(AActor* maker, int damage) override;
	virtual int DamagePlus_Implementation(int inValue, AActor* ActorAcceptDamage) override;
	virtual int OnDefendingHit_Implementation(int inValue) override;
	


	
	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
													const FVector& PreviousLocation, float TimeDelta) override;
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;



	// 外形缩放部分
	UFUNCTION(BlueprintCallable, Category="Character | Outlook")
	void SetScale(const float targetValue);



	

	/*
	 * ----- 移动输入部分 ----- 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Character | InputAction")
	UInputAction* Action_MoveFront; // 朝向摄像机移动
	UPROPERTY(EditDefaultsOnly, Category = "Character | InputAction")
	UInputAction* Action_MoveBack;  // 远离摄像机移动
	UPROPERTY(EditDefaultsOnly, Category = "Character | InputAction")
	UInputAction* Action_MoveLeft;
	UPROPERTY(EditDefaultsOnly, Category = "Character | InputAction")
	UInputAction* Action_MoveRight;
	UPROPERTY(EditDefaultsOnly, Category = "Character | InputAction")
	UInputAction* Action_MoveGP; // 手柄左摇杆
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;
	void MoveX(const FInputActionValue& Value);
	void MoveY(const FInputActionValue& Value);
	void Move2D(const FInputActionValue& Value);
	
};

inline bool ABasePixelCharacter::CanAttack_Implementation()
{
	return !GetIsAttacking() && !bDashing && !bRepulsion &&	!bHurt && !bDead;
}

inline void ABasePixelCharacter::SetBlendPitch(float V)
{
	CurBlendPitch = V;
}


inline void ABasePixelCharacter::SetBlendYaw(float V)
{
	DeltaBlendYaw = 0;
	CurBlendYaw = V;
}

inline void ABasePixelCharacter::AddBlendYaw(float V)
{
	DeltaBlendYaw = -V;
	CurBlendYaw += V;
}



