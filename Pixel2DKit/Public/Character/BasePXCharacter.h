// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/Buff_Interface.h"
#include "Item/Weapon/BaseWeapon.h"
#include "UI/Player/BasePlayerStatusWidget.h"
#include "BasePXCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDie);

// 发起攻击的监听事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAttackStart);


USTRUCT(BlueprintType)
struct FWeatherEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightDistancePercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HPEffectPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeedEffectPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EPConsumePercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamagePlusPercent;
};




class UInputAction;
class UAbilityComponent;
class UBuffComponent;
class UTalentComponent;

class UPXASComponent;
class UHealthComponent;
class UEnergyComponent;
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
};



UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API ABasePXCharacter : public APaperZDCharacter, public IFight_Interface, public IBuff_Interface,
										public IAbilitySystemInterface, public IInteract_Interface
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void LoadData();
	
	float FallingStartTime;
	bool PreFrameFalling = false;
	bool PreSpriteLeft = false;
	FVector CameraOffsetForBulletTime;
	float JumpStartTime;


	bool bViewYawChangingByPlayerControl = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBasePlayerStatusWidget* PlayerStatusWidget;
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UPXCharacterDataAsset* DataAsset;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDie OnPlayerDie;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnPlayerAttackStart OnPlayerAttackStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VelocityRepelFactor = 1.0f;

	FTimerHandle ScaleTimerHandle;
	FTimerHandle AttackHitTimerHandle;
	int AttackHitComboNum = 0;



	float ScaleLerpValue = 0;
	float ScaleCurValue = 1.0f;
	FVector InitScale;


#pragma region 临时存储数据，后续看存到哪

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int PerfectDodgeTimes = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Talent)
	int HereReviveTimes = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = Achievement)
	bool HasOneHealthPoint = false;

	UPROPERTY(BlueprintReadWrite, Category = Achievement)
	int AirDashTimes = 0;
	
#pragma endregion 
	


	
	virtual void Tick_SaveFallingStartTime();
	virtual void Tick_SpriteRotation(); // 漂移式偏转
	virtual void Tick_SpringArmMotivation();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraOffsetSpeedFactor = 0.04;
	
	FVector CurCameraOffset;
	TMap<FName, FVector> CameraOffsetMap;
	UFUNCTION(BlueprintCallable)
	void AddCameraOffset(FName OffsetName, FVector Offset);
	
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
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY()
	TWeakObjectPtr<class UPXAttributeSet> AttributeSetBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> InitAbilitiesToGive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> InitEffects;

#pragma endregion
	
	ABasePXCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GAS)
	UPXAttributeSet* GetAttributeSet() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UHealthComponent> HealthComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UEnergyComponent> EnergyComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UFightComponent> FightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UAbilityComponent> AbilityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UBuffComponent> BuffComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UTalentComponent> TalentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UCameraComponent> Camera;
	
#pragma region EffectGameplayTags
	UPROPERTY()
	FEffectGameplayTags EffectGameplayTags;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Fight)
	bool HasEffectGameplayTag(const FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Fight)
	float GetEffectGameplayTag(const FGameplayTag Tag) const;
#pragma endregion
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	FVector FrontViewFactor = {0.5, 0.5, 0.1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	FVector FollowViewFactor = {-0.2, -0.2, -0.1};
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

	UPROPERTY(BlueprintReadOnly, Category = Fight)
	ABaseWeapon* Weapon;
	
	UFUNCTION(BlueprintCallable, Category = Fight)
	void LoadWeapon(TSubclassOf<ABaseWeapon> WeaponClass = nullptr);
	
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float MaxWalkSpeed = 200.f;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float MaxAcceleration = 400.f;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float BasicAirControl = 1.0f;
	
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float JumpZVelocity = 250.0f;
	
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float GravityScale = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BasicDashSpeed = 400.0f;
	
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

	// Right就是东侧
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetRightVectorWithBlendYaw();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = View)
	FVector GetVectorFaceToCamera();
	

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	int CurJumpCount = 0;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	int32 CurMaxJumpCount = 2;
	UPROPERTY(BlueprintReadonly, Category = Movement)
	int32 BasicMaxJumpCount = 2;
	UPROPERTY(BlueprintReadonly, Category = Movement)
	float BasicJumpMaxHoldTime = 0.3f;

	
	UPROPERTY(BlueprintReadonly, Category = Movement)
	float BasicAttackInterval = 1.0f;
	
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

	UPROPERTY(BlueprintReadWrite, Category = Fight)
	float BasicRepelValue;
	
	UPROPERTY(BlueprintReadWrite, Category = Fight)
	int	BasicAttackValue = 10;

	
	
	UFUNCTION(BlueprintCallable, Category = View)
	void SetBlendPitch(float V);
	UFUNCTION(BlueprintCallable, Category = View)
	void SetBlendYaw(float V);
	UFUNCTION(BlueprintCallable, Category = View)
	void AddBlendYaw(float V);
	
	void AddViewYaw(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = View)
	void AddViewYaw(float V, bool bPlayerControl);
	UFUNCTION(BlueprintCallable, Category = View)
	void AddViewPitch(const FInputActionValue& Value);

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

	UFUNCTION(BlueprintCallable, Category = Anim)
	void EndNormalAttack();
#pragma endregion

	
#pragma region IFight_Interface
	virtual bool GetIsAttacking() override;
	virtual bool GetIsDefending() override;
	virtual bool CanAttack_Implementation() override;
	virtual bool IsAlive_Implementation() override;
	virtual FGameplayTagContainer GetOwnCamp_Implementation() override;
	virtual FGameplayTagContainer GetEnemyCamp_Implementation() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual void OnAttackHiting_Implementation() override;
	virtual void PowerRepulsion_Implementation(float Power) override;
	virtual void OnBeAttacked_Invulnerable_Implementation() override;
	virtual void OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage) override;
	virtual int DamagePlus_Implementation(int InDamage, AActor* Receiver) override;
	virtual int OnDefendingHit_Implementation(int inValue) override;
	virtual void OnAttackHolding_Implementation() override;
	virtual void OnDefendingHitEffect_Implementation() override;
	virtual void OnAnimVulnerableBegin_Implementation() override;
	virtual void OnAnimVulnerableEnd_Implementation() override;
	virtual void OnAttackEffect_Implementation() override;
	virtual void OnAttackEffectBegin_Implementation() override;
	virtual void OnAttackEffectEnd_Implementation() override;
	virtual void OnDashEffectBegin_Implementation() override;
	virtual void OnDashEffectEnd_Implementation() override;
	virtual UAbilityComponent* GetAbilityComponent_Implementation() override;
	virtual bool FindEffectGameplayTag_Implementation(const FGameplayTag Tag, float& Result) override;
	virtual APawn* GetPawn_Implementation() override;
	virtual float GetAttackInterval_Implementation() override;
	virtual int GetAttackDamage_Implementation() override;
	virtual FVector GetAttackRepel_Implementation() override;
	virtual void OnDie_Implementation() override;
#pragma endregion


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttackRelease();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttackHoldingRelease();
	
#pragma region IBuff_Interface
	virtual void BuffEffect_Speed_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime = 9999) override;
	virtual void BuffUpdate_Speed_Implementation() override;
	virtual void BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime = 9999) override;
	virtual void BuffUpdate_Attack_Implementation() override;
	virtual void BuffEffect_Sight_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime = 9999) override;
	virtual void BuffUpdate_Sight_Implementation() override;
	virtual int32 Buff_CalInitDamage_Implementation(int32 InDamage) override;
	virtual void AddBuff_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor, bool Permanent) override;
	virtual void RemoveBuff_Implementation(FGameplayTag Tag, bool OnlySelf = true) override;
	virtual float GetShortSightResistancePercent_Implementation() override;
	virtual float GetSlowDownResistancePercent_Implementation() override;
#pragma endregion
	
	

#pragma region Input
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;
	void MoveX(const FInputActionValue& Value);
	void MoveY(const FInputActionValue& Value);
	void Move2D(const FInputActionValue& Value);
	void TryToAttack();
	void AttackRelease();
	void TryToJump();
	void JumpRelease();
	
	
#pragma endregion
	
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



