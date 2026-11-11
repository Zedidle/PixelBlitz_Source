#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "Abilities/GameplayAbility.h"
#include "Fight/Components/StateComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/Interact_Interface.h"

class UBasePlayerStatusWidget;

#include "BasePXCharacter.generated.h"

class ABaseInteractableItem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDie);

// 发起攻击的监听事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerAttackStart, EAttackType, AttackType, FVector, AttackDirection);

class ABaseWeapon;
class ABasePlatform;

class UInputAction;
class UAbilityComponent;
class UBuffComponent;
class UTalentComponent;

class UPXASComponent;
class UStateComponent;
class UFightComponent;

class UCameraComponent;
class USpringArmComponent;
class UActorInteractorComponentOverlap;

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
class PIXELBLITZ_API ABasePXCharacter : public APaperZDCharacter, public IFight_Interface, public IInteract_Interface,
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
	TWeakObjectPtr<ABasePlatform> JumpTakeoffPlatform;

	ABasePlatform* ResolvePlatformActor(const FHitResult* Hit = nullptr) const;

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

#pragma region 对话镜头
	// 对话进行时把镜头锚点移向玩家与 NPC 之间，并拉近视距
	// 由 APXPlayerState 在对话 UI 创建/销毁时驱动（SetDialogueCameraFocus / ClearDialogueCameraFocus）

	TWeakObjectPtr<AActor> DialogueCameraTargetActor;

	/** 镜头锚点在 玩家→NPC 连线上的位置比例：0=玩家位置，1=NPC 位置 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Dialogue")
	float DialogueCameraFocusFactor = 0.6f;

	/** 对话时的视距缩放系数（乘在 CurSpringArmLength 上，1=不缩放） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Dialogue")
	float DialogueSpringArmLengthFactor = 0.55f;

	/** 当前实际生效的视距系数，逐帧向目标值平滑 */
	float CurDialogueSpringArmFactor = 1.f;

	UFUNCTION(BlueprintCallable, Category = "Camera|Dialogue")
	void SetDialogueCameraFocus(AActor* TargetNPC);

	UFUNCTION(BlueprintCallable, Category = "Camera|Dialogue")
	void ClearDialogueCameraFocus();
#pragma endregion
	
	void SetLanding(const bool V, const float time = 0.1f);


	void OnHPChanged(int32 OldValue, int32 NewValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckNearDeath();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Revive();

#pragma region GAS


	UPROPERTY(BlueprintReadOnly)
	UPXASComponent* CachedASC;

	// 自带 ASC：不依赖 PlayerState 的角色（对话切换 / NPC 展示）用它做属性容器，
	// 否则这类角色 GetAbilitySystemComponent() 返回空，LoadData 与蓝图全部失效
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UPXASComponent> OwnedASC;

	UPROPERTY()
	TObjectPtr<class UPXAttributeSet> OwnedAttributeSet;
		
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

	// ASC 依赖 PlayerState，Possess / PlayerState 复制到达后需要通知 StateComponent 重新解析
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

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

	// 处于攻击前摇
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool bAttackStartup;
	
	// 用于管理攻击效果生效，暂不用于动画控制
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool bInAttackEffect;
	
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool bInAttackSkill;
	
	// 冲刺进入
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bDashing;

	// 冲刺生效期间
	UPROPERTY(BlueprintReadWrite, Category = Animation)
	bool InSkillEffect;

	// 攻击蓄力中
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackHolding;

	// 蓄力攻击释放
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	bool bAttackFire;

#pragma region 瞄准写权（漂移偏转 / BlendYaw / 瞄准 三者争抢 Sprite 旋转的仲裁）
	// 瞄准方（蓄力箭蓝图）置 true 后接管 Sprite 旋转：漂移偏转暂停、BlendYaw 增量暂存
	UPROPERTY(BlueprintReadOnly, Category = "Attack|Aim")
	bool bAimOverride = false;

	// 瞄准期间 Sprite 应取的世界朝向 Yaw，由瞄准方每帧更新（通常 = 指向锁定目标）
	UPROPERTY(BlueprintReadWrite, Category = "Attack|Aim")
	float AimOverrideYaw = 0.f;

	// 瞄准期间被暂存的视角横移增量，松弓后归还给 CurBlendYaw
	float PendingBlendYaw = 0.f;

	// 松弓后 Sprite 从瞄准朝向滑回漂移朝向的残差
	float AimRemainYaw = 0.f;

	// 最近一次漂移偏转实际断言的基准朝向（静止松弓滑回时作终点）
	float LastSpriteBaseYaw = 0.f;

	// 漂移偏转目标朝向计算（Tick 与松弓残差共用同一套判定，保证滑回终点=漂移本应断言的朝向）
	float CalcDriftTargetYaw(bool& bOutAsserted);

	// 松弓后视角增量归还 / 朝向残差滑回的时间常数（指数平滑）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Aim", meta = (ClampMin = "0.01", UIMin = "0.01"))
	float AimReleaseBlendTime = 0.15f;

	UFUNCTION(BlueprintCallable, Category = "Attack|Aim")
	void SetAimOverride(bool V);
#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Holding", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AttackHoldMoveSpeedSub = 50.0f;

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
	FVector2D ViewPitchRange = FVector2D(-50, 5);
	
	
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
	FVector GetDashSpeed();
	
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
	void SetInAttackEffect(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetAttackStartup(const bool V);
	
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
	virtual void OnSkillEffectBegin_Implementation() override;
	virtual void OnSkillEffectEnd_Implementation() override;
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

	/**
	 * 控制台调试命令（PIE 里按 ` 输入 DebugInteraction）：
	 * 打印自身 Interactor 与场上所有 ABaseNPC Interactable 的交互快照，
	 * 用于定位"换人后无法交互"卡在哪一环（状态/通道/占用/对话门控）。
	 */
	UFUNCTION(Exec, Category="PX|Debug")
	void DebugInteraction();
	
#pragma endregion

	bool IsProjectileNormalAttack() const;
	float GetAttackHoldMoveSpeedSub() const;
	void RefreshAttackHoldingMoveSpeed();
	void SetAnimPlayerPlaybackReversed(bool bReversed) const;

	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseInteractableItem>> InteractableItems;
	void AddInteractableItem(ABaseInteractableItem* Item);
	void RemoveInteractableItem(ABaseInteractableItem* Item);

	/**
	 * Mountea 交互发起方（Overlap 版）：靠玩家胶囊体和 NPC 的 Box 重叠发现目标，
	 * 与 B_InteractionExample_Character 同一套做法。
	 * 胶囊的注册在 BeginPlay 里（必须早于 Super::BeginPlay，见 .cpp 注释），
	 * 唯一必须在蓝图里设的是 Safety Tracing Mode = None（构造默认从世界原点打射线，必失败）。
	 * InputAction.Interact 已绑到 Interact()，按键时把意图转给它即可。
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	TObjectPtr<UActorInteractorComponentOverlap> InteractorComponent;


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
	float GetDashSpeedValue();
	
	
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
	// 瞄准期间视角锁死：横移增量暂存，松弓后由 Tick_SpriteRotation 归还
	if (bAimOverride)
	{
		PendingBlendYaw += V;
		return;
	}
	DeltaBlendYaw = -V;
	CurBlendYaw += V;
}



