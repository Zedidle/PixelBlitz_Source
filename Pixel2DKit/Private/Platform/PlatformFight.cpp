// Fill out your copyright notice in the Description page of Project Settings.


#include "Platform/PlatformFight.h"

#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"

void APlatformFight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PlayerCharacter)
	
	if (Enemies.IsEmpty())
	{
		PlayerCharacter->FightCenterForCameraOffset = FVector::ZeroVector;
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
				if (PlayerCharacter->CurSpringArmLength * 1.2 > Enemy->GetDistanceTo(PlayerCharacter))
				{
					TmpCenterLocation += Enemy->GetActorLocation();
					TmpNum++;
				}
			}
		}
	}

	if (TmpNum == 0)
	{
		PlayerCharacter->FightCenterForCameraOffset = FVector::ZeroVector;
		return;
	}

	// 设置镜头中心偏移
	TmpCenterLocation = TmpCenterLocation / TmpNum;
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector DirLength = TmpCenterLocation - PlayerLocation;
	PlayerCharacter->FightCenterForCameraOffset = DirLength *
		FRotator(0, PlayerCharacter->CurBlendYaw, 0).RotateVector(FVector(0.25, 0.15, 0.3));


	// 设置镜头偏转
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector EyeViewPoint; FRotator _;
	CameraManager->GetActorEyesViewPoint(EyeViewPoint, _);
	
	FVector CenterToViewPoint =  TmpCenterLocation - EyeViewPoint;
	float DistanceCenterToPlayer = DirLength.Size();
	float cal2 = PlayerCharacter->CurSpringArmLength + FMath::GetMappedRangeValueClamped(FVector2D(InRangeA, PlayerCharacter->CurSpringArmLength * 0.7),
		FVector2D(0.0f, 100.0f), DistanceCenterToPlayer);

	if (CenterToViewPoint.Size() < cal2)
	{
		float CrossZ = FVector::CrossProduct(CenterToViewPoint.GetSafeNormal(), (PlayerLocation - EyeViewPoint).GetSafeNormal()).Z;
		if (FMath::Abs(CrossZ) > 0.15)
		{
			PlayerCharacter->AddViewYaw(CrossZ * FMath::GetMappedRangeValueClamped(FVector2D(0, 300),
					FVector2D(-1.8, OutRangeB), PlayerCharacter->GetVelocity().Size()), false);
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
			enemy->OnEnemyDeath.AddDynamic(this, &APlatformFight::OnEnemyDie);
			Enemies.Add(enemy);
		}
	}
}

void APlatformFight::OnEnemyDie_Implementation(ABaseEnemy* enemy)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(enemy)
	Enemies.Remove(enemy);
	enemy->OnEnemyDeath.RemoveAll(this);
	if (IsValid(PlatformFightCountWidget))
	{
		PlatformFightCountWidget->UpdateCount(Enemies.Num());
	}

	if (Enemies.IsEmpty())
	{
		SetActorTickEnabled(false);
		FightEnd();
		
	}

}

void APlatformFight::FightEnd_Implementation()
{
}

void APlatformFight::ShowCountWidget(bool bShow)
{
	if (bShow)
	{
		if (IsValid(PlatformFightCountWidget))
		{
			PlatformFightCountWidget->Show();
		}
		else
		{
			if (PlatformFightCountWidgetClass)
			{
				PlatformFightCountWidget = CreateWidget<UPlatformFightCountWidget>(GetWorld(), PlatformFightCountWidgetClass);
				if (PlatformFightCountWidget)
				{
					PlatformFightCountWidget->AddToViewport(999);
				}
			}
		}
	}
	else
	{
		if (IsValid(PlatformFightCountWidget))
		{
			PlatformFightCountWidget->Fade();
		}
	}
}


void APlatformFight::ActivateFight(bool bActivate)
{
	if (bActivate)
	{
		
	}
	else
	{
		
	}
}

void APlatformFight::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (ABasePXCharacter* C = Cast<ABasePXCharacter>(OtherActor))
	{
		PlayerCharacter = C;
		SetActorTickEnabled(true);
	}
}

void APlatformFight::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	SetActorTickEnabled(false);
	PlayerCharacter = nullptr;
}
