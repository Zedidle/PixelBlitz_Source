// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "Components/WidgetComponent.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "Utilitys/SpaceFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/BasePixelAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyDataAsset.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/PXEnemyAttributeSet.h"
#include "Perception/PawnSensingComponent.h"
#include "PlayerState/PXPlayerState.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/DropSubsystem.h"
#include "Subsystems/PXAnimSubsystem.h"
#include "Utilitys/CommonFuncLib.h"

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
	
	if (ABasePXCharacter* C = Cast<ABasePXCharacter>(Character))
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
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bDead")), V);
	
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
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bHurt")), V);

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
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bInAttackState")), V);
	
	if (UBlackboardComponent* BlackboardComponent = GetController()->FindComponentByClass<UBlackboardComponent>())
	{
		BlackboardComponent->SetValueAsBool(FName("bInAttackState"), V);
	}
}

void ABaseEnemy::SetInAttackEffect(bool V)
{
	bInAttackEffect = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bInAttackEffect")), V);
}

void ABaseEnemy::SetAttackAnimToggle(const bool V)
{
	bAttackAnimToggle = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bAttackAnimToggle")), V);
}

void ABaseEnemy::SetInDefendState(const bool V)
{
	bInDefendState = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bInDefendState")), V);
}

void ABaseEnemy::SetDefendStart(const bool V)
{
	bDefendStart = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bDefendStart")), V);
}

void ABaseEnemy::SetDefendHurt(const bool V)
{
	bDefendHurt = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bDefendHurt")), V);
}

void ABaseEnemy::SetJumping(const bool V, const float time)
{
	bJumping = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bJumping")), V);
	
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

void ABaseEnemy::LoadEnemyData_Implementation(FName Level)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataAsset)
	if (!DataAsset->EnemyLevelDataTable.IsValid()) {
		DataAsset->EnemyLevelDataTable.LoadSynchronous();
	}
	UDataTable* DataTable = DataAsset->EnemyLevelDataTable.Get();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	UDataTableSubsystem* DataTableManager = GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	EnemyData = DataTableManager->FindRow<FEnemyData>(DataTable, EnemyLevel);

	HealthComponent->ModifyMaxHP(EnemyData.HP, EStatChange::Reset, true);
	SetActorScale3D(FVector(EnemyData.BodyScale));
	DropData = EnemyData.Drop;
	LoadLookDeterrence(EnemyData.LookDeterrence);

	BasicAttackDamage = EnemyData.AttackDamage;
	CurAttackDamage = BasicAttackDamage;
	AttackInterval = EnemyData.AttackInterval;
	
	EnemyAIComponent->AttackRange = EnemyData.AttackRange;
	GetCharacterMovement()->MaxAcceleration = EnemyData.MoveAcceleration;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData.MoveSpeed;

	BasicAttackRepel = EnemyData.AttackKnockbackForce;
	CurAttackRepel = BasicAttackRepel;

	PawnSensingComponent->SightRadius = EnemyData.SightRadius;
	LostEnemyTime = EnemyData.LostEnemyTime;

	HealthComponent->RepelResistance = EnemyData.RepelResistance;
}

void ABaseEnemy::SetLanding(const bool V, const float time)
{
	bLanding = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bLanding")), V);
	
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

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	FightComp = CreateDefaultSubobject<UFightComponent>(TEXT("FightComp"));
	EnemyAIComponent = CreateDefaultSubobject<UEnemyAIComponent>(TEXT("EnemyAIComponent"));
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	
	// 近战的默认方位，针对不同怪物需要重定义
	ActionFieldsCanAttack.AddTag(FGameplayTag::RequestGameplayTag(TEXT("ActionField.West.Near")));
	ActionFieldsCanAttack.AddTag(FGameplayTag::RequestGameplayTag(TEXT("ActionField.East.Near")));

	AbilitySystem = CreateDefaultSubobject<UPXEnemyASComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	
	AttributeSet = CreateDefaultSubobject<UPXEnemyAttributeSet>(TEXT("AttributeSetBase"));

}

void ABaseEnemy::Initialize_Implementation(FName Level)
{
	EnemyLevel = Level;
	LoadEnemyData(EnemyLevel);
}


void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	OnEnemyDeath.RemoveAll(this);
}

void ABaseEnemy::LoadLookDeterrence_Implementation(int32 Level)
{
}

void ABaseEnemy::OnDie_Implementation()
{
	UDropSubsystem* DropSubsystem = GetGameInstance()->GetSubsystem<UDropSubsystem>();
	DropSubsystem->SpawnItems(DropData, GetActorLocation(), 0.2f);

	if (APXPlayerState* PS = Cast<APXPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0)))
	{
		PS->RecoverPlayerState();
	}
	
	OnEnemyDeath.Broadcast(this);
}


void LoadLookDeterrence(int32 Level)
{
	
}


UAbilitySystemComponent* ABaseEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

bool ABaseEnemy::GetIsAttacking()
{
	return bInAttackEffect;
}

bool ABaseEnemy::GetIsDefending()
{
	return bInDefendState;
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
		FGameplayTag ActionField = EnemyAIComponent->GetActionFieldByPlayer();
		if (!ActionFieldsCanAttack.HasTag(ActionField)) return false;
		
		float distance = GetDistanceToPlayer();
		return EnemyAIComponent->AttackRange.X < distance && distance < EnemyAIComponent->AttackRange.Y;
	}
	return false;
}

bool ABaseEnemy::CanAttack_Implementation()
{
	if (!IsValid(PixelCharacter)) return false;
	if (!Execute_IsAlive(PixelCharacter)) return false;
	
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
	if (GetIsDefending())
	{
		UCommonFuncLib::SpawnFloatingTextDefault(TEXT("Ability/AbilityPassive"), TEXT("Defend"), GetActorLocation(),
			FLinearColor(0.491, 0.672, 0.672));
		SetDefendHurt(true);
		return iniDamage / 2;
	}
	return iniDamage;
}

void ABaseEnemy::OnDieEnd_Implementation()
{
	IFight_Interface::OnDieEnd_Implementation();
}

void ABaseEnemy::OnRemoteAttackEffect_Implementation()
{
	IFight_Interface::OnRemoteAttackEffect_Implementation();
}

void ABaseEnemy::OnRemoteAttackEnd_Implementation()
{
	IFight_Interface::OnRemoteAttackEnd_Implementation();
}

void ABaseEnemy::OnDefendEffectBegin_Implementation()
{
	IFight_Interface::OnDefendEffectBegin_Implementation();
}

void ABaseEnemy::OnDefendEffectEnd_Implementation()
{
	IFight_Interface::OnDefendEffectEnd_Implementation();
}

void ABaseEnemy::OnAttackEffectBegin_Implementation()
{
	IFight_Interface::OnAttackEffectBegin_Implementation();
}

void ABaseEnemy::OnAttackEffectEnd_Implementation()
{
	IFight_Interface::OnAttackEffectEnd_Implementation();
}

UAbilityComponent* ABaseEnemy::GetAbilityComponent_Implementation()
{
	return AbilityComponent;
}

APawn* ABaseEnemy::GetPawn_Implementation()
{
	return this;
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
	FGameplayTag ActionField = EnemyAIComponent->GetActionFieldByPlayer();
	if (ActionField.GetTagName() == TEXT("ActionField.East.Near") ||
		ActionField.GetTagName() == TEXT("ActionField.East.Mid"))
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
	else if (ActionField.GetTagName() == TEXT("ActionField.West.Near") ||
		ActionField.GetTagName() == TEXT("ActionField.West.Mid"))
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
