// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Components/EnemyAIComponent.h"

#include "GameplayTagsManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utilitys/SpaceFuncLib.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/TimerSubsystemFuncLib.h"

extern TAutoConsoleVariable<int32> CDebugTraceDuration;

UEnemyAIComponent::UEnemyAIComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UEnemyAIComponent::SetActionFieldDistance(const FActionFieldDistance& actionFieldDistance)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningEnemy)
	
	ActionFieldDistance = actionFieldDistance;
	
	const TArray<FGameplayTag>& ActionFieldTags = OwningEnemy->ActionFieldsCanAttack.GetGameplayTagArray();
	for (const auto& ActionFieldTag : ActionFieldTags)
	{
		FString TagString = ActionFieldTag.ToString();
		if (TagString.Contains("Near"))
		{
			DirDistanceToActionPoint.Add(ActionFieldTag, (ActionFieldDistance.DistanceNear.X + ActionFieldDistance.DistanceNear.Y) / 2);
		}
		else if (TagString.Contains("Mid"))
		{
			DirDistanceToActionPoint.Add(ActionFieldTag, (ActionFieldDistance.DistanceMid.X + ActionFieldDistance.DistanceMid.Y) / 2);
		}
		else if (TagString.Contains("Far"))
		{
			DirDistanceToActionPoint.Add(ActionFieldTag, (ActionFieldDistance.DistanceFar.X + ActionFieldDistance.DistanceFar.Y) / 2);
		}
		else if (TagString.Contains("Remote"))
		{
			DirDistanceToActionPoint.Add(ActionFieldTag, (ActionFieldDistance.DistanceRemote.X + ActionFieldDistance.DistanceRemote.Y) / 2);

		}
	}


	
	
}

void UEnemyAIComponent::SetPixelCharacter(ABasePXCharacter* Character)
{
	PXCharacter = Character;
	if (Character == nullptr) return;

	FName TimerName = FName(GetReadableName() + "_SetPixelCharacter");
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
	[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		if (WeakThis->OwningEnemy)
		{
			WeakThis->OwningEnemy->SetPXCharacter(nullptr);
		}
	}, AttackPatienceTime);
}

FVector UEnemyAIComponent::GetMoveDotDirRandLocation(FVector NewTargetLocation, float DotDirPerRotate, float MaxRotateValue,
                                                     float DefaultDirRotate, float MinDirectlyDistance)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, FVector::ZeroVector);

	FVector OwnerLocation = OwningEnemy->GetActorLocation();

	// 同步高度
	NewTargetLocation.Z = OwnerLocation.Z;

	// 移动向量
	FVector MoveVector = FRotator(0, (FMath::RandBool()? 1 : -1) * (DefaultDirRotate + BlockValue * BlockDirModifyValue), 0)
							.RotateVector(NewTargetLocation - OwnerLocation);

	// 处理范围周围半径范围的怪物互斥逻辑
	if (MinDirectlyDistance > MoveVector.Length())
	{
		BlockValue = 0;
		
		// 判断是否有怪物要去相近的位置，如果相较之下自身里目标位置较远，则后退让位
		TArray<AActor*> ActorsToIgnore = {OwningEnemy};
		TArray<FHitResult> OutHits;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), OwnerLocation, NewTargetLocation, AllyCheckRadius, 
	EnemyTrace, false, ActorsToIgnore, EDrawDebugTrace::None, OutHits,
	true, FLinearColor::Red, FLinearColor::Green, 0.5f);
		
		for (auto& result:OutHits)
		{
			if (!IsValid(result.GetActor())) continue;
			
			ABaseEnemy* Ally = Cast<ABaseEnemy>(result.GetActor());
			if (!IsValid(Ally)) continue;

			UEnemyAIComponent* AllyAI = Ally->GetComponentByClass<UEnemyAIComponent>();
			if (!IsValid(AllyAI)) continue;


			FVector VectorToAlly = Ally->GetActorLocation() - OwnerLocation;
			// 与同盟位置的点乘
			float DotAlly = MoveVector.GetSafeNormal2D().Dot(VectorToAlly.GetSafeNormal2D());
			
			// 如果当前移动方向与检测到的同盟方向不接近，> 60° ，则忽略
			if ( DotAlly < 0.5) continue;

			// 检测到同盟后，产生默认斥力
			NewTargetLocation += AllyRepulsion * (OwnerLocation - Ally->GetActorLocation());


			FVector AllyToTarget = Ally->GetActorLocation() - AllyAI->CurTargetLocation;
			
			// 如果同盟的目标点 与 自身目标点十分接近，且它到目标的距离小于自身到目标的，则再次产生斥力（让路）
			if (AllyAI->CurTargetLocation.Equals(NewTargetLocation, 20) && MoveVector.Size() > AllyToTarget.Size())
			{
				NewTargetLocation += 0.5 * AllyRepulsion * (OwnerLocation - AllyAI->CurTargetLocation);
			}
		}

		CurTargetLocation = NewTargetLocation;
		return CurTargetLocation;
	}

	
	TMap<FVector, int> TargetOffsetMap;
	int totalR = 0;
	float blockR = 0; // 阻挡因子
	int n = -1;
	while (DotDirPerRotate * n <= MaxRotateValue)
	{
		n++;
		float tmpD = blockR * DotDirPerRotate * n / MaxRotateValue;
		float tmpR = FMath::Cos(FMath::DegreesToRadians(DotDirPerRotate * n));

		// 一般来说，越偏移目标点则概率越小，移动距离也越小，但是受到阻挡因子的影响，起始方向也会偏转
		float DistanceScale = tmpR > 0 ? FMath::Max(0.5, tmpR) + tmpD : FMath::Min(-0.5, tmpR) - tmpD;
		
		// 目标点相对起点的位置
		FVector TempTargetOffset = DistanceScale * FRotator(0, (FMath::RandBool() ? 1 : -1) * DotDirPerRotate * n, 0).RotateVector(MoveVector);

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwningEnemy); // 忽略自己

		// 存储碰撞结果
		FHitResult HitResult;

		// 执行射线检测
		bool bHit = UKismetSystemLibrary::LineTraceSingle(
			this, // WorldContextObject
			OwnerLocation, // 起点
			OwnerLocation + TempTargetOffset, // 终点
			TraceTypeQuery3, // 检测通道（例如 Visibility）
			false, // bTraceComplex
			ActorsToIgnore, // 忽略的 Actor
			EDrawDebugTrace::None, // 调试绘制类型
			HitResult, // 碰撞结果
			true // bIgnoreSelf
		);

		// 检查是否命中
		if (bHit && HitResult.GetActor() || USpaceFuncLib::CheckCliffProcess(OwnerLocation, OwnerLocation + TempTargetOffset, GetCheckCliffHeight()))
		{
			blockR += DistanceScale;
			continue;
		}

		float tmpBlockAvg = blockR * (MaxRotateValue - DotDirPerRotate * n) / MaxRotateValue;
		tmpR = (DistanceScale + tmpBlockAvg ) * 100 + 400;
		totalR += tmpR;
		TargetOffsetMap.FindOrAdd(TempTargetOffset) = tmpR;
		
		blockR -= tmpBlockAvg;
	}
	

	TArray<FVector> values;
	TargetOffsetMap.GetKeys(values);
	int tmpV = 0;
	int randV = FMath::RandRange(0, totalR);
	for (int i = 0; i < values.Num(); i++)
	{
		tmpV += TargetOffsetMap[values[i]];
		if (tmpV >= randV)
		{
			BlockValue *= BlockValueWeekPercent;
			FVector CurDir = values[i].GetSafeNormal2D();
			if (!PreDirection.IsZero())
			{
				CurDir = UKismetMathLibrary::VLerp(CurDir, PreDirection, PreDirValue);
			}
			
			PreDirection = CurDir;
			return values[i].Size() * CurDir + OwnerLocation;
		}
	}

	BlockValue += FMath::Abs(blockR);
	PreDirection = FVector::ZeroVector;
	return OwnerLocation;
}

FVector UEnemyAIComponent::GetAttackLocation()
{
	if (!IsValid(OwningEnemy)) return FVector::Zero();
	if (!IsValid( PXCharacter)) return FVector::Zero();	

	FVector Dir = (OwningEnemy->GetActorLocation() - PXCharacter->GetActorLocation()).GetSafeNormal2D();
	if (OwningEnemy->GetDistanceToPlayer() < AttackRange.X)
	{
		return PXCharacter->GetActorLocation() + (AttackRange.X + 5) * Dir;
	}

	if (OwningEnemy->GetDistanceToPlayer() > AttackRange.Y)
	{
		return PXCharacter->GetActorLocation() + (AttackRange.Y - 5) * Dir;
	}

	return OwningEnemy->GetActorLocation();
}


FVector UEnemyAIComponent::GetNearestActionFieldCanAttackLocation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, FVector::ZeroVector)

	FVector EnemyLocation = OwningEnemy->GetActorLocation();
	FGameplayTag GameplayTag = GetActionFieldByPlayer();
	if (!GameplayTag.IsValid()) return EnemyLocation;

	FVector PlayerLocation = PXCharacter->GetActorLocation();
	FVector EastDir = PXCharacter->GetRightVectorWithBlendYaw();

	float CurDistance = MAX_FLOAT;
	FVector Result = EnemyLocation;
	FVector TmpPoint;
	
	for (auto& DirDistance: DirDistanceToActionPoint)
	{
		FString TagString = DirDistance.Key.ToString();
		if (TagString.Contains("East"))
		{
			TmpPoint = PlayerLocation + EastDir * DirDistance.Value;
		}
		else if (TagString.Contains("North"))
		{
			FVector NorthDir = FRotator(0, 90, 0).RotateVector(EastDir);
			TmpPoint = PlayerLocation + NorthDir * DirDistance.Value;
		}
		else if (TagString.Contains("West"))
		{
			FVector WestDir = FRotator(0, 180, 0).RotateVector(EastDir);
			TmpPoint = PlayerLocation + WestDir * DirDistance.Value;
		}
		else if (TagString.Contains("South"))
		{
			FVector SouthDir = FRotator(0, -90, 0).RotateVector(EastDir);
			TmpPoint = PlayerLocation + SouthDir * DirDistance.Value;
		}
		
		float TmpDistance = FVector::Distance(TmpPoint, EnemyLocation);
		if (CurDistance > TmpDistance)
		{
			CurDistance = TmpDistance;
			Result = TmpPoint;
		}
	}
	
	return GetMoveDotDirRandLocation(Result);
}


float UEnemyAIComponent::GetCheckCliffHeight()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, 50)
	
	UCapsuleComponent* CapsuleComponent = OwningEnemy->GetComponentByClass<UCapsuleComponent>();
	UCharacterMovementComponent* CharacterMovementComponent = OwningEnemy->GetComponentByClass<UCharacterMovementComponent>();
	if (!CapsuleComponent || !CharacterMovementComponent) return 0;
	
	float ScaleZ = OwningEnemy->GetActorScale3D().Z;
	return ScaleZ * (CharacterMovementComponent->MaxStepHeight + CapsuleComponent->GetUnscaledCapsuleHalfHeight());
}

float UEnemyAIComponent::GetMinDirSwitchDistance()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, 5)
	
	UCapsuleComponent* CapsuleComponent = OwningEnemy->GetComponentByClass<UCapsuleComponent>();
	UCharacterMovementComponent* CharacterMovementComponent = OwningEnemy->GetComponentByClass<UCharacterMovementComponent>();
	if (!CapsuleComponent || !CharacterMovementComponent) return 0;
	
	float ScaleZ = OwningEnemy->GetActorScale3D().X;
	return FMath::Max(ScaleZ * CapsuleComponent->GetUnscaledCapsuleRadius() - 5 , 1);
}

FGameplayTag UEnemyAIComponent::GetActionFieldByPlayer() const
{
	if (!IsValid(PXCharacter) || !OwningEnemy) return FGameplayTag();
	
	EWorldDirection Dir = USpaceFuncLib::ActorAtActorWorldDirection(OwningEnemy, PXCharacter, PXCharacter->CurBlendYaw);
	float Distance = (OwningEnemy->GetActorLocation() - PXCharacter->GetActorLocation()).Size2D();
	
	if (Dir == East)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.East.Near")));
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.East.Mid")));
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.East.Far")));
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.East.Remote")));
		}
	}
	else if (Dir == West)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.West.Near")));
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.West.Mid")));
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.West.Far")));
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.West.Remote")));
		}
	}
	else if (Dir == North)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.North.Near")));
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.North.Mid")));
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.North.Far")));
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.North.Remote")));
		}
	}
	else if (Dir == South)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.South.Near")));
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.South.Mid")));
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.South.Far")));
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("ActionField.South.Remote")));
		}
	}

	return FGameplayTag();
}


// Called when the game starts
void UEnemyAIComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningEnemy = Cast<ABaseEnemy>(GetOwner());
	
}


// Called every frame
void UEnemyAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

