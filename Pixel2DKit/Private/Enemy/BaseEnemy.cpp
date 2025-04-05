// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "Components/WidgetComponent.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "FunctionLibrary/SpaceFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Anim/BasePixelAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystems/PixelAnimSubsystem.h"

AActor* ABaseEnemy::GetPixelCharacter()
{
	return Cast<AActor>(PixelCharacter);
}

bool ABaseEnemy::SetPixelCharacter(AActor* Character)
{
	if (Character == nullptr)
	{
		PixelCharacter = nullptr;
		if (IsValid(EnemyAIComponent))
		{
			EnemyAIComponent->SetPixelCharacter(PixelCharacter);
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
		if (IsValid(EnemyAIComponent))
		{
			EnemyAIComponent->SetPixelCharacter(PixelCharacter);
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

	if (bDead)
	{
		bAttackAnimToggle = false;
		bInAttackState = false;
		bInAttackEffect = false;
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
	bInAttackState = V;
	// UPixelAnimSubsystem::SetAnimProperty();
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetInAttackState(V);
	}
	if (UBlackboardComponent* BlackboardComponent = GetController()->FindComponentByClass<UBlackboardComponent>())
	{
		BlackboardComponent->SetValueAsBool(FName("bInAttackState"), V);
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

void ABaseEnemy::SetAttackAnimToggle(const bool V)
{
	bAttackAnimToggle = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetAttackAnimToggle(V);
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

float ABaseEnemy::GetDistanceToPlayer() const
{
	if (IsValid(PixelCharacter))
	{
		return (PixelCharacter->GetActorLocation() - GetActorLocation()).Size();
	}
	return 99999;
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


ABaseEnemy::ABaseEnemy(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	HealthComponent_CPP = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent_CPP"));
	FightComp = CreateDefaultSubobject<UFightComponent>(TEXT("FightComp"));
	EnemyAIComponent = CreateDefaultSubobject<UEnemyAIComponent>(TEXT("EnemyAIComponent"));

	// 近战的默认方位，针对不同怪物需要重定义
	ActionFieldsCanAttack = { WestNear, EastNear };
}


bool ABaseEnemy::GetIsAttacking()
{
	return bInAttackEffect;
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

bool ABaseEnemy::CanMove_Implementation()
{
	return !bDead && !bHurt && !bInAttackState && !bInAttackEffect;
}

bool ABaseEnemy::InAttackRange()
{
	if (IsValid(EnemyAIComponent))
	{
		EActionField Field = EnemyAIComponent->GetActionFieldByPlayer();
		if (!ActionFieldsCanAttack.Contains(Field)) return false;
		
		float distance = GetDistanceToPlayer();
		return EnemyAIComponent->AttackRange.X < distance && distance < EnemyAIComponent->AttackRange.Y;
	}
	return false;
}

bool ABaseEnemy::CanAttack_Implementation()
{
	if (!IsValid(PixelCharacter)) return false;
	if (!IFight_Interface::Execute_IsAlive(PixelCharacter)) return false;
	
	return !bDead && !bInAttackState;
}

bool ABaseEnemy::Dash_Implementation()
{
	return false;

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

void ABaseEnemy::TryAttack_Implementation()
{
	if (!Execute_CanAttack(this)) return;
	if (!InAttackRange()) return;
	
	SetAttackAnimToggle(true);
	SetInAttackState(true);
	GetCharacterMovement()->StopMovementImmediately();
}



// 最基本的东侧近战判定
void ABaseEnemy::ActionAtPlayerEastNear_Implementation(float Distance)
{
	TryAttack();
	// 或者远离 / 接近玩家
	// …… 任意行动
}

void ABaseEnemy::ActionAtPlayerEastMid_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerEastFar_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerEastRemote_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerWestNear_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerWestMid_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerWestFar_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerWestRemote_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerNorthNear_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerNorthMid_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerNorthFar_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerNorthRemote_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerSouthNear_Implementation(float Distance)
{
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerSouthMid_Implementation(float Distance)
{
}

void ABaseEnemy::ActionAtPlayerSouthFar_Implementation(float Distance)
{
}

void ABaseEnemy::ActionAtPlayerSouthRemote_Implementation(float Distance)
{
}


void ABaseEnemy::Tick_KeepFaceToPixelCharacter(float DeltaSeconds)
{
	if (GetIsAttacking()) return;
	if (!Execute_IsAlive(this)) return;
	if (!IsValid(PixelCharacter)) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	float f = USpaceFuncLib::ActorAtActorRight(PixelCharacter,this) ? 180 : 0;

	FVector L;
	FRotator R; // 凑数，后面修改
	UGameplayStatics::GetPlayerCameraManager(World, 0 )->GetActorEyesViewPoint(L,R);
	R = UKismetMathLibrary::FindLookAtRotation (L, GetActorLocation());


	FVector PlayerForward = PixelCharacter->GetMoveForwardVector();
	FVector PlayerToSelf = (GetActorLocation() - PixelCharacter->GetActorLocation()).GetSafeNormal2D();
	float c = PlayerForward.Dot(PlayerToSelf);
	float d = FMath::Acos(c) * 180 / PI;

	float z = PlayerForward.Cross(PlayerToSelf).Z;
	
	float yaw = f + R.Yaw - 90;
	
	// 判断怪物是否在玩家的东侧或西侧
	EActionField ActionField = EnemyAIComponent->GetActionFieldByPlayer();
	if (ActionField == EastNear || ActionField == EastMid)
	{
		if (z > 0)
		{
			SetActorRotation(FRotator(0, yaw + d, 0));
		}
		else
		{
			SetActorRotation(FRotator(0, yaw - d, 0));
		}
	}
	else if (ActionField == WestNear || ActionField == WestMid)
	{
		if (z > 0)
		{
			SetActorRotation(FRotator(0, yaw + d + 180, 0));
		}
		else
		{
			SetActorRotation(FRotator(0, yaw - d + 180, 0));
		}
	}
	else
	{
		SetActorRotation(FRotator(0, yaw, 0));
	}
	
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
