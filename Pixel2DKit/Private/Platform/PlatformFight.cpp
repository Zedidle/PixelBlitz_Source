// Fill out your copyright notice in the Description page of Project Settings.


#include "Platform/PlatformFight.h"

#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/DebugFuncLab.h"

void APlatformFight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	
	if (Enemies.IsEmpty())
	{
		PXCharacter->TargetFightCameraOffset = FVector::ZeroVector;
		return;
	}
	int32 TmpNum = 0;
	FVector TmpCenterLocation = FVector::ZeroVector;

	for (auto Enemy: Enemies)
	{
		if (Enemy->Implements<UFight_Interface>())
		{
			if (IFight_Interface::Execute_GetTarget(Enemy))
			{
				if (PXCharacter->CurSpringArmLength * 1.2 > Enemy->GetDistanceTo(PXCharacter))
				{
					TmpCenterLocation += Enemy->GetActorLocation();
					TmpNum++;
				}
			}
		}
	}

	if (TmpNum == 0)
	{
		PXCharacter->TargetFightCameraOffset = FVector::ZeroVector;
		return;
	}

	// 设置镜头中心偏移
	TmpCenterLocation = TmpCenterLocation / TmpNum;
	FVector PlayerLocation = PXCharacter->GetActorLocation();
	FVector DirLength = TmpCenterLocation - PlayerLocation;


	float FightCameraOffsetPoint = DirLength.Size() / (PXCharacter->CurSpringArmLength * 1.2);
	PXCharacter->TargetFightCameraOffset = FightCameraOffsetFactor * DirLength * FightCameraOffsetPoint;

	
	// 设置镜头偏转
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector EyeViewPoint; FRotator _;
	CameraManager->GetActorEyesViewPoint(EyeViewPoint, _);
	
	FVector CenterToViewPoint =  TmpCenterLocation - EyeViewPoint;
	float DistanceCenterToPlayer = DirLength.Size();
	float cal2 = PXCharacter->CurSpringArmLength + FMath::GetMappedRangeValueClamped(FVector2D(InRangeA, PXCharacter->CurSpringArmLength * 0.7),
		FVector2D(0.0f, 100.0f), DistanceCenterToPlayer);

	if (CenterToViewPoint.Size() < cal2)
	{
		float CrossZ = FVector::CrossProduct(CenterToViewPoint.GetSafeNormal(), (PlayerLocation - EyeViewPoint).GetSafeNormal()).Z;
		if (FMath::Abs(CrossZ) > 0.15)
		{
			PXCharacter->AddViewYaw(CrossZ * FMath::GetMappedRangeValueClamped(FVector2D(0, 300),
					FVector2D(-1.8, OutRangeB), PXCharacter->GetVelocity().Size()), false);
		}
	}
}

void APlatformFight::RegisterEnemies_Implementation()
{
	for (auto spawner : EnemySpawners)
	{
		if (!IsValid(spawner)) continue;
		if (ABaseEnemy* enemy = spawner->SpawnEnemy())
		{
			enemy->OnEnemyDie.AddDynamic(this, &APlatformFight::OnEnemyDie);
			Enemies.Add(enemy);
		}
	}
}

void APlatformFight::OnEnemyDie_Implementation(ABaseEnemy* enemy)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(enemy)
	Enemies.Remove(enemy);
	enemy->OnEnemyDie.RemoveAll(this);
	if (IsValid(PlatformFightCountWidget))
	{
		PlatformFightCountWidget->UpdateCount(Enemies.Num());
	}

	// UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("APlatformFight::OnEnemyDie Num: %d"), Enemies.Num()));

	if (Enemies.IsEmpty())
	{
		FightEnd();
	}

}

void APlatformFight::FightEnd_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	

	PXCharacter->TargetFightCameraOffset = FVector::ZeroVector;
	PXCharacter->OnPlayerDie.RemoveDynamic(this, &ThisClass::FightEnd);
	PXCharacter = nullptr;

	ActivateFight(false);
	SetActorTickEnabled(false);
}

void APlatformFight::ShowCountWidget()
{
	if (IsValid(PlatformFightCountWidget))
	{
		PlatformFightCountWidget->Show();
	}
	else if (PlatformFightCountWidgetClass)
	{
		PlatformFightCountWidget = CreateWidget<UPlatformFightCountWidget>(GetWorld(), PlatformFightCountWidgetClass);
		if (PlatformFightCountWidget)
		{
			PlatformFightCountWidget->CurNum = Enemies.Num();
			PlatformFightCountWidget->AddToViewport(999);
		}
	}
}

void APlatformFight::HideCountWidget()
{
	if (IsValid(PlatformFightCountWidget))
	{
		PlatformFightCountWidget->Fade();
	}
}

bool APlatformFight::ActivateFight_Implementation(bool bActivate)
{
	if (bActivate)
	{
		if (Enemies.IsEmpty()) return false;

		ShowCountWidget();
		return true;
	}

	HideCountWidget();
	
	return false;
}

void APlatformFight::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (ABasePXCharacter* C = Cast<ABasePXCharacter>(OtherActor))
	{
		SetActorTickEnabled(true);
		PXCharacter = C;
		if (!PXCharacter->OnPlayerDie.IsAlreadyBound(this, &ThisClass::FightEnd))
		{
			PXCharacter->OnPlayerDie.AddDynamic(this, &ThisClass::FightEnd);
		}
		ActivateFight(true);
		OnPlayerIn();
	}
}

void APlatformFight::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (OtherActor == PXCharacter && Enemies.Num() > 0)
	{
		FightEnd();
		OnPlayerOut();
	}

}
