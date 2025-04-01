// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Components/EnemyAIComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FunctionLibrary/SpaceFuncLib.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"


UEnemyAIComponent::UEnemyAIComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UEnemyAIComponent::SetPixelCharacter(ABasePixelCharacter* Character)
{
	if (AttackPatienceTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackPatienceTimerHandle);
	}
	
	if (Character == nullptr) return;
	
	PixelCharacter = Character;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda(
		[this]()
		{
			if (ABaseEnemy* Owner = Cast<ABaseEnemy>(GetOwner()))
			{
				Owner->SetPixelCharacter(nullptr);
			}
		});
	GetWorld()->GetTimerManager().SetTimer(AttackPatienceTimerHandle, TimerDelegate,AttackPatienceTime,false );
}

FVector UEnemyAIComponent::GetMoveDotDirRandLocation(FVector TargetLocation, float DotDirPerRotate, float MaxRotateValue,
                                                     float DefaultDirRotate, const float MinDirectlyDistance)
{
	if (GetOwner() == nullptr) return FVector::ZeroVector;
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	TargetLocation.Z = OwnerLocation.Z; // 涉及到转向，避免Owner太高转到半空中导致无效位置
	FVector DefaultDirVector = FRotator(0, (FMath::RandBool()? 1 : -1) * (DefaultDirRotate + BlockValue * BlockDirModifyValue), 0)
							.RotateVector(TargetLocation - OwnerLocation);

	
	if (MinDirectlyDistance > DefaultDirVector.Length())
	{
		BlockValue = 0;
		FVector Location = OwnerLocation + DefaultDirVector;
		// 判断是否有怪物要去相近的位置，如果相较之下自身里目标位置较远，则后退让位
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner()); // 忽略自己
		TArray<FHitResult> OutHits;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), OwnerLocation, Location, 100, 
	TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::None, OutHits,
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
		ActorsToIgnore.Add(GetOwner()); // 忽略自己

		// 存储碰撞结果
		FHitResult HitResult;

		// 执行射线检测
		bool bHit = UKismetSystemLibrary::LineTraceSingle(
			this, // WorldContextObject
			OwnerLocation, // 起点
			OwnerLocation + TempTargetOffset, // 终点
			ETraceTypeQuery::TraceTypeQuery3, // 检测通道（例如 Visibility）
			false, // bTraceComplex
			ActorsToIgnore, // 忽略的 Actor
			EDrawDebugTrace::None, // 调试绘制类型
			HitResult, // 碰撞结果
			true // bIgnoreSelf
			
		);

		// 检查是否命中
		if (bHit && HitResult.GetActor() || USpaceFuncLib::CheckCliffProcess(OwnerLocation, OwnerLocation + TempTargetOffset, GetCheckCliffHeight_EnemyAI()))
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

	// 寻路射线测试
	// for(auto& v : TargetOffsetMap)
	// {
	// 	FLinearColor color = FLinearColor::LerpUsingHSV(FLinearColor(0.1, 0.5, 0.1, 0.2),FLinearColor::Green, float(v.Key) / 150);
	// 	UKismetSystemLibrary::DrawDebugLine(GetWorld(), OwnerLocation, OwnerLocation + v.Value, color, 0.2f, 0.5);
	// }

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
	if (!IsValid(Owner)) return FVector::Zero();
	if (!IsValid( PixelCharacter)) return FVector::Zero();	

	FVector Dir = (Owner->GetActorLocation() - PixelCharacter->GetActorLocation()).GetSafeNormal2D();
	if (Owner->GetDistanceToPlayer() < AttackRange.X)
	{
		return PixelCharacter->GetActorLocation() + (AttackRange.X + 5) * Dir;
	}

	if (Owner->GetDistanceToPlayer() > AttackRange.Y)
	{
		return PixelCharacter->GetActorLocation() + (AttackRange.Y - 5) * Dir;
	}

	return Owner->GetActorLocation();
}


FVector UEnemyAIComponent::GetActionFieldLocation(const bool bNear)
{
	if (!IsValid(GetOwner())) return FVector(0);
	if (!IsValid(PixelCharacter)) return GetOwner()->GetActorLocation();

	const FVector PlayerLocation = PixelCharacter->GetActorLocation();
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	float Distance = (PlayerLocation - OwnerLocation).Size2D();
	FVector Dir = (OwnerLocation - PlayerLocation).GetSafeNormal2D();

	FVector TargetLocation;
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


float UEnemyAIComponent::GetCheckCliffHeight_EnemyAI()
{
	if (!IsValid(Owner)) return 50;
	
	UCapsuleComponent* CapsuleComponent = GetOwner()->GetComponentByClass<UCapsuleComponent>();
	UCharacterMovementComponent* CharacterMovmentComponent = GetOwner()->GetComponentByClass<UCharacterMovementComponent>();
	if (!CapsuleComponent || !CharacterMovmentComponent) return 0;
	
	float ScaleZ = GetOwner()->GetActorScale3D().Z;
	return ScaleZ * (CharacterMovmentComponent->MaxStepHeight + CapsuleComponent->GetUnscaledCapsuleHalfHeight());
}

EActionField UEnemyAIComponent::GetActionFieldByPlayer() const
{
	if (!IsValid(PixelCharacter)) return EActionField::None;
	if (!IsValid(GetOwner())) return EActionField::None;
	
	EWorldDirection Dir = USpaceFuncLib::ActorAtActorWorldDirection(GetOwner(), PixelCharacter, PixelCharacter->CurBlendYaw);
	float Distance = (GetOwner()->GetActorLocation() - PixelCharacter->GetActorLocation()).Size2D();
	
	
	if (Dir == East)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return EastNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return EastMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return EastFar;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return EastRemote;	
		}
	}
	else if (Dir == West)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return WestNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return WestMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return WestFar;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return WestRemote;	
		}
	}
	else if (Dir == North)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return NorthNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return NorthMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return NorthFar;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return NorthRemote;	
		}
	}
	else if (Dir == South)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return SouthNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return SouthMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return SouthFar;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return SouthRemote;	
		}
	}

	return None;
}


// Called when the game starts
void UEnemyAIComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ABaseEnemy>(GetOwner());
	
}


// Called every frame
void UEnemyAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

