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

FVector UEnemyAIComponent::GetMoveDotDirRandLocation(FVector TargetLocation, float DotDirPerRotate, float MaxRotateValue,
                                                     float DefaultDirRotate, const float MinDirectlyDistance)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, FVector::ZeroVector);

	FVector OwnerLocation = OwningEnemy->GetActorLocation();
	TargetLocation.Z = OwnerLocation.Z; // 涉及到转向，避免Owner太高转到半空中导致无效位置
	FVector DefaultDirVector = FRotator(0, (FMath::RandBool()? 1 : -1) * (DefaultDirRotate + BlockValue * BlockDirModifyValue), 0)
							.RotateVector(TargetLocation - OwnerLocation);

	
	if (MinDirectlyDistance > DefaultDirVector.Length())
	{
		BlockValue = 0;
		FVector Location = OwnerLocation + DefaultDirVector;
		// 判断是否有怪物要去相近的位置，如果相较之下自身里目标位置较远，则后退让位
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwningEnemy); // 忽略自己
		TArray<FHitResult> OutHits;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), OwnerLocation, Location, 100, 
	EnemyTrace, false, ActorsToIgnore, EDrawDebugTrace::None, OutHits,
	true, FLinearColor::Red, FLinearColor::Green, 0.5f);
		
		for (auto& result:OutHits)
		{
			if (!IsValid(result.GetActor())) continue;
			if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(result.GetActor()))
			{
				if (DefaultDirVector.GetSafeNormal2D().Dot((Enemy->GetActorLocation() - OwnerLocation).GetSafeNormal2D()) < 0.8) continue;
				
				if (UEnemyAIComponent* EnemyAIComp = Enemy->GetComponentByClass<UEnemyAIComponent>())
				{
					if (EnemyAIComp->CurTargetLocation.Equals(Location, 10))
					{
						if (DefaultDirVector.Size() > (Enemy->GetActorLocation() - EnemyAIComp->CurTargetLocation).Size())
						{
							CurTargetLocation = OwnerLocation - 0.5 * FMath::FRand() * DefaultDirVector;
							return CurTargetLocation;
						}
					}
				}
			}
		}

		CurTargetLocation = Location;
		return Location;
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
		float DistanceScale;
		if (tmpR > 0)  // 当前偏转 < 90°
		{
			DistanceScale = FMath::Max(0.5, tmpR) + tmpD;
		}
		else // > 90°
		{
			DistanceScale = FMath::Min(-0.5, tmpR) - tmpD;
		}
		
// 		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black,
//		FString::Printf(TEXT("GetMoveDotDirRandLocation R: %d, %d"), R, __LINE__));
		
		// 目标点相对起点的位置
		FVector TempTargetOffset = DistanceScale * FRotator(0, (FMath::RandBool() ? 1 : -1) * DotDirPerRotate * n, 0).RotateVector(DefaultDirVector);

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwningEnemy); // 忽略自己

		// 存储碰撞结果
		FHitResult HitResult;

		EDrawDebugTrace::Type DrawType = EDrawDebugTrace::None;

#if WITH_EDITOR
		if (CDebugTraceDuration.GetValueOnGameThread())
		{
			DrawType = EDrawDebugTrace::ForDuration;
		}
#endif
		
		// 执行射线检测
		bool bHit = UKismetSystemLibrary::LineTraceSingle(
			this, // WorldContextObject
			OwnerLocation, // 起点
			OwnerLocation + TempTargetOffset, // 终点
			ETraceTypeQuery::TraceTypeQuery3, // 检测通道（例如 Visibility）
			false, // bTraceComplex
			ActorsToIgnore, // 忽略的 Actor
			DrawType, // 调试绘制类型
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
	
#if WITH_EDITOR
	if (CDebugTraceDuration.GetValueOnGameThread())
	{
		// 寻路射线测试
		for(auto& v : TargetOffsetMap)
		{
			FLinearColor color = FLinearColor::LerpUsingHSV(FLinearColor(0.1, 0.5, 0.1, 0.2),FLinearColor::Green, v.Key.Length() / 150);
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), OwnerLocation, OwnerLocation + v.Value, color, 0.2f, 0.5);
		}
	}

#endif

	TArray<FVector> values;
	TargetOffsetMap.GetKeys(values);
	int tmpV = 0;
	int randV = FMath::RandRange(0, totalR);
	for (int i = 0; i < values.Num(); i++)
	{
		tmpV += TargetOffsetMap[values[i]];
		if (tmpV >= randV)
		{
			BlockValue = BlockValue * BlockValueWeekValue;
			FVector CurDir = values[i].GetSafeNormal2D();
			if (!PreDir.IsZero())
			{
				CurDir = UKismetMathLibrary::VLerp(CurDir, PreDir, PreDirValue);
			}
			
			PreDir = CurDir;
			return values[i].Size() * CurDir + OwnerLocation;
		}
	}

	BlockValue += FMath::Abs(blockR);
	PreDir = FVector::ZeroVector;
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


FVector UEnemyAIComponent::GetCurActionFieldDirectionLocation(const bool bNear)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, FVector::ZeroVector)
	
	if (!IsValid(PXCharacter)) return OwningEnemy->GetActorLocation();

	const FVector PlayerLocation = PXCharacter->GetActorLocation();
	const FVector OwnerLocation = OwningEnemy->GetActorLocation();
	float Distance = (PlayerLocation - OwnerLocation).Size2D();
	FVector Dir = (OwnerLocation - PlayerLocation).GetSafeNormal2D();

	// 应该直接走向最近的能攻击的区域
	// 可能后续要加入直接跳跃到可攻击区域的技能
	
	FVector TargetLocation = PlayerLocation;
	
	if (ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y)
	{
		if (bNear)
		{
			TargetLocation = GetAttackLocation();
		}
		else
		{
			TargetLocation = PlayerLocation + Dir * (ActionFieldDistance.DistanceMid.X + 10);
		}
	}
	else if (ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y)
	{
		if (bNear)
		{
			TargetLocation = PlayerLocation + Dir * (ActionFieldDistance.DistanceNear.Y - 10);
		}
		else
		{
			TargetLocation = PlayerLocation + Dir * (ActionFieldDistance.DistanceFar.X + 10);
		}
	}
	else if (ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y)
	{
		if (bNear)
		{
			TargetLocation = PlayerLocation + Dir * (ActionFieldDistance.DistanceMid.Y - 10);
		}
		else
		{
			TargetLocation = PlayerLocation + Dir * (ActionFieldDistance.DistanceRemote.X + 10);
		}
	}
	else if (ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y)
	{
		if (bNear)
		{
			TargetLocation = PlayerLocation + Dir * (ActionFieldDistance.DistanceRemote.X - 10);
		}
		else
		{
			TargetLocation = OwnerLocation + Dir * 100;
		}
	}

	
	CurTargetLocation = GetMoveDotDirRandLocation(TargetLocation);
	return CurTargetLocation;
}

FVector UEnemyAIComponent::GetNearestActionFieldCanAttackLocation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, FVector::ZeroVector)

	FVector EnemyLocation = OwningEnemy->GetActorLocation();
	FGameplayTag GameplayTag = GetActionFieldByPlayer();
	if (!GameplayTag.IsValid()) return EnemyLocation;

	const FGameplayTagContainer& ActionFieldsCanAttack = OwningEnemy->ActionFieldsCanAttack;

	FVector PlayerLocation = PXCharacter->GetActorLocation();
	FVector EastDir = PXCharacter->GetRightVectorWithBlendYaw();
	FVector NorthDir = FRotator(0, 90, 0).RotateVector(EastDir);
	FVector WestDir = FRotator(0, 90, 0).RotateVector(NorthDir);
	FVector SouthDir = FRotator(0, 90, 0).RotateVector(WestDir);

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
			TmpPoint = PlayerLocation + NorthDir * DirDistance.Value;
		}
		else if (TagString.Contains("West"))
		{
			TmpPoint = PlayerLocation + WestDir * DirDistance.Value;
		}
		else if (TagString.Contains("South"))
		{
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

