// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/BasePXCharacter.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "EnemyAIComponent.generated.h"




USTRUCT(BlueprintType)
struct FActionFieldDistance
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceNear = {0, 100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceMid = {100, 300};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceFar = {300, 600};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceRemote = {600, 3000};
};



UCLASS( BlueprintType )
class PIXEL2DKIT_API UEnemyAIComponent : public UActorComponent
{
	GENERATED_BODY()

	FVector PreDirection = FVector::ZeroVector;


	// 阻挡因子，动态改变，该值越大时默认偏转角度越大
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlockValue = 0.0f; 

	// 方向惯性因子
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PreDirValue = 0.7f;


	// 调整怪物的接近欲望，基于阻挡因子的方向调整基值, 该值越大，欲望越低
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlockDirModifyValue = 100;
	
	// 调整怪物的接近欲望，每次寻路成功后，进行一次阻挡因子衰减， 该值越小，欲望越大
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlockValueWeekPercent = 0.7; 

	// 盟友互斥检测半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AllyCheckRadius = 100.f;

	// 盟友互斥因子，建议 0.1 - 2 之间，越大越松散
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AllyRepulsion = 0.5f;
	
public:	
	// Sets default values for this component's properties
	UEnemyAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;












	
	
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	class ABaseEnemy* OwningEnemy = nullptr;

	TMap<FGameplayTag, float> DirDistanceToActionPoint;
	
	// 随时变化的当前目标位置
	FVector CurTargetLocation;

	// 目标中的玩家
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	ABasePXCharacter* PXCharacter;
	
	// 如果 n 秒内都打不到玩家，则回到巡逻状态
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	float AttackPatienceTime = 5;   

	// 最远的攻击距离, 20 - 50 为默认小怪攻击距离；
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	FVector2D AttackRange = {20.0f, 50.0f};  

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy)
	TEnumAsByte<ETraceTypeQuery> EnemyTrace;

	
	// 行动区间范围定义
	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	FActionFieldDistance ActionFieldDistance;

	void SetActionFieldDistance(const FActionFieldDistance& actionFieldDistance);
	
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void SetPixelCharacter(ABasePXCharacter* Character);
	

	/* 向量点积法 弱随机移动到目标位置
	 * TargetLocation - 尝试到达的目标位置
	 * DotDirPerRotate - 每次偏转角度
	 * MaxRotateValue - 最大偏转角度
	 * DefaultDirRotate - 默认的最初角度
	 * MinDirectlyDistance - 低于多少距离时，直接到达
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Camera, meta = (AllowPrivateAccess))
	FVector GetMoveDotDirRandLocation(FVector NewTargetLocation, float DotDirPerRotate = 15.0f,
								float MaxRotateValue = 90, float DefaultDirRotate = 0, float MinDirectlyDistance = 50);

	// 获取在X方向（相对玩家的东西方向）的攻击位置
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	FVector GetAttackLocation();

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	FVector GetNearestActionFieldCanAttackLocation();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	float GetCheckCliffHeight();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	float GetMinDirSwitchDistance();
	
	UFUNCTION(Blueprintable, Blueprintable, Category = "EnemyAI")
	FGameplayTag GetActionFieldByPlayer() const;



		
};
