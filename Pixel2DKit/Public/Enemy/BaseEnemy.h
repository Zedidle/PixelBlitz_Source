#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"
#include "Utilitys/PXCustomStruct.h"
#include "Engine/DataTable.h"
#include "GAS/PXEnemyASComponent.h"
#include "BaseEnemy.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, ABaseEnemy*, Enemy);


class UPXEnemyAttributeSet;

USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 HP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float BodyScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float AttackInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MoveAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 LookDeterrence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int32 AttackComplexity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FVector AttackKnockbackForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FVector RepelResistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float LostEnemyTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FDrop Drop;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FVector2D AttackRange;
};




class UPawnSensingComponent;
class UEnemyAIComponent;
class UHealthComponent;
class UFightComponent;
class UEnemyDataAsset;
class UAbilityComponent;
class ABasePXCharacter;

UCLASS()
class PIXEL2DKIT_API ABaseEnemy : public APaperZDCharacter, public IFight_Interface, public IEnemyAI_Interface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	UPROPERTY()
	ABasePXCharacter* PixelCharacter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	UEnemyDataAsset* DataAsset;

	
	void SetLanding(const bool V, const float time = 0.1f);

	
public:
	ABaseEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UAbilityComponent> AbilityComponent;






	
	UFUNCTION(BlueprintNativeEvent)
	void Initialize(FName Level);
	void Initialize_Implementation(FName Level);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintNativeEvent)
	void LoadEnemyData(FName Level);
	void LoadEnemyData_Implementation(FName Level);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadLookDeterrence(int32 Level);
	void LoadLookDeterrence_Implementation(int32 Level);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnDie();
	void OnDie_Implementation();

	UPROPERTY(BlueprintAssignable)
	FOnEnemyDeath OnEnemyDeath;
	
	UPROPERTY(BlueprintReadOnly, Category = EnemyAI)
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAI)
	TObjectPtr<UEnemyAIComponent> EnemyAIComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight)
	TObjectPtr<UHealthComponent> HealthComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fight, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFightComponent> FightComp;

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetPixelCharacter();
	UFUNCTION(BlueprintCallable, Category="Enemy")
	bool SetPixelCharacter(AActor* Character);

	UPROPERTY(BlueprintReadOnly)
	FEnemyData EnemyData;
	
	UPROPERTY()
	FDrop DropData;

	UPROPERTY(BlueprintReadOnly)
	int32 BasicAttackDamage = 10;

	UPROPERTY(BlueprintReadWrite)
	int32 CurAttackDamage = 10;
	
	UPROPERTY(BlueprintReadWrite)
	float AttackInterval = 2.0f;

	UPROPERTY()
	float BasicMoveSpeed = 100.0f;

	UPROPERTY(BlueprintReadOnly);
	float LostEnemyTime = 5.0f;
	
	
	
	// 基础攻击击退力度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	FVector BasicAttackRepel = FVector(50,50,100); 

	// 基础攻击击退力度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	FVector CurAttackRepel = FVector(50,50,100); 

	// 巡逻时随机移动的范围, 不应该超过平台的间隔导致空间位置不足，从而强行寻路掉落平台
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	float RandomMoveRange = 100.0f; 
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bDead; 

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bHurt;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInAttackState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInAttackEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bAttackAnimToggle;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInDefendState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bDefendStart;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bDefendHurt;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bJumping;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bLanding;

	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;

#pragma region Animations
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetHurt(const bool V, const float duration);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInAttackState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInAttackEffect(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetAttackAnimToggle(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInDefendState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDefendStart(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDefendHurt(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetJumping(const bool V, const float time = 0.2f);
#pragma endregion

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy | Fight")
	float GetDistanceToPlayer() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
	void TryAttack();




#pragma region GAS
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPXEnemyASComponent* AbilitySystem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPXEnemyAttributeSet* AttributeSet = nullptr;
	
	
#pragma endregion





	
#pragma region Fight_Interface
	virtual bool GetIsAttacking() override;
	virtual bool GetIsDefending() override;
	virtual bool IsAlive_Implementation() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual float GetRandomMoveRange_Implementation() override;
	virtual bool CanMove_Implementation() override;
	virtual bool CanAttack_Implementation() override;
	virtual bool Dash_Implementation() override;
	virtual void OnBeAttacked_Invulnerable_Implementation() override;
	virtual int DamagePlus_Implementation(int inValue, AActor* ActorDamaged) override;
	virtual int OnDefendingHit_Implementation(int iniDamage) override;
	virtual void OnDieEnd_Implementation() override;
	virtual void OnRemoteAttackEffect_Implementation() override;
	virtual void OnRemoteAttackEnd_Implementation() override;
	virtual void OnDefendEffectBegin_Implementation() override;
	virtual void OnDefendEffectEnd_Implementation() override;
	virtual void OnAttackEffectBegin_Implementation() override;
	virtual void OnAttackEffectEnd_Implementation() override;
	virtual UAbilityComponent* GetAbilityComponent_Implementation() override;
	virtual APawn* GetPawn_Implementation() override;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight")
	FGameplayTagContainer ActionFieldsCanAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight", meta = (ExposeOnSpawn = true))
	FName EnemyLevel = "1"; 
	
protected:
	void Tick_KeepFaceToPixelCharacter(float DeltaSeconds);

	virtual void Tick(float DeltaSeconds) override;

	
};

