// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Fight/Components/StateComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "Item/Weapon/BaseWeapon.h"
#include "UI/Player/BasePlayerStatusWidget.h"
#include "BasePXCharacter.generated.h"

class ABaseInteractableItem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDie);

// 发起攻击的监听事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerAttackStart, EAttackType, AttackType, FVector, AttackDirection);



class UInputAction;
class UAbilityComponent;
class UBuffComponent;
class UTalentComponent;

class UPXASComponent;
class UStateComponent;
class UFightComponent;

class UCameraComponent;
class USpringArmComponent;

struct FInputActionValue;


USTRUCT(BlueprintType)
struct FEffectGameplayTags
{
	GENERATED_BODY()
private:
	TMap<FGameplayTag, float> Data;

public:
	bool Contains(const FGameplayTag& Tag) const
	{
		return Data.Contains(Tag);
	}

	float GetData(const FGameplayTag& Tag) const
	{
		if ( Data.Contains(Tag))
		{
			return Data[Tag];
		}
		return 0.0f;
	}
	
	float operator[](const FGameplayTag& Tag) const {
		if ( Data.Contains(Tag))
		{
			return Data[Tag];
		}
		return 0.0f;
	}

	void SetData(const FGameplayTag& Tag, float Value)
	{
		Data.Add(Tag, Value);
	}

	void AddData(const FGameplayTag& Tag, float Value)
	{
		if (Data.Contains(Tag))
		{
			Data[Tag] += Value;
		}
		else
		{
			Data.Add(Tag, Value);
		}
	}

	TArray<FGameplayTag> GetAllKeys() const
	{
		TArray<FGameplayTag> Keys;
		Data.GenerateKeyArray(Keys);
		return Keys;
	}
};



UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API ABasePXCharacter : public APaperZDCharacter, public IFight_Interface, public IInteract_Interface,
										public IAbilitySystemInterface
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void LoadData();
	
	float FallingStartTime;
	bool PreFrameFalling = false;
	bool PreSpriteLeft = false;
	FVector CameraOffsetForBulletTime;
	float JumpStartTime = -1;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBasePlayerStatusWidget* PlayerStatusWidget;
	
public:

	// 要在蓝图类中直接设定
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UPXCharacterDataAsset* DataAsset;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDie OnPlayerDie;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnPlayerAttackStart OnPlayerAttackStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VelocityRepelFactor = FVector(1.0, 1.0, 1.2);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashInitSpeedPercent = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashNewDirSpeedPercent = 0.2;
	
	int AttackHitComboNum = 0;

	float ScaleLerpValue = 0;
	float ScaleCurValue = 1.0f;
	FVector InitScale;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetSpriteForwardVector();

	
#pragma region 临时存储数据，后续看存到哪

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int PerfectDodgeTimes = 0;

	UPROPERTY(BlueprintReadWrite, Category = Achievement)
	bool HasOneHealthPoint = false;

	UPROPERTY(BlueprintReadWrite, Category = Achievement)
	int AirDashTimes = 0;
	
#pragma endregion 

	
	virtual void Tick_SaveFallingStartTime(float DeltaSeconds);
	virtual void Tick_SpriteRotation(float DeltaSeconds); // 漂移式偏转
	virtual void Tick_SpringArmMotivation(float DeltaSeconds);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MinDot_VelocityToCamera = 0.1f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MinCP_VelocityToCamera = 0.1f;

	bool TurnLeft;
	bool TurnRight;
	
	FVector CurCameraOffset;
	TMap<FName, FVector> CameraOffsetMap;
	UFUNCTION(BlueprintCallable)
	void AddCameraOffset(FName OffsetName, FVector Offset, float SustainTime = 0);
	
	UFUNCTION(BlueprintCallable)
	void RemoveCameraOffset(FName OffsetName);
	
	void SetLanding(const bool V, const float time = 0.1f);


	UFUNCTION(BlueprintNativeEvent)
	void OnHPChanged(int32 OldValue, int32 NewValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckNearDeath();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Revive();

#pragma region GAS


	UPROPERTY(BlueprintReadOnly)
	UPXASComponent* CachedASC;
		
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY()
	TWeakObjectPtr<class UPXAttributeSet> AttributeSetBase;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> InitEffects;

#pragma endregion
	
	ABasePXCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GAS)
	UPXAttributeSet* GetAttributeSet() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStateComponent* StateComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFightComponent* FightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAbilityComponent* AbilityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBuffComponent* BuffComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;
	
#pragma region EffectGameplayTags
	UPROPERTY()
	FEffectGameplayTags EffectGameplayTags;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasEffectGameplayTag(const FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetEffectGameplayTag(const FGameplayTag Tag) const;
#pragma endregion

	float PreSpringArmZ = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	FVector FrontViewFactor = {0.4, 0.4, 0.1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	FVector FollowViewFactor = {-0.2, -0.5, -0.1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	FVector SpringArmMotivationVelocityPowFactor = {1.0f, 1.0f, 1.0f};
	
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = GameplayFlow)
	void ToStartPoint();

	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = Movement)
	bool SelfCanJump();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Movement)
	void JumpStart();
	virtual void Jump() override;
	virtual void OnJumped_Implementation() override;
	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ReadyToStart();

	UPROPERTY(BlueprintReadOnly)
	ABaseWeapon* Weapon;
	
	UFUNCTION(BlueprintCallable)
	void LoadWeapon(TSubclassOf<ABaseWeapon> WeaponClass = nullptr);

	
	UPROPERTY(BlueprintReadOnly, Category = View)
	float CurBlendPitch = -20.0f;

	UPROPERTY(BlueprintReadOnly, Category = View)
	float CurDot_VelocityToCamera = 0;
	
	
	// 玩家视角水平转移角度
	UPROPERTY(BlueprintReadWrite, Category = View)
	float DeltaBlendYaw = 0.0f;
	// 玩家视角水平转移
	UPROPERTY(BlueprintReadOnly, Category = View)
	float CurBlendYaw = 0.0f;

	// 实际上就是Sight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float BasicSpringArmLength = 280.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float CurSpringArmLength = 280.0f;

	// Right就是东侧
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetRightVectorWithBlendYaw();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetVectorFaceToCamera();
	

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	int CurJumpCount = 0;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	int32 MaxJumpCount = 2;
	
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
	

	// 仅用于控制攻击动画的进入
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackAnimToggle; 

	// 处于攻击前摇
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool bAttackStartup;
	
	// 用于管理攻击效果生效，暂不用于动画控制
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool bInAttackEffect;

	// 冲刺进入
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bDashing;

	// 冲刺生效期间
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool InDashEffect;

	// 攻击蓄力中
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackHolding;

	// 蓄力攻击释放
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
	
	void AddViewYaw(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = View)
	void AddViewYaw(float AxisValue);
	UFUNCTION(BlueprintCallable, Category = View)
	void AddViewPitch(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	FVector2D ViewPitchRange = FVector2D(-50, 20);
	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Game)
	void PreReadyToStart();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Movement)
	float GetFallingTime();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character | Movement")
	virtual FVector GetDashDirection();

	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
													const FVector& PreviousLocation, float TimeDelta) override;

	
	// 外形缩放部分
	UFUNCTION(BlueprintCallable, Category="Character | Outlook")
	void SetScale(const float targetValue);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadAbility();


	UFUNCTION(BlueprintCallable)
	void CameraOffset_BulletTime(float SustainTime, FVector CameraOffset = FVector(0), float GlobalTimeRate = 0.1f);

	UFUNCTION(BlueprintCallable)
	void OutOfControl(float SustainTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector CalSkillVelocity(float DashSpeed);
	
#pragma region Animation
	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetHurt(const bool V, const float duration = 0.1f);
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetJumping(const bool V, float time = 0.2f);
	
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

	UFUNCTION(BlueprintCallable, Category = Anim)
	void EndNormalAttack();
#pragma endregion

	
#pragma region IFight_Interface
	virtual bool GetIsAttacking() override;
	virtual bool GetIsDefending() override;
	virtual bool CanAttack_Implementation() override;
	virtual bool IsAlive_Implementation() override;
	virtual FGameplayTagContainer GetOwnCamp_Implementation() const override;
	virtual FGameplayTagContainer GetEnemyCamp_Implementation() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual void OnAttackHit_Implementation(AActor* Receiver) override;
	virtual void OnSkillHit_Implementation() override;
	virtual void PowerRepulsion_Implementation(float Power) override;
	virtual void OnBeAttacked_Invulnerable_Implementation() override;
	virtual void OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce) override;
	virtual int DamagePlus_Implementation(int InDamage, AActor* Receiver) override;
	virtual void OnAttackWeakPoint_Implementation(AActor* Receiver) override;
	virtual int OnDefendingHit_Implementation(int inValue) override;
	virtual void OnAttackHolding_Implementation() override;
	virtual void OnDefendingHitEffect_Implementation() override;
	virtual void OnAnimVulnerableBegin_Implementation() override;
	virtual void OnAnimVulnerableEnd_Implementation() override;
	virtual void OnAttackEffect_Implementation() override;
	virtual void OnAttackEffectBegin_Implementation() override;
	virtual void OnAttackEffectEnd_Implementation() override;
	virtual void OnPickGold_Implementation() override;
	virtual void OnDashEffectBegin_Implementation() override;
	virtual void OnDashEffectEnd_Implementation() override;
	virtual bool FindEffectGameplayTag_Implementation(const FGameplayTag Tag, float& Result) override;
	virtual APawn* GetPawn_Implementation() override;
	virtual float GetAttackCD_Implementation() override;
	virtual int GetAttackDamage_Implementation() override;
	virtual FVector GetAttackRepel_Implementation() override;
	virtual void OnDie_Implementation() override;
#pragma endregion


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttackRelease();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttackHoldingRelease();

	UFUNCTION(BlueprintNativeEvent)
	void OnKillEnemy();
	
	int32 CalInitDamage(int32 InDamage);

#pragma region Input
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;
	void Input_Move2D(const FInputActionValue& Value);
	void TryToAttack();
	void AttackRelease();
	void TryToJump();
	void JumpRelease();
	void Interact();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="TryUseSkill")
	void BP_TryUseSkill();
	void TryUseSkill();
	
#pragma endregion

	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseInteractableItem>> InteractableItems;
	void AddInteractableItem(ABaseInteractableItem* Item);
	void RemoveInteractableItem(ABaseInteractableItem* Item);
	

	UFUNCTION(BlueprintNativeEvent)
	void OnLevelLoading(FGameplayTag Channel, const FDefaultEmptyMessage& Message);

	UFUNCTION(BlueprintNativeEvent)
	void OnLevelLoaded(FGameplayTag Channel, const FDefaultEmptyMessage& Message);

#pragma region Massage
	
	FGameplayMessageListenerHandle ListenerHandle_OnLevelLoading;
	FGameplayMessageListenerHandle ListenerHandle_OnLevelLoaded;
	
#pragma endregion


	UFUNCTION()
	void OnAttributeChanged(const FGameplayAttribute& Attribute, float  OldValue, float NewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetBasicDashSpeed();
	
	
};

inline bool ABasePXCharacter::CanAttack_Implementation()
{
	return !GetIsAttacking() && !bDashing && !bRepulsion &&	!bHurt && !bDead;
}

inline void ABasePXCharacter::SetBlendPitch(float V)
{
	CurBlendPitch = V;
}


inline void ABasePXCharacter::SetBlendYaw(float V)
{
	DeltaBlendYaw = 0;
	CurBlendYaw = V;
}

inline void ABasePXCharacter::AddBlendYaw(float V)
{
	DeltaBlendYaw = -V;
	CurBlendYaw += V;
}



