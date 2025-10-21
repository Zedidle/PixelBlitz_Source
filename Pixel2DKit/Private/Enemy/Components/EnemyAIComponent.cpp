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
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/DebugFuncLab.h"

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

void UEnemyAIComponent::SetPXCharacter(ABasePXCharacter* Character)
{
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningEnemy)
	PXCharacter = Character;

	FName TimerName = FName("UEnemyAIComponent::SetPixelCharacter" + OwningEnemy->GetActorNameOrLabel());
	
	if (PXCharacter)
	{
		if (UTimerSubsystemFuncLib::IsDelayActive(GetWorld(), TimerName)) return;
		
		UTimerSubsystemFuncLib::SetDelayLoop(GetWorld(), TimerName, [WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->UpdatePlayerPaths();
			
		}, CheckPlayerLocationInterval);
	}
	else
	{
		UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("UEnemyAIComponent::SetPixelCharacter: null")));
		UTimerSubsystemFuncLib::CancelDelay(GetWorld(), TimerName);
	}

	
}

void UEnemyAIComponent::UpdatePlayerPaths()
{
	if (!PXCharacter) return;
	if (!OwningEnemy) return;
			
	FHitResult OutHit;
	// 想玩家位置做一个射线检测，判断能否直接看见
	bool bLostSeePlayer = UKismetSystemLibrary::LineTraceSingle(GetWorld(), OwningEnemy->GetActorLocation(), PXCharacter->GetActorLocation(),
	TraceTypeQuery1, false, {OwningEnemy},
			EDrawDebugTrace::None, OutHit, true,
			FLinearColor::Yellow, FLinearColor::Blue, 1.0f);

	if (bLostSeePlayer) return;

	PlayerPaths.Add(PXCharacter->GetActorLocation());
	
	if (PlayerPaths.Num() > MaxPlayerPathsNum)
	{
		PlayerPaths.RemoveAt(0);
	}
}

FVector UEnemyAIComponent::GetMoveDotDirRandLocation(FVector NewTargetLocation, float DotDirPerRotate, float MaxRotateValue,
                                                     float DefaultDirRotate, float MinDirectlyDistance)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, FVector::ZeroVector);

	// 不同步高度，可能可以实现跨阶梯
	// NewTargetLocation.Z = OwnerLocation.Z;

	// 检测同盟是否阻挡自身的移动方位
	if (MoveCheckAllies(NewTargetLocation, NewTargetLocation, MinDirectlyDistance)) return NewTargetLocation;


	FVector OwnerLocation = OwningEnemy->GetActorLocation();
	
	if (PreDirection.IsZero())
	{
		PreBlockYawDirection = FMath::RandBool();
	}
	
	// 移动向量
	float BlockYawModify = FMath::Min(MaxBlockYawModify, BlockValue * BlockDirModifyValue);
	
	FVector MoveVector = FRotator(0, (PreBlockYawDirection ? 1 : -1) * (DefaultDirRotate + BlockYawModify), 0)
							.RotateVector(NewTargetLocation - OwnerLocation);

	
	TMap<FVector, int> TargetOffsetMap;
	int totalR = 0;
	
	float blockR = 0; // 阻挡因子
	
	int n = -1;
	while (DotDirPerRotate * n <= MaxRotateValue)
	{
		n++;

		float tmpD = blockR * DotDirPerRotate * n / MaxRotateValue;
		
		float tmpR = FMath::Cos(FMath::DegreesToRadians(DotDirPerRotate * n));

		// 一般来说，越偏移目标点则概率越小，移动距离也越小
		// 但阻挡因子会影响 起始方向 偏转
		float DistanceScale = tmpR > 0 ? FMath::Max(0.5, tmpR) + tmpD : FMath::Min(-0.5, tmpR) - tmpD;
		
		// 目标点相对起点的位置
		FVector TempTargetOffset = DistanceScale * FRotator(0, (PreBlockYawDirection ? 1 : -1) * DotDirPerRotate * n, 0).RotateVector(MoveVector);

		// 存储碰撞结果
		FHitResult HitResult;

		// 执行射线检测
		bool bHit = UKismetSystemLibrary::LineTraceSingle(this, OwnerLocation, OwnerLocation + TempTargetOffset,
			TraceTypeQuery3,false, {OwningEnemy}, 
			EDrawDebugTrace::None,
			HitResult, // 碰撞结果
			true, FLinearColor::Blue, FLinearColor::Green, 0.1 );

		// 无法到达则跳过并提供阻挡因子
		if (bHit || USpaceFuncLib::CheckCliffProcess(OwnerLocation, OwnerLocation + TempTargetOffset, GetCheckCliffHeight()))
		{
			// UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("blockR Plus DistanceScale: %f \n "), DistanceScale));
			blockR += DistanceScale;
			continue;
		}

		float tmpBlockAvg = blockR * (MaxRotateValue - DotDirPerRotate * n) / MaxRotateValue;
		tmpR = (DistanceScale + tmpBlockAvg ) * 100 + 400;
		totalR += tmpR;
		TargetOffsetMap.FindOrAdd(TempTargetOffset, tmpR);
		
		blockR -= tmpBlockAvg;
	}

	
	FVector Result;
	if (UCommonFuncLib::CalRandomMap(TargetOffsetMap, Result))
	{
		BlockValue *= BlockValueWeekPercent;
		FVector CurDirection = Result.GetSafeNormal2D();
		if (!PreDirection.IsZero())
		{
			CurDirection = UKismetMathLibrary::VLerp(CurDirection, PreDirection, PreDirectionPercent);
		}
			
		PreDirection = CurDirection;
		return Result.Size() * CurDirection + OwnerLocation;
	}

	// UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("\n BlockValue Plus blockR: %f \n "), blockR));
	
	BlockValue += FMath::Abs(blockR);
	PreDirection = FVector::ZeroVector;
	return OwnerLocation;
}

bool UEnemyAIComponent::MoveCheckAllies(FVector& Result, FVector NewTargetLocation, float MinDirectlyDistance)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(OwningEnemy, false);
	
	FVector OwnerLocation = OwningEnemy->GetActorLocation();
	
	// 判断是否有怪物要去相近的位置，如果相较之下自身里目标位置较远，则后退让位
	TArray<AActor*> ActorsToIgnore = {OwningEnemy};
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), OwnerLocation, NewTargetLocation, AllyCheckRadius, 
EnemyTrace, false, ActorsToIgnore, EDrawDebugTrace::None, OutHits,
true, FLinearColor::Red, FLinearColor::Green, 0.1f);

	TArray<ABaseEnemy*> FoundAllies;
	for (auto& result : OutHits)
	{
		if (!IsValid(result.GetActor())) continue;
		if (ABaseEnemy* Ally = Cast<ABaseEnemy>(result.GetActor()))
		{
			FoundAllies.Add(Ally);		
		}
	}

	FVector MoveVector = NewTargetLocation - OwnerLocation;
	
	if (FoundAllies.IsEmpty())
	{
		if (MoveVector.Size() < MinDirectlyDistance)
		{
			CurTargetLocation = NewTargetLocation;
			Result = CurTargetLocation;
			return true;
		}
		return false;
	}
	else
	{
		for (auto& Ally : FoundAllies)
		{
			if (!IsValid(Ally)) continue;

			UEnemyAIComponent* AllyAI = Ally->GetComponentByClass<UEnemyAIComponent>();
			if (!IsValid(AllyAI)) continue;

			FVector VectorToAlly = Ally->GetActorLocation() - OwnerLocation;

			// 近距离斥力
			if (VectorToAlly.Length() < AllyCheckRadius * 0.5)
			{
				NewTargetLocation += 0.5 * AllyRepulsion * (OwnerLocation - Ally->GetActorLocation());
			}

			// 移动方位斥力， 与同盟位置的点乘
			float DotAlly = MoveVector.GetSafeNormal2D().Dot(VectorToAlly.GetSafeNormal2D());
			// 如果当前移动方向与检测到的同盟方向不接近，> 60° ，则忽略
			if ( DotAlly < 0.5)
			{
				// 检测到同盟后，产生默认斥力
				NewTargetLocation += AllyRepulsion * (OwnerLocation - Ally->GetActorLocation());
			}

			FVector AllyToTarget = Ally->GetActorLocation() - AllyAI->CurTargetLocation;
			
			// 如果同盟的目标点 与 自身目标点十分接近，且它到目标的距离小于自身到目标的，则再次产生斥力（让路）
			if (AllyAI->CurTargetLocation.Equals(NewTargetLocation, 20) && MoveVector.Size() > AllyToTarget.Size())
			{
				NewTargetLocation += 0.5 * AllyRepulsion * (OwnerLocation - AllyAI->CurTargetLocation);
			}
		}

		CurTargetLocation = NewTargetLocation;
		Result = CurTargetLocation;
		return true;
	}
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

bool UEnemyAIComponent::GetPlayerPathPoint(FVector& Point)
{
	if (!OwningEnemy || PlayerPaths.IsEmpty()) return false;
	
	Point = PlayerPaths[0];
	if (FVector::Distance(Point, OwningEnemy->GetActorLocation()) < DistanceNextPathPointNear)
	{
		// PlayerPaths.HeapPopDiscard(0);
		PlayerPaths.RemoveAt(0);
	}

	return true;
}


// Called when the game starts
void UEnemyAIComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningEnemy = Cast<ABaseEnemy>(GetOwner());
	
}

void UEnemyAIComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	FName TimerName = FName("UEnemyAIComponent::SetPixelCharacter" + OwningEnemy->GetActorNameOrLabel());
	UTimerSubsystemFuncLib::CancelDelay(GetWorld(), TimerName);
}


// Called every frame
void UEnemyAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

