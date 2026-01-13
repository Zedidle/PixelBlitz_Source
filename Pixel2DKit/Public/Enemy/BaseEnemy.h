#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "PaperZDCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Character/BasePXCharacter.h"
#include "Components/EnemyAIComponent.h"
#include "Curves/CurveVector.h"
#include "Interfaces/Fight_Interface.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"
#include "Utilitys/PXCustomStruct.h"
#include "Engine/DataTable.h"
#include "Fight/Components/HealthComponent.h"
#include "GAS/PXEnemyASComponent.h"
#include "BaseEnemy.generated.h"


class UAISenseConfig_Sight;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDie, ABaseEnemy*, Enemy);


class UPXEnemyAttributeSet;

USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 HP = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float BodyScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 AttackDamage = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float AttackInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MoveSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MoveAcceleration = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 LookDeterrence = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 AttackComplexity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FVector AttackKnockbackForce = FVector(200, 100, 50);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FVector RepelResistance = FVector(20);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float SightRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float LostEnemyTime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	TMap<FName, int> DropID_Rate;

	// 行动区间范围定义
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FActionFieldDistance ActionFieldDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeakPointRefreshPeriod = 6.0f;
};


USTRUCT(BlueprintType)
struct FActionMove
{
	GENERATED_BODY()

	bool bActionMoving = false;
	float CurTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | ActionMove")
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | ActionMove")
	FVector TargetLocation  = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | ActionMove")
	TObjectPtr<UCurveVector> CurveVector = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | ActionMove")
	float SustainTime = 0.5f;

	// 是否能被打断？
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | ActionMove")
	bool bCabBeInterrupt = true;
};






class UAIPerceptionComponent;
class UEnemyAIComponent;
class UHealthComponent;
class UFightComponent;
class UEnemyDataAsset;
class UAbilityComponent;
class ABasePXCharacter;



UCLASS(Abstract)
class PIXEL2DKIT_API ABaseEnemy : public APaperZDCharacter, public IFight_Interface, public IEnemyAI_Interface, public IBuff_Interface, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> ECC_PlayerPawn; 
	
	UPROPERTY()
	TWeakObjectPtr<ABasePXCharacter> PXCharacter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	UEnemyDataAsset* DataAsset;
	
	void SetLanding(const bool V, const float time = 0.1f);

	FActionMove ActionMove;
	
public:
	ABaseEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	bool IsActionMoving() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAI)
	TObjectPtr<UEnemyAIComponent> EnemyAIComponent;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UHealthComponent> HealthComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UFightComponent> FightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UBuffComponent> BuffComponent;

	// 怪物也可以使用技能与天赋，并获得对应属性
	UPROPERTY()
	FEffectGameplayTags EffectGameplayTags;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
	UFUNCTION(BlueprintCallable)
	void SetActionMove(const FVector& MoveVector, const FName& CurveName, float SustainTime = 0.5f, bool bInterrupt = false, bool bCabBeInterrupt = false);

	UFUNCTION(BlueprintCallable)
	void TryJumpToOtherPlatform(const FVector& StartLocation, const FVector& TargetLocation);
	
	UFUNCTION(BlueprintNativeEvent)
	void Initialize(FName Level);
	
	UFUNCTION(BlueprintNativeEvent)
	void LoadEnemyData(FName Level);

	virtual float GetDefaultHalfHeight() const override;

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadLookDeterrence(int32 Level);
	
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDie OnEnemyDie;
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetPixelCharacter();
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void SetPXCharacter(ABasePXCharacter* Character);

	UFUNCTION()
	void Event_OnPlayerDie();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnSensingPlayer")
	void BP_OnSensingPlayer(AActor* PlayerActor);
	UFUNCTION()
	void OnSensingPlayer(AActor* PlayerActor);
	UFUNCTION()
	void OnLostPlayer(AActor* PlayerActor);
	UFUNCTION()
	void GoPatrol();

	UPROPERTY(BlueprintReadOnly)
	FEnemyData EnemyData;

	// 最大水平跳跃距离，应该取决于自身速度
	float MaxHorizontalDistance = 200;
	
	int32 CurAttackDamage = 10;
	FVector CurAttackRepel = FVector(50,50,100);
	
	UPROPERTY(BlueprintReadWrite)
	float BasicAttackInterval = 2.0f;

	UPROPERTY(BlueprintReadWrite)
	float WeakPointRefreshPeriod = 6.0f;

	UPROPERTY(BlueprintReadOnly);
	float LostEnemyTime = 10.0f;

	UPROPERTY(BlueprintReadOnly)
	int InjuredNum;

	// 巡逻时随机移动的范围, 不应该超过平台的间隔导致空间位置不足，从而强行寻路掉落平台
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	float RandomMoveRange = 200.0f; 
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bDead; 

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bHurt;

	// 是否处于攻击状态，理解为 冷却 即可，同步给行为树黑板控制状态
	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bInAttackState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bInAttackEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bAttackAnimToggle;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bInDefendState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bDefendStart;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bDefendHurt;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bJumping;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	bool bLanding;

	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;

#pragma region Animations
	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetHurt(const bool V, float Duration);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetInAttackState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetInAttackEffect(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetAttackAnimToggle(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetInDefendState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetDefendStart(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetDefendHurt(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Anim")
	void SetJumping(const bool V, const float time = 0.2f);
#pragma endregion

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy | Fight")
	float GetHorizontalDistanceToPlayer() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy | Fight")
	float GetVerticalDistanceToPlayer() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Enemy | Fight")
	void TryAttack();

	UFUNCTION(BlueprintNativeEvent, Category="Enemy | Fight")
	void OnEnemyHPChanged(int32 OldValue, int32 NewValue);

	UFUNCTION(BlueprintNativeEvent, Category="Enemy | Fight")
	void OnHurt(int RemainHP);

	UFUNCTION(Blueprintable, BlueprintPure)
	FVector GetHorizontalDirectionToPlayer();
	
	
#pragma region GAS
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPXEnemyASComponent* AbilitySystem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPXEnemyAttributeSet* AttributeSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> InitAbilitiesToGive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> InitEffects;
	
#pragma endregion



#pragma region Buff_Interface
	virtual void BuffUpdate_Attack_Implementation() override;
	virtual void BuffUpdate_Speed_Implementation() override;
	virtual void BuffUpdate_Sight_Implementation() override;
	
#pragma endregion

	
#pragma region Fight_Interface
	virtual bool GetIsAttacking() override;
	virtual bool GetIsDefending() override;
	virtual bool IsAlive_Implementation() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual bool CanMove_Implementation() override;
	virtual bool CanAttack_Implementation() override;
	virtual bool Dash_Implementation() override;
	virtual void OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce) override;
	virtual void OnBeAttacked_Invulnerable_Implementation() override;
	virtual int DamagePlus_Implementation(int InDamage, AActor* Receiver) override;
	virtual void PowerRepulsion_Implementation(float Power) override;
	virtual int OnDefendingHit_Implementation(int iniDamage) override;
	virtual void OnDefendingHitEffect_Implementation() override;
	virtual void OnDie_Implementation() override;
	virtual void OnAnimDieEnd_Implementation() override;
	virtual void OnRemoteAttackEffect_Implementation() override;
	virtual void OnRemoteAttackEnd_Implementation() override;
	virtual void OnDefendEffectBegin_Implementation() override;
	virtual void OnDefendEffectEnd_Implementation() override;
	virtual void OnAttackEffectBegin_Implementation() override;
	virtual void OnAttackEffectEnd_Implementation() override;
	virtual bool FindEffectGameplayTag_Implementation(const FGameplayTag Tag, float& Result) override;
	virtual APawn* GetPawn_Implementation() override;
	virtual float GetAttackInterval_Implementation() override;
	virtual int GetAttackDamage_Implementation() override;
	virtual FVector GetAttackRepel_Implementation() override;
	virtual void OnAnimVulnerableBegin_Implementation() override;
	virtual void OnAnimVulnerableEnd_Implementation() override;
#pragma endregion
	
#pragma region ActionFields
	// 战斗的行动模式
	virtual void ActionAtPlayerEastNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerEastMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerEastFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerEastRemote_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestRemote_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthRemote_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthRemote_Implementation(float Distance) override;
#pragma endregion ActionFields

	
	// 战斗距离判断, 当前的距离是否适合当前区间的攻击
	virtual bool InAttackRange() override;
	virtual bool InAttackRange_Vertical();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DelayLosePlayer();

	// 默认可行动方位，一般为全方位距离可行，实际行动受设定距离限制与ActionAtPlayerXXXXX接口，需要针对不同怪物部分删除
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight")
	FGameplayTagContainer ActionFieldsCanAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight", meta = (ExposeOnSpawn = true))
	FName EnemyLevel = "1"; 
	
protected:
	virtual void Tick(float DeltaSeconds) override;

	void Tick_KeepFaceToPixelCharacter(float DeltaSeconds);
	void Tick_ActionMove(float DeltaSeconds);
	void Tick_SnapOnPlatform(float DeltaSeconds);
	


	
};

