#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/BasePXCharacter.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "EnemyAIComponent.generated.h"

class ABaseEnemy;

UENUM(BlueprintType)
enum class EPlayerMovementType : uint8
{
	None UMETA(DisplayName = "None"),
	Approach UMETA(DisplayName = "Approach"),
	ApproachQuick UMETA(DisplayName = "ApproachQuick"),
	Avoidance UMETA(DisplayName = "Avoidance"),
	AvoidanceQuick UMETA(DisplayName = "AvoidanceQuick"),
};

USTRUCT(BlueprintType)
struct FActionFieldDistance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceNear = {0, 60};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceMid = {60, 240};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceFar = {240, 600};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceRemote = {600, 2000};
};

UCLASS(BlueprintType)
class PIXEL2DKIT_API UEnemyAIComponent : public UActorComponent
{
	GENERATED_BODY()

	float PlayerHigher = 0.0f;
	EPlayerMovementType CurPlayerMovementType = EPlayerMovementType::None;

	TArray<FVector> PlayerPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MaxPlayerPathsNum = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 DistanceNextPathPointNear = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CheckPlayerLocationInterval = 0.5f;

	FVector PreDirection = FVector::ZeroVector;
	bool PreBlockYawDirection = false;
	float BlockValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PreDirectionPercent = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxBlockYawModify = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlockDirModifyValue = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlockValueWeekPercent = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AllyCheckRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AllyRepulsion = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float OnPlayerAttackStart_DodgeRate = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float OnPlayerAttackStart_DodgeDistance = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float OnPlayerAttackHolding_HesitationRate = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float OnCrowded_HesitationRate = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 OnCrowded_HesitationMinAllies = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float HesitationDuration = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float HesitationDodgeRateBonus = 0.2f;

	// Reserved for the future "contest skill" system. Not used in calculations yet.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float HesitationContestSkillBonus = 0.0f;

public:
	UEnemyAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	ABaseEnemy* OwningEnemy = nullptr;

	TMap<FGameplayTag, float> DirDistanceToActionPoint;
	FVector CurTargetLocation;

	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	TWeakObjectPtr<ABasePXCharacter> PXCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	TArray<FVector2D> AttackRange = {{20.0f, 60.0f}};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy)
	TEnumAsByte<ETraceTypeQuery> EnemyTrace;

	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	FActionFieldDistance ActionFieldDistance;

	void SetActionFieldDistance(const FActionFieldDistance& ActionFieldDistanceIn);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetPXCharacter(ABasePXCharacter* Character);

	void UpdatePlayerPaths();
	void LostPlayer();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Camera, meta = (AllowPrivateAccess))
	FVector GetMoveDotDirRandLocation(
		FVector NewTargetLocation,
		float DotDirPerRotate = 10.0f,
		float MaxRotateValue = 60.0f,
		float DefaultDirRotate = 0.0f,
		float MinDirectlyDistance = 50.0f);

	bool MoveCheckAllies(FVector& Result, FVector NewTargetLocation, float MinDirectlyDistance = 50.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	FVector GetNearestActionFieldCanAttackLocation();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	float GetCheckCliffHeight();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	float GetMinDirSwitchDistance();

	UFUNCTION(Blueprintable, Blueprintable, Category = "EnemyAI")
	FGameplayTag GetActionFieldByPlayer() const;

	bool GetPlayerPathPoint(FVector& Point);

	UFUNCTION()
	void OnPlayerAttackStart(EAttackType Type, FVector Direction);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EnemyAI")
	float GetCurrentPlayerAttackStartDodgeRate() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EnemyAI")
	float GetHesitationContestSkillBonus() const;

	UFUNCTION(BlueprintCallable, Category = "EnemyAI")
	bool TryEnterHesitationState();

	bool TryEnterHesitationStateByRate(float EnterRate);
	bool TryEnterHesitationStateByCrowd(const FVector& NewTargetLocation);
	int32 CountAlliesOnPath(const FVector& NewTargetLocation) const;
	bool HasDirectSightToPlayer() const;

	UFUNCTION()
	void Event_CheckPlayerMovement();
};
