// Fill out your copyright notice in the Description page of Project Settings.

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
	None UMETA(DisplayName = "无"),
	Approach UMETA(DisplayName = "接近"),
	ApproachQuick UMETA(DisplayName = "快速接近"),
	Avoidance UMETA(DisplayName = "远离"),
	AvoidanceQuick UMETA(DisplayName = "快速远离"),
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



UCLASS( BlueprintType )
class PIXEL2DKIT_API UEnemyAIComponent : public UActorComponent
{
	GENERATED_BODY()

	// 玩家相对于自身的高度
	float PlayerHigher = 0;
	EPlayerMovementType CurPlayerMovementType = EPlayerMovementType::None;
	
	// 记录玩家移动路径
	TArray<FVector> PlayerPaths;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int MaxPlayerPathsNum = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int DistanceNextPathPointNear = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CheckPlayerLocationInterval = 0.5f;
	
	FVector PreDirection = FVector::ZeroVector;
	bool PreBlockYawDirection = false;

	// 阻挡因子，动态改变，该值越大时默认偏转角度越大
	float BlockValue = 0.0f; 

	// 方向惯性因子, 每次移动保留上一次移动方向的比例
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PreDirectionPercent = 0.7f;

	// 当被障碍物阻挡时，侧向移动的最大角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxBlockYawModify = 60;
	
	// 调整怪物的接近欲望，基于阻挡因子的方向调整基值, 该值越大，欲望越低
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlockDirModifyValue = 60;
	
	// 调整怪物的接近欲望，每次寻路成功后，进行一次阻挡因子衰减， 该值越小，欲望越大
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlockValueWeekPercent = 0.2; 

	// 盟友互斥检测半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AllyCheckRadius = 100.f;

	// 盟友互斥因子，建议 0.1 - 2 之间，越大越松散
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AllyRepulsion = 0.5f;







	// 监听玩家后的反应

	// 闪避数值，需要配置
	// 概率
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float OnPlayerAttackStart_DodgeRate = 0.6f;

	// 距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float OnPlayerAttackStart_DodgeDistance = 50.0f;




	
public:	
	// Sets default values for this component's properties
	UEnemyAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void DestroyComponent(bool bPromoteChildren = false);
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	ABaseEnemy* OwningEnemy = nullptr;

	TMap<FGameplayTag, float> DirDistanceToActionPoint;
	
	// 随时变化的当前目标位置
	FVector CurTargetLocation;

	// 目标中的玩家
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	TWeakObjectPtr<ABasePXCharacter> PXCharacter;
	
	// 最远的攻击距离, 20 - 50 为默认近战小怪攻击距离；
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	TArray<FVector2D> AttackRange = {{20.0f, 60.0f}};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy)
	TEnumAsByte<ETraceTypeQuery> EnemyTrace;

	
	// 行动区间范围定义
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	FActionFieldDistance ActionFieldDistance;

	void SetActionFieldDistance(const FActionFieldDistance& actionFieldDistance);
	
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void SetPXCharacter(ABasePXCharacter* Character);

	void UpdatePlayerPaths();

	void LostPlayer();

	/* 向量点积法 弱随机移动到目标位置
	 * TargetLocation - 尝试到达的目标位置
	 * DotDirPerRotate - 每次偏转角度
	 * MaxRotateValue - 最大偏转角度
	 * DefaultDirRotate - 默认的最初角度
	 * MinDirectlyDistance - 低于多少距离时，直接到达, 不宜太远，会掉落平台
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Camera, meta = (AllowPrivateAccess))
	FVector GetMoveDotDirRandLocation(FVector NewTargetLocation, float DotDirPerRotate = 10.0f,
								float MaxRotateValue = 60, float DefaultDirRotate = 0, float MinDirectlyDistance = 50);

	bool MoveCheckAllies(FVector& Result, FVector NewTargetLocation, float MinDirectlyDistance = 50);

	
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

	// 监听玩家运动状态
	UFUNCTION()
	void Event_CheckPlayerMovement();
	
};
