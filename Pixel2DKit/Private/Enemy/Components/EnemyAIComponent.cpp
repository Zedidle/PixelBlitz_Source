// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Components/EnemyAIComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FunctionLibrary/SpaceFuncLib.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UEnemyAIComponent::UEnemyAIComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
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
		return OwnerLocation + DefaultDirVector;
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
				CurDir = UKismetMathLibrary::VLerp(PreDir, CurDir ,PreDirValue);
			}
			
			PreDir = CurDir;
			return values[i].Size() * CurDir + OwnerLocation;
		}
	}

	BlockValue += FMath::Abs(blockR);
	PreDir = FVector::ZeroVector;
	return OwnerLocation;
}

bool UEnemyAIComponent::InAttackRangeX_EnemyAI()
{
	if (!PixelCharacter || !GetOwner()) return false;
	float distanceX = USpaceFuncLib::GetDistanceX(GetOwner(), PixelCharacter, 0);
	float distanceY = USpaceFuncLib::GetDistanceY(GetOwner(), PixelCharacter, 0);

	return distanceY < AttackRangeOffset_Y &&
			distanceX < AttackRange_X + AttackRangeOffset_X;
}

bool UEnemyAIComponent::InAttackRangeY_EnemyAI()
{
	if (!PixelCharacter || !GetOwner()) return false;
	if (!CanAttackY) return false;

	float distanceX = USpaceFuncLib::GetDistanceX(GetOwner(), PixelCharacter, 0);
	float distanceY = USpaceFuncLib::GetDistanceY(GetOwner(), PixelCharacter, 0);

	return distanceY < AttackRange_Y + AttackRangeOffset_Y &&
			distanceX < AttackRange_X + AttackRangeOffset_X;
}

FVector UEnemyAIComponent::GetAttackLocation_EnemyAI()
{
	if (!GetOwner() || !PixelCharacter) return FVector(0);

	FVector LocationX = GetTargetLocationX();
	if (!CanAttackY) return LocationX;

	FVector LocationY = GetTargetLocationY();
	if (FVector::Distance(GetOwner()->GetActorLocation(), LocationX) > FVector::Distance(GetOwner()->GetActorLocation(), LocationY))
	{
		return LocationY;
	}
	return LocationX;
}

FVector UEnemyAIComponent::GetTargetLocationX()
{
	if (!GetOwner() || !PixelCharacter) return FVector(0);

	FVector Dir = FRotator(0, 90, 0).RotateVector(USpaceFuncLib::GetDirection2DFromPlayerViewPoint(0));
	
	// 基于方位的左或右偏移
	FVector Offset = Dir * AttackRange_X;


	EWorldDirection Direction = USpaceFuncLib::ActorAtActorWorldDirection(GetOwner(), PixelCharacter);
	
	// 选择较近的一侧
	if (USpaceFuncLib::ActorAtActorRight(PixelCharacter, GetOwner()))
	{
		Offset *= -1;
	}

	return GetMoveDotDirRandLocation(PixelCharacter->GetActorLocation() + Offset);
}

FVector UEnemyAIComponent::GetTargetLocationY()
{
	if (!GetOwner() || !PixelCharacter) return FVector(0);

	FVector Dir = USpaceFuncLib::GetDirection2DFromPlayerViewPoint(0);
	FVector Offset = Dir * AttackRange_Y;
	if (USpaceFuncLib::ActorAtActorFront(GetOwner(), PixelCharacter))
	{
		Offset *= -1;	
	}
	return GetMoveDotDirRandLocation(PixelCharacter->GetActorLocation() + Offset);
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
			// 默认近战移动
			TargetLocation = GetAttackLocation_EnemyAI();
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

	return GetMoveDotDirRandLocation(TargetLocation);
}


float UEnemyAIComponent::GetCheckCliffHeight_EnemyAI()
{
	AActor* Owner = GetOwner();
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
	
	
	if (Dir == EWorldDirection::East)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return EActionField::EastNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return EActionField::EastMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return EActionField::EastMid;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return EActionField::EastMid;	
		}
	}
	else if (Dir == EWorldDirection::West)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return EActionField::WestNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return EActionField::WestMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return EActionField::WestMid;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return EActionField::WestMid;	
		}
	}
	else if (Dir == EWorldDirection::North)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return EActionField::NorthNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return EActionField::NorthMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return EActionField::NorthMid;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return EActionField::NorthMid;	
		}
	}
	else if (Dir == EWorldDirection::South)
	{
		if( ActionFieldDistance.DistanceNear.X < Distance && Distance <= ActionFieldDistance.DistanceNear.Y )
		{
			return EActionField::SouthNear;
		}
		if( ActionFieldDistance.DistanceMid.X < Distance && Distance <= ActionFieldDistance.DistanceMid.Y )
		{
			return EActionField::SouthMid;	
		}
		if( ActionFieldDistance.DistanceFar.X < Distance && Distance <= ActionFieldDistance.DistanceFar.Y )
		{
			return EActionField::SouthMid;	
		}
		if( ActionFieldDistance.DistanceRemote.X < Distance && Distance <= ActionFieldDistance.DistanceRemote.Y )
		{
			return EActionField::SouthMid;	
		}
	}

	return None;
}


// Called when the game starts
void UEnemyAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemyAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

