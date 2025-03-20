// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
// #include "InputAction.h"
// #include "InputActionValue.h"
#include "BasePixelCharacter.generated.h"

class UInputAction;
struct FInputActionValue;

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
	FVector FightCenterForCameraOffset = FVector(0.0f);
	FVector CurCameraOffset = FVector(0.0f);
	int AttackHitComboNum = 0;
	
	virtual void Tick_SaveFallingStartTime();
	virtual void Tick_SpriteRotation_cpp(); // 漂移式偏转
	virtual void Tick_SpringArmMotivation_cpp();

	void SetLanding(const bool V, const float time = 0.1f);

	
public:
	ABasePixelCharacter();

	virtual void BeginPlay() override;
	
	virtual void Falling() override;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float BasicMoveSpeed = 200.f;
	
	UPROPERTY(BlueprintReadOnly, Category = View)
	float CurBlendPitch = -20.0f;

	UPROPERTY(BlueprintReadWrite, Category = View)
	float DeltaBlendYaw = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = View)
	float CurBlendYaw = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float CurSpringArmLength = 280.0f;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetMoveForwardVector();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetFaceToCamera();


	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bDead;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bHurt;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bJumping;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bLanding;
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bDashing;
	
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackStart;  //仅用于控制攻击动画开始

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
	


	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetHurt(const bool V);

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetJumping(const bool V, const float time = 0.2f);
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetFalling(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetDashing(const bool V);

	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetAttackStart(const bool V);

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
	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="Fight_Interface")
	AActor* GetTarget();
	virtual AActor* GetTarget_Implementation();

	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta) override;

	
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;


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
// 	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
// FString::Printf(TEXT("ABasePixelCharacter::AddBlendYaw CurBlendYaw:%f, DeltaBlendYaw:%f ,  %d"), CurBlendYaw, DeltaBlendYaw, __LINE__));
}



