// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BasePixelCharacter.h"
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
	FVector2D DistanceRemote = {600, 9999};
};



UCLASS( BlueprintType )
class PIXEL2DKIT_API UEnemyAIComponent : public UActorComponent
{
	GENERATED_BODY()

	// 调整近战怪物的 ”进攻欲望“
	float BlockDirModifyValue = 100; // 基于阻挡因子的方向调整基值, 该值越大，欲望越低
	float BlockValueWeekValue = 0.7; // 每次寻路成功后，进行一次阻挡因子衰减， 该值越小，欲望越大

	
	float BlockValue = 0.0f; // 阻挡因子，动态改变，该值越大时默认偏转角度越大


	
public:	
	// Sets default values for this component's properties
	UEnemyAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess))
	ABasePixelCharacter* PixelCharacter;

	// 是否能基于Y方向发起攻击
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess))
	bool CanAttackY = false;	

	// 如果 n 秒内都打不到玩家，则回到巡逻状态
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess))
	float AttackPatienceTime = 5;   

	// 发起攻击时，与目标在X方向的距离
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess))
	float AttackRange_X = 50.0f;  

	// 发起攻击时，与目标在Y方向的距离
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess))
	float AttackRange_Y = 50.0f;  

	// 发起攻击时，与目标在Y方向的距离偏差
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess))
	float AttackRangeOffset_X = 5.0f;  

	// 发起攻击时，与目标在Y方向的距离偏差
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess))
	float AttackRangeOffset_Y = 5.0f;  

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess))
	FActionFieldDistance ActionFieldDistance;
	
	/* 向量点积法 弱随机移动到目标位置
	 * TargetLocation - 目标位置
	 * DotDirPerRotate - 每次偏转角度
	 * MaxRotateValue - 最大偏转角度
	 * DefaultDirRotate - 默认的最初角度
	 * MinDirectlyDistance - 低于多少距离时，直接到达
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Camera, meta = (AllowPrivateAccess))
	FVector GetMoveDotDirRandLocation(const FVector TargetLocation, const float DotDirPerRotate = 15.0f,
								const float MaxRotateValue = 90, const float DefaultDirRotate = 0, const float MinDirectlyDistance = 50);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	bool InAttackRangeX_EnemyAI();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	bool InAttackRangeY_EnemyAI();

	// 获取在X方向（相对玩家的东西方向）的攻击位置
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	FVector GetAttackLocation_EnemyAI();
	FVector GetTargetLocationX();
	FVector GetTargetLocationY();


	// 获取 接近/远离 (-1 / 1)玩家的行动域位置
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	FVector GetActionFieldLocation(const bool bNear = true);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = EnemyAI)
	float GetCheckCliffHeight_EnemyAI();

	UFUNCTION(Blueprintable, Blueprintable, Category = "EnemyAI")
	EActionField GetActionFieldByPlayer() const;
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
