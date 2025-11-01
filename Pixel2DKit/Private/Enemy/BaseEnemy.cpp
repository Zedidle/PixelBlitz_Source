// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"

#include "NiagaraFunctionLibrary.h"
#include "PaperFlipbookComponent.h"
#include "StaticMeshAttributes.h"
#include "Components/WidgetComponent.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "Utilitys/SpaceFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/BasePXAnimInstance.h"
#include "Core/PXGameState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyDataAsset.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/PXEnemyAttributeSet.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/PXCharacterPlayerState.h"
#include "Player/PXPlayerState.h"
#include "Settings/Config/EnemyActionMoveDataAsset.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Sound/SoundCue.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/DropSubsystem.h"
#include "Subsystems/EnemyAISubsystem.h"
#include "Subsystems/PXAnimSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/SoundFuncLib.h"


#define LOCTEXT_NAMESPACE "PX"

AActor* ABaseEnemy::GetPixelCharacter()
{
	return Cast<AActor>(PXCharacter);
}

void ABaseEnemy::SetPXCharacter(AActor* Character)
{
	if (Character == nullptr)
	{
		PXCharacter = nullptr;
		if (IsValid(EnemyAIComponent))
		{
			EnemyAIComponent->SetPXCharacter(nullptr);
		}
		if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(GetController()))
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("PlayerPawn"), nullptr);
		}
		return;
	}

	if (IsValid(PXCharacter)) return;

	if (ABasePXCharacter* C = Cast<ABasePXCharacter>(Character))
	{
		PXCharacter = C;
		if (IsValid(EnemyAIComponent))
		{
			EnemyAIComponent->SetPXCharacter(PXCharacter);
		}
		if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(GetController()))
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("PlayerPawn"), PXCharacter);
		}
	}
}

void ABaseEnemy::OnSensingPlayer(AActor* PlayerActor)
{
	if (PXCharacter == nullptr)
	{
		// 受惊
		// 后续可能要配置受惊距离
		if (PlayerActor->GetDistanceTo(this) < 100.0f)
		{
			SetActionMove(GetHorizontalDirectionToPlayer() * -20, "Startled", 0.5, true);
		}
	}

	SetPXCharacter(PlayerActor);
	BP_OnSensingPlayer(PlayerActor);
}

void ABaseEnemy::OnLostPlayer(AActor* PlayerActor)
{
	if (PlayerActor != PXCharacter) return;
	DelayLosePlayer();
}

void ABaseEnemy::GoPatrol()
{
	SetAttackAnimToggle(false);
	SetInAttackState(false);
	SetPXCharacter(nullptr);
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

		ActionMove.bActionMoving = false;
		// ActionMove图中被击落
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->SetActive(true, false);
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}
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

float ABaseEnemy::GetHorizontalDistanceToPlayer() const
{
	if (IsValid(PXCharacter))
	{
		return (PXCharacter->GetActorLocation() - GetActorLocation()).Size2D();
	}
	return 0;
}

float ABaseEnemy::GetVerticalDistanceToPlayer() const
{
	if (IsValid(PXCharacter))
	{
		return PXCharacter->GetActorLocation().Z - GetActorLocation().Z;
	}
	return 0;
}

void ABaseEnemy::LoadEnemyData_Implementation(FName Level)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataAsset)

	UDataTable* DataTable = DataAsset->EnemyLevelDataTable.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	UDataTableSubsystem* DataTableManager = GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	EnemyData = DataTableManager->FindRow<FEnemyData>(DataTable, EnemyLevel);

	HealthComponent->ModifyMaxHP(EnemyData.HP, EStatChange::Reset, true);
	SetActorScale3D(FVector(EnemyData.BodyScale));

	LoadLookDeterrence(EnemyData.LookDeterrence);

	CurAttackDamage = EnemyData.AttackDamage;
	BasicAttackInterval = EnemyData.AttackInterval;
	
	ActionFieldsCanAttack = FGameplayTagContainer::CreateFromArray(DataAsset->ActionFieldsCanAttack);
	EnemyAIComponent->SetActionFieldDistance(EnemyData.ActionFieldDistance);
	GetCharacterMovement()->MaxAcceleration = EnemyData.MoveAcceleration;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData.MoveSpeed;

	CurAttackRepel = EnemyData.AttackKnockbackForce;

	if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Controller) )
	{
		EnemyAIController->UpdateSightRadius(EnemyData.SightRadius);
	}
	
	LostEnemyTime = EnemyData.LostEnemyTime;

	HealthComponent->RepelResistance = EnemyData.RepelResistance;

	
#pragma region GAS

	InitAbilitiesToGive.Append(DataAsset->InitAbilitiesToGive);
	InitEffects.Append(DataAsset->InitEffects);
	if (AbilitySystem)
	{
		for (auto Ability : InitAbilitiesToGive)
		{
			AbilitySystem->K2_GiveAbility(Ability);
		}

		FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		
		for (auto Effect : InitEffects)
		{
			if (!Effect) continue;
			UGameplayEffect* GameplayEffect = Effect->GetDefaultObject<UGameplayEffect>();
			FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
				Effect, 
				1.0f, 
				EffectContext
			);
        
			if (SpecHandle.IsValid())
			{
				// 应用效果到自身
				AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}	
	}
#pragma endregion
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
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	FightComponent = CreateDefaultSubobject<UFightComponent>(TEXT("FightComp"));
	EnemyAIComponent = CreateDefaultSubobject<UEnemyAIComponent>(TEXT("EnemyAIComponent"));
	// AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));

	AbilitySystem = CreateDefaultSubobject<UPXEnemyASComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	
	AttributeSet = CreateDefaultSubobject<UPXEnemyAttributeSet>(TEXT("AttributeSetBase"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	if (GetSprite())
	{
		GetSprite()->CastShadow = true;
	}
}

bool ABaseEnemy::IsActionMoving() const
{
	return ActionMove.bActionMoving;
}

void ABaseEnemy::Initialize_Implementation(FName Level)
{
	EnemyLevel = Level;
	LoadEnemyData(EnemyLevel);
}


void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnHPChanged.AddDynamic(this, &ABaseEnemy::OnEnemyHPChanged);
	}
}

void ABaseEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	OnEnemyDie.RemoveAll(this);
	
	if (HealthComponent)
	{
		HealthComponent->OnHPChanged.RemoveDynamic(this, &ABaseEnemy::OnEnemyHPChanged);
	}
}

void ABaseEnemy::SetActionMove(const FVector& MoveVector,  const FName& CurveName, float SustainTime, bool bInterrupt, bool bCabBeInterrupt)
{
	if (!GetCapsuleComponent()) return;
	if (CurveName.IsNone()) return;
	if (SustainTime <= 0.0f) return;
	if (ActionMove.bActionMoving && !ActionMove.bCabBeInterrupt) return;
	if (ActionMove.bActionMoving && !bInterrupt) return;

	UEnemyAISubsystem* EnemyAISubsystem = GetGameInstance()->GetSubsystem<UEnemyAISubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(EnemyAISubsystem)

	const FCurveFloatData& ActionMoveCurveData = EnemyAISubsystem->GetActionMoveCurveData(CurveName);
	UCurveVector* Curve = ActionMoveCurveData.Curve.LoadSynchronous();
	if (!IsValid(Curve)) return;
	
	if (ActionMoveCurveData.Curve.IsNull()) return;

	FVector StartLocation = GetActorLocation();
	if (ActionMoveCurveData.CheckBehindDistance > 0)
	{
		FHitResult OutHit;
		FVector CheckBehindLocation = StartLocation - ActionMoveCurveData.CheckBehindDistance * MoveVector.GetSafeNormal2D();
		bool bBlock = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation,CheckBehindLocation,
		TraceTypeQuery1, false, {this},
				EDrawDebugTrace::None, OutHit, true,
				FLinearColor::Red, FLinearColor::Green, 1.0f);

		if (bBlock) return;
	}

	float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector Direction = MoveVector.GetSafeNormal2D();
	FVector TargetLocation = USpaceFuncLib::GetHorizontalFarestPosition(GetActorLocation(), Direction, MoveVector.Size2D(), 50, Radius);


	if (EnemyAIComponent)
	{
		EnemyAIComponent->MoveCheckAllies(TargetLocation, TargetLocation);
	}

	ActionMove.bActionMoving = true;
	ActionMove.CurTime = 0;
	ActionMove.SustainTime = SustainTime;
	ActionMove.StartLocation = StartLocation;
	ActionMove.TargetLocation = TargetLocation;
	ActionMove.CurveVector = Curve;
	ActionMove.bCabBeInterrupt = bCabBeInterrupt;
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetActive(false, false);
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void ABaseEnemy::TryJumpToOtherPlatform(const FVector& StartLocation, const FVector& TargetLocation)
{
	if (ActionMove.bActionMoving) return;
	if (!GetCapsuleComponent()) return;
	if (EnemyData.MoveSpeed <= 0.0f) return;
	
	float JumpDuration = FMath::FRandRange(0.1f, 0.2f) + FMath::FRandRange(0.5f, 0.55f) * FVector::Dist2D(StartLocation, TargetLocation) / EnemyData.MoveSpeed;
	
	float VerticalDistanceToPlayer = GetVerticalDistanceToPlayer();
	
	FName CurveName;
	if (FMath::Abs(VerticalDistanceToPlayer) < 50)
	{
		CurveName = "JumpHorizontal";
	}
	else if (VerticalDistanceToPlayer >= 50 && VerticalDistanceToPlayer <= 150)
	{
		FHitResult HitResult_Ceil;
		bool bHitCeil = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, StartLocation + FVector(0,0,150),
		TraceTypeQuery1, false, {},
				EDrawDebugTrace::None, HitResult_Ceil, true,
				FLinearColor::Black, FLinearColor::Gray, 1.0f);

		FVector HalfHorizontalVector = (TargetLocation - StartLocation).GetSafeNormal2D() * (TargetLocation - StartLocation).Size2D() / 2;
		HalfHorizontalVector.Z = 100;

		bHitCeil |= UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, StartLocation + HalfHorizontalVector,
		TraceTypeQuery1, false, {},
				EDrawDebugTrace::None, HitResult_Ceil, true,
				FLinearColor::Red, FLinearColor::Green, 1.0f);
		
		if (!bHitCeil)
		{
			CurveName = "JumpToHigher";
		}
	}
	else if (VerticalDistanceToPlayer  <= -50 && VerticalDistanceToPlayer >= -150)
	{
		CurveName = "JumpToLower";
	}

	if (CurveName.IsNone()) return;
	
	UEnemyAISubsystem* EnemyAISubsystem = GetGameInstance()->GetSubsystem<UEnemyAISubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(EnemyAISubsystem)

	const FCurveFloatData& ActionMoveCurveData = EnemyAISubsystem->GetActionMoveCurveData(CurveName);
	UCurveVector* Curve = ActionMoveCurveData.Curve.LoadSynchronous();
	if (!IsValid(Curve)) return;
	
	if (ActionMoveCurveData.Curve.IsNull()) return;

	ActionMove.bActionMoving = true;
	ActionMove.CurTime = 0;
	ActionMove.SustainTime = JumpDuration;
	ActionMove.StartLocation = StartLocation;
	ActionMove.TargetLocation = TargetLocation;
	ActionMove.CurveVector = Curve;
	ActionMove.bCabBeInterrupt = false;
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetActive(false, false);
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
	
}

float ABaseEnemy::GetDefaultHalfHeight() const
{
	if (GetCapsuleComponent())
	{
		return GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	return Super::GetDefaultHalfHeight();
}


void ABaseEnemy::LoadLookDeterrence_Implementation(int32 Level)
{
}

void ABaseEnemy::OnDie_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	if (AEnemyAIController* EnemyAI = GetController<AEnemyAIController>())
	{
		EnemyAI->StopAI();
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
	
	UDropSubsystem* DropSubsystem = GetGameInstance()->GetSubsystem<UDropSubsystem>();
	DropSubsystem->SpawnItems(EnemyData.DropID_Rate, GetActorLocation());

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerPawn, ECR_Overlap);
	}
	
	SetInAttackEffect(false);
	SetInAttackState(false);
	
	if (APXCharacterPlayerState* PS = Cast<APXCharacterPlayerState>(UGameplayStatics::GetPlayerState(World, 0)))
	{
		PS->OnEnemyDie(this);
	}
	if (APXGameState* GS = Cast<APXGameState>(UGameplayStatics::GetGameState(World)))
	{
		GS->OnEnemyDie(this);
	}

	if (GetSprite())
	{
		GetSprite()->SetSpriteColor(FLinearColor::Gray);
	}
	OnEnemyDie.Broadcast(this);
}


void LoadLookDeterrence(int32 Level)
{
	
}


void ABaseEnemy::OnEnemyHPChanged_Implementation(int32 OldValue, int32 NewValue)
{
	if (bDead) return;
	if (OldValue == NewValue) return;
	
	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(HealthComponent)
	
	if (NewValue > 0)
	{
		OnHurt(NewValue);
		
		if (NewValue < OldValue)
		{
			if (OldValue - NewValue > HealthComponent->GetMaxHP() * 0.1)
			{
				SetHurt(true, 0.2f);
			}
		}
	}
	else
	{
		if (DataAsset)
		{
			if (USoundCue* DieSound = DataAsset->DieSound.LoadSynchronous())
			{
				USoundFuncLib::PlaySoundAtLocation(DieSound, GetActorLocation());
			}
			if (UNiagaraSystem* NS_Die = DataAsset->NS_Die.LoadSynchronous())
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Die, GetActorLocation());
			}
		}

		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			// 运行一颗空树
			// AIController->RunBehaviorTree();
			
			if (AIController->GetBlackboardComponent())
			{
				AIController->GetBlackboardComponent()->SetValueAsBool("bDead", true);
			}
		}
		SetDead(true);
		Execute_OnDie(this);
		if (GetSprite())
		{
			GetSprite()->SetLooping(false);
		}

	}
}

void ABaseEnemy::OnHurt_Implementation(int RemainHP)
{
	InjuredNum++;
	if (DataAsset)
	{
		if (USoundCue* HurtSound = DataAsset->HurtSound.LoadSynchronous())
		{
			USoundFuncLib::PlaySoundAtLocation(HurtSound, GetActorLocation());
		}

		if (UNiagaraSystem* NS_Hurt = DataAsset->NS_Hurt.LoadSynchronous())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Hurt, GetActorLocation());
		}
	}
}

FVector ABaseEnemy::GetHorizontalDirectionToPlayer()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, FVector::ZeroVector)

	if (PXCharacter)
	{
		return (PXCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	}

	return FVector::ZeroVector;
}

UAbilitySystemComponent* ABaseEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void ABaseEnemy::BuffUpdate_Attack_Implementation()
{
	if (BuffComponent)
	{
		CurAttackDamage = EnemyData.AttackDamage * (1 + BuffComponent->EffectedPercent_Attack) + BuffComponent->EffectedValue_Attack;
	}
}

void ABaseEnemy::BuffUpdate_Speed_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MovementComponent);

	MovementComponent->MaxWalkSpeed = EnemyData.MoveSpeed * (BuffComponent->EffectedPercent_Speed + 1.0f) + BuffComponent->EffectedValue_Speed;
	MovementComponent->MaxAcceleration = EnemyData.MoveAcceleration * (BuffComponent->EffectedPercent_Speed + 1.0f);

	if (MovementComponent->MaxWalkSpeed < EnemyData.MoveSpeed)
	{
		CustomTimeDilation = MovementComponent->MaxWalkSpeed / EnemyData.MoveSpeed;
	}
}

void ABaseEnemy::BuffUpdate_Sight_Implementation()
{
	if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Controller) )
	{
		EnemyAIController->UpdateSightRadius(EnemyData.SightRadius * (BuffComponent->EffectedPercent_Sight + 1.0f) + BuffComponent->EffectedValue_Sight);
	}
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
	return PXCharacter;
}


bool ABaseEnemy::CanMove_Implementation()
{
	return !bDead && !bHurt && !bInAttackState && !bInAttackEffect;
}

bool ABaseEnemy::InAttackRange()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(EnemyAIComponent, false)
	if (!InAttackRange_Vertical()) return false;
	
	FGameplayTag ActionField = EnemyAIComponent->GetActionFieldByPlayer();
	if (!ActionFieldsCanAttack.HasTag(ActionField)) return false;

	float distance = GetHorizontalDistanceToPlayer();
	for (auto& range: EnemyAIComponent->AttackRange)
	{
		if (range.X < distance && distance < range.Y)
		{
			return true;
		}
	}
	
	return false;
}

bool ABaseEnemy::InAttackRange_Vertical()
{
	return FMath::Abs(GetVerticalDistanceToPlayer()) < GetDefaultHalfHeight() * 2;
}

void ABaseEnemy::DelayLosePlayer_Implementation()
{
	FName TimerName = FName(GetActorNameOrLabel() + "_DelayLosePlayer");
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
		[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;
			
			WeakThis->GoPatrol();
		}, LostEnemyTime);
}

void ABaseEnemy::OnAttack_Implementation()
{
	if (FightComponent)
	{
		if (GetSprite())
		{
			FightComponent->MeleeTraceAttack(FName("HitSocket"), GetSprite(),
				Execute_GetAttackDamage(this), Execute_GetAttackRepel(this));
		}
	}
}

bool ABaseEnemy::CanAttack_Implementation()
{
	if (!IsValid(PXCharacter)) return false;
	if (!Execute_IsAlive(PXCharacter)) return false;
	
	return !bDead && !bInAttackState;
}

bool ABaseEnemy::Dash_Implementation()
{
	return false;

}

void ABaseEnemy::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Maker)
	
	OutDamage = InDamage;
	if (Execute_GetOwnCamp(this).HasTag(
				FGameplayTag::RequestGameplayTag("Enemy.BOSS")))
	{
		float Result;
		if (Maker->Implements<UFight_Interface>() && Execute_FindEffectGameplayTag(Maker,
			FGameplayTag::RequestGameplayTag("AbilitySet.ToBossDamagePlusPercent"),
			Result))
		{
			OutDamage *= 1 + Result;
		}
	}

	// if (AbilityComponent)
	// {
	// 	AbilityComponent->OnBeAttacked(Maker, OutDamage, OutDamage, bForce);
	// }

	// 暂时默认防御减半伤害
	if (bInDefendState)
	{
		OutDamage *= 0.5;
	}

	OnSensingPlayer(Maker);
}




void ABaseEnemy::OnBeAttacked_Invulnerable_Implementation()
{
	IFight_Interface::OnBeAttacked_Invulnerable_Implementation();
}

int ABaseEnemy::DamagePlus_Implementation(int InDamage, AActor* Receiver)
{
	return 0;
}

void ABaseEnemy::PowerRepulsion_Implementation(float Power)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		UTimerSubsystemFuncLib::SetDelay(this, [WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}, Power / 6000);
	}
}

int ABaseEnemy::OnDefendingHit_Implementation(int iniDamage)
{
	if (GetIsDefending())
	{
		UCommonFuncLib::SpawnFloatingText(
			LOCTEXT("AbilityPassive_Defend", "防御"),
			GetActorLocation(),
			FLinearColor(0.491, 0.672, 0.672));
		
		SetDefendHurt(true);
		return iniDamage / 2;
	}
	return iniDamage;
}

void ABaseEnemy::OnDefendingHitEffect_Implementation()
{
	SetDefendHurt(false);
}

void ABaseEnemy::OnAnimDieEnd_Implementation()
{
	GetCharacterMovement()->DestroyComponent();
}

void ABaseEnemy::OnRemoteAttackEffect_Implementation()
{
	IFight_Interface::OnRemoteAttackEffect_Implementation();
}

void ABaseEnemy::OnRemoteAttackEnd_Implementation()
{
	FName TimerName = FName(GetActorNameOrLabel() + "_InAttackState");
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), TimerName, 
	[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->SetInAttackState(false);
		}, BasicAttackInterval);
}

void ABaseEnemy::OnDefendEffectBegin_Implementation()
{
	SetInDefendState(true);
	SetDefendStart(false);
}

void ABaseEnemy::OnDefendEffectEnd_Implementation()
{
	SetInDefendState(false);
}

void ABaseEnemy::OnAttackEffectBegin_Implementation()
{
	if (bDead)
	{
		SetInAttackState(false);
		return;
	}

	SetInAttackEffect(true);
	OnAttack();
	DelayLosePlayer();
}

void ABaseEnemy::OnAttackEffectEnd_Implementation()
{
	SetInAttackEffect(false);
	FName TimerName = FName(GetActorNameOrLabel() + "_InAttackState");
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), TimerName, 
	[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->SetInAttackState(false);
		}, BasicAttackInterval);
}


bool ABaseEnemy::FindEffectGameplayTag_Implementation(const FGameplayTag Tag, float& Result)
{
	if (EffectGameplayTags.Contains(Tag))
	{
		Result = EffectGameplayTags[Tag];
		return true;
	}
	Result = 0;
	return false;
}


APawn* ABaseEnemy::GetPawn_Implementation()
{
	return this;
}

float ABaseEnemy::GetAttackInterval_Implementation()
{
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag("AbilitySet.AttackAccPercent");
	if (EffectGameplayTags.Contains(Tag))
	{
		return BasicAttackInterval / (1 + EffectGameplayTags[Tag]);
	}

	return BasicAttackInterval;
}

int ABaseEnemy::GetAttackDamage_Implementation()
{
	return CurAttackDamage;
}

FVector ABaseEnemy::GetAttackRepel_Implementation()
{
	return CurAttackRepel;
}

void ABaseEnemy::OnAnimVulnerableBegin_Implementation()
{
	if (HealthComponent)
	{
		HealthComponent->SetInvulnerable(true);
	}
}

void ABaseEnemy::OnAnimVulnerableEnd_Implementation()
{
	if (HealthComponent)
	{
		HealthComponent->SetInvulnerable(false);
	}
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
	
	UPXASComponent* ASC = Cast<UPXASComponent>(GetAbilitySystemComponent());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC)

	if (ASC->TryActivateAbilityByTagName("Ability.NormalAttack"))
	{
		SetAttackAnimToggle(true);
		SetInAttackState(true);
	
		GetCharacterMovement()->StopMovementImmediately();
	}
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
	TryAttack();
}

void ABaseEnemy::ActionAtPlayerSouthFar_Implementation(float Distance)
{
}

void ABaseEnemy::ActionAtPlayerSouthRemote_Implementation(float Distance)
{
}


void ABaseEnemy::Tick_KeepFaceToPixelCharacter(float DeltaSeconds)
{
	if (!Execute_IsAlive(this)) return;
	if (!IsValid(PXCharacter)) return;

	bool PlayerAtRight = USpaceFuncLib::ActorAtActorRight(PXCharacter,this);

	FVector Dir_PlayerRight = PXCharacter->GetRightVectorWithBlendYaw();
	FVector Dir_PlayerToSelf = (PXCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	float DotValue = Dir_PlayerRight.Dot(Dir_PlayerToSelf);
	float Degree = FMath::Acos(DotValue) * 180 / PI;

	float CrossZ = Dir_PlayerRight.Cross(Dir_PlayerToSelf).Z;

	EWorldDirection RelativeDirection_PlayerToSelf = USpaceFuncLib::ActorAtActorWorldDirection(EnemyAIComponent->PXCharacter, this, PXCharacter->CurBlendYaw);

	float InYaw;
	if (RelativeDirection_PlayerToSelf == North)
	{
		InYaw = PlayerAtRight ? PXCharacter->CurBlendYaw - 45 : PXCharacter->CurBlendYaw - 135;
	}
	else if (RelativeDirection_PlayerToSelf == South)
	{
		InYaw = PlayerAtRight ? PXCharacter->CurBlendYaw + 45 : PXCharacter->CurBlendYaw + 135;
	}
	else
	{
		// 东 或 西
		InYaw = CrossZ > 0 ? PXCharacter->CurBlendYaw + Degree : PXCharacter->CurBlendYaw - Degree;
	}
	
	SetActorRotation(FRotator(0, InYaw, 0));
}

void ABaseEnemy::Tick_ActionMove(float DeltaSeconds)
{
	if (bDead) return;
	if (!ActionMove.bActionMoving) return;
	if (!ActionMove.CurveVector) return;

	ActionMove.CurTime += DeltaSeconds;
	float MovePercent = ActionMove.CurTime / ActionMove.SustainTime;

	if (MovePercent <= 1)
	{

		FVector CurCurveVector = ActionMove.CurveVector->GetVectorValue(MovePercent);
		FVector CurLocation;
		CurLocation.X = FMath::Lerp(ActionMove.StartLocation.X , ActionMove.TargetLocation.X, CurCurveVector.X);
		CurLocation.Y = FMath::Lerp(ActionMove.StartLocation.Y , ActionMove.TargetLocation.Y, CurCurveVector.Y);

		if (CurCurveVector.Z >= 1.5)
		{
			// 水平小跳模式
			CurLocation.Z = FMath::Lerp(ActionMove.StartLocation.Z , ActionMove.TargetLocation.Z, MovePercent) + CurCurveVector.Z;
		}
		else
		{
			// 跨平台跳跃模式
			float P = FMath::Max(50, FMath::Abs(ActionMove.StartLocation.Z - ActionMove.TargetLocation.Z) / 2);
			CurLocation.Z = FMath::Lerp(ActionMove.StartLocation.Z , ActionMove.TargetLocation.Z, MovePercent) + CurCurveVector.Z * P;
		}

		float HalfHeight = GetDefaultHalfHeight();
		// 为了避陷入浮动的地面
		FHitResult OutHit;
		bool bHitFloor = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CurLocation,
			CurLocation + FVector(0, 0, -HalfHeight),
		TraceTypeQuery1, false, {this},
				EDrawDebugTrace::None, OutHit, true,
				FLinearColor::Yellow, FLinearColor::Blue, 1.0f);
		
		if (bHitFloor)
		{
			CurLocation.Z =  OutHit.Location.Z + HalfHeight;
		}
		
		SetActorLocation(CurLocation, false);
	}
	else
	{
		ActionMove.bActionMoving = false;
		ActionMove.bCabBeInterrupt = true;
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->SetActive(true, false);
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}
	}
}

void ABaseEnemy::Tick_SnapOnPlatform(float DeltaSeconds)
{
	if (!bDead) return;

	FVector CurLocation = GetActorLocation();
	float HalfHeight = GetDefaultHalfHeight();
	
	FHitResult OutHit;
	bool bHitFloor = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CurLocation,
		CurLocation + FVector(0, 0, -5 * HalfHeight),
	TraceTypeQuery1, false, {this},
			EDrawDebugTrace::None, OutHit, true,
			FLinearColor::Yellow, FLinearColor::Blue, 1.0f);

	AddActorWorldOffset(FVector(0, 0, HalfHeight - OutHit.Distance));
}

void ABaseEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Tick_KeepFaceToPixelCharacter(DeltaSeconds);
	Tick_ActionMove(DeltaSeconds);
	Tick_SnapOnPlatform(DeltaSeconds);
	
	// 目前只有 HealthWidget
	if (UWidgetComponent* Widget = FindComponentByClass<UWidgetComponent>())
	{
		Widget->SetVisibility(IsValid(PXCharacter));
	}

	

	
}






#undef LOCTEXT_NAMESPACE