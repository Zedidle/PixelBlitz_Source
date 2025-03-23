// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"

#include <filesystem>
#include "Components/WidgetComponent.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "FunctionLibrary/SpaceFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "BasePixelAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"

AActor* ABaseEnemy::GetPixelCharacter()
{
	return Cast<AActor>(PixelCharacter);
}

bool ABaseEnemy::SetPixelCharacter(AActor* Character)
{
	if (Character == nullptr)
	{
		PixelCharacter = nullptr;
		if (UEnemyAIComponent* EnemyAIComponent = GetComponentByClass<UEnemyAIComponent>())
		{
			EnemyAIComponent->PixelCharacter = nullptr;			
		}
		if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(GetController()))
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("PlayerPawn"), nullptr);
		}
		return true;
	}
	
	if (ABasePixelCharacter* C = Cast<ABasePixelCharacter>(Character))
	{
		PixelCharacter = C;
		if (UEnemyAIComponent* EnemyAIComponent = GetComponentByClass<UEnemyAIComponent>())
		{
			EnemyAIComponent->PixelCharacter = PixelCharacter;			
		}
		if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(GetController()))
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("PlayerPawn"), PixelCharacter);
		}
		return true;
	}
	return false;
}

void ABaseEnemy::SetDead(bool V)
{
	bDead = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetDead(V);
	}
	if (UBlackboardComponent* BlackboardComponent = GetController()->FindComponentByClass<UBlackboardComponent>())
	{
		BlackboardComponent->SetValueAsBool(FName("bDead"), V);
	}
}

void ABaseEnemy::SetHurt(bool V, const float duration)
{
	bHurt = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetHurt(V);
	}
	if (!bHurt) return;
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
[this]{
			SetHurt(false, 0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, duration, false);
}

void ABaseEnemy::SetInAttackState(bool V)
{
	bInAttackEffect = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetInAttackState(V);
	}
}

void ABaseEnemy::SetInAttackEffect(bool V)
{
	bInAttackEffect = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetInAttackEffect(V);
	}
}

void ABaseEnemy::SetAttackStartX(const bool V)
{
	bAttackStartX = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetAttackStartX(V);
	}
}

void ABaseEnemy::SetAttackStartY(const bool V)
{
	bAttackStartY = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetAttackStartY(V);
	}
}

void ABaseEnemy::SetInDefendState(const bool V)
{
	bInDefendState = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetDefendState(V);
	}
}

void ABaseEnemy::SetDefendStart(const bool V)
{
	bDefendStart = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetDefendState(V);
	}
}

void ABaseEnemy::SetDefendHurt(const bool V)
{
	bDefendHurt = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetDefendHurt(V);
	}
}

void ABaseEnemy::SetJumping(const bool V, const float time)
{
	bJumping = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetJumping(V);
	}
	if (!bJumping) return;
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
[this]{
			SetJumping(false, 0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, time, false);
	
}

void ABaseEnemy::SetLanding(const bool V, const float time)
{
	bLanding = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetLanding(V);
	}
	if (!bLanding) return;
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
[this]{
			SetLanding(false, 0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, time, false);
}

ABaseEnemy::ABaseEnemy()
{
	HealthComponent_CPP = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent_CPP"));
	FightComp = CreateDefaultSubobject<UFightComponent>(TEXT("FightComp"));
}


bool ABaseEnemy::GetIsAttacking_Implementation()
{
	return false;
}

bool ABaseEnemy::IsAlive_Implementation()
{
	return !bDead;
}

AActor* ABaseEnemy::GetTarget_Implementation()
{
	return PixelCharacter;
}

float ABaseEnemy::GetRandomMoveRange_Implementation()
{
	return RandomMoveRange;
}

bool ABaseEnemy::InAtttckRange_EnemyAI_Implementation()
{
	if (UEnemyAIComponent* EnemyAIComponent = GetComponentByClass<UEnemyAIComponent>())
	{
		return EnemyAIComponent->InAttackRangeX_EnemyAI() || EnemyAIComponent->InAttackRangeY_EnemyAI(); 	
	}
	return false;
}

void ABaseEnemy::OnReachedAttackLocation_EnemyAI_Implementation()
{
	if (UEnemyAIComponent* EnemyAIComponent = GetComponentByClass<UEnemyAIComponent>())
	{
		if (EnemyAIComponent->InAttackRangeX_EnemyAI())
		{
			OnReachedEnemyX_EnemyAI();
		}
		else if (EnemyAIComponent->InAttackRangeY_EnemyAI())
		{
			OnReachedEnemyY_EnemyAI();
		}
	}
}

bool ABaseEnemy::CanMove_EnemyAI_Implementation()
{
	return !bDead || !bHurt || !bInAttackState || !bInAttackEffect;
}

bool ABaseEnemy::Dash_EnemyAI_Implementation()
{
	return false;
}

void ABaseEnemy::OnReachedEnemyX_EnemyAI_Implementation()
{
	
}

void ABaseEnemy::OnReachedEnemyY_EnemyAI_Implementation()
{
	
}

void ABaseEnemy::OnBeAttacked_Invulnerable_Implementation()
{
	IFight_Interface::OnBeAttacked_Invulnerable_Implementation();
}

int ABaseEnemy::DamagePlus_Implementation(int inValue, AActor* ActorDamaged)
{
	return 0;
}

int ABaseEnemy::OnDefendingHit_Implementation(int iniDamage)
{
	return iniDamage;
}

void ABaseEnemy::Jump()
{
	Super::Jump();
	SetJumping(true);
}

void ABaseEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	SetLanding(true);
}

void ABaseEnemy::Tick_KeepFaceToPixelCharacter(float DeltaSeconds)
{
	if (GetIsAttacking()) return;
	if (!IsAlive()) return;
	if (!PixelCharacter) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	float f = USpaceFuncLib::ActorAtActorRight(PixelCharacter,this) ? 180 : 0;

	FVector L;
	FRotator R; // 凑数，后面修改
	UGameplayStatics::GetPlayerCameraManager(World, 0 )->GetActorEyesViewPoint(L,R);
	R = UKismetMathLibrary::FindLookAtRotation (L, GetActorLocation());

	SetActorRotation(FRotator(0, f + R.Yaw - 90, 0));
}

void ABaseEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Tick_KeepFaceToPixelCharacter(DeltaSeconds);

	// 目前只有 HealthWidget
	if (UWidgetComponent* Widget = FindComponentByClass<UWidgetComponent>())
	{
		Widget->SetVisibility(IsValid(PixelCharacter));
	}
	
}
