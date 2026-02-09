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
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyDataAsset.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/StateComponent.h"
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
#include "Subsystems/TimerManagerFuncLib.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/SoundFuncLib.h"


#define LOCTEXT_NAMESPACE "PX"

AActor* ABaseEnemy::GetPixelCharacter()
{
	return Cast<AActor>(PXCharacter);
}

void ABaseEnemy::SetPXCharacter(ABasePXCharacter* Character)
{
	if (Character == nullptr || !Execute_IsAlive(Character))
	{
		if (!PXCharacter.IsValid()) return;

		PXCharacter->OnPlayerDie.RemoveDynamic(this, &ThisClass::Event_OnPlayerDie);
		
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

	if (PXCharacter.IsValid()) return;

	PXCharacter = Character;
	PXCharacter->OnPlayerDie.AddUniqueDynamic(this, &ThisClass::Event_OnPlayerDie);
	
	if (IsValid(EnemyAIComponent))
	{
		EnemyAIComponent->SetPXCharacter(PXCharacter.Get());
	}
	if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(GetController()))
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("PlayerPawn"), PXCharacter.Get());
	}
}

void ABaseEnemy::Event_OnPlayerDie()
{
	SetPXCharacter(nullptr);
}

void ABaseEnemy::OnSensingPlayer(AActor* PlayerActor)
{
	bool bPrePlayerNull = PXCharacter == nullptr;
	SetPXCharacter(Cast<ABasePXCharacter>(PlayerActor));
	
	if (bPrePlayerNull && Execute_CanMove(this))
	{
		if (PlayerActor->GetDistanceTo(this) < 100.0f)
		{
			SetActionMove(GetHorizontalDirectionToPlayer() * -20, "Startled", FMath::RandRange(0.3f, 0.4f), true);
		}
	}

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

void ABaseEnemy::SetHurt(bool V, float Duration)
{
	bHurt = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bHurt")), V);

	if (!bHurt) return;

	FName TimerName = FName(GetName() + "_ABaseEnemy::SetHurt");

	// float RemainingTime = UTimerManagerFuncLib::GetRemainingTime(GetWorld(), TimerName);
	// if (RemainingTime > 0)
	// {
	// 	if (RemainingTime > Duration)
	// 	{
	// 		Duration = Duration / 2 + RemainingTime;
	// 	}
	// 	else
	// 	{
	// 		Duration = RemainingTime / 2 + Duration;
	// 	}
	// }
	
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), TimerName, [WeakThis = TWeakObjectPtr(this)]
	{
		if (WeakThis.IsValid())
		{
			WeakThis->SetHurt(false, 0);
		}
	}, Duration);
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
	if (PXCharacter.IsValid())
	{
		return (PXCharacter->GetActorLocation() - GetActorLocation()).Size2D();
	}
	return 0;
}

float ABaseEnemy::GetVerticalDistanceToPlayer() const
{
	if (PXCharacter.IsValid())
	{
		return PXCharacter->GetActorLocation().Z - GetActorLocation().Z;
	}
	return 0;
}

void ABaseEnemy::LoadData_Implementation(FName Level)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataAsset)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	
	UDataTable* DataTable = DataAsset->EnemyLevelDataTable.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	UDataTableSubsystem* DataTableManager = GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	EnemyData = DataTableManager->FindRow<FEnemyData>(DataTable, EnemyLevel);

	CachedASC->SetAttributeValue(EPXAttribute::BasicMaxHP, EnemyData.BasicMaxHP);
	CachedASC->SetAttributeValue(EPXAttribute::CurMaxHP, EnemyData.BasicMaxHP);
	CachedASC->SetAttributeValue(EPXAttribute::HP, EnemyData.BasicMaxHP);
	
	CachedASC->SetAttributeValue(EPXAttribute::BasicAttackValue, EnemyData.BasicAttackValue);
	CachedASC->SetAttributeValue(EPXAttribute::CurAttackValue, EnemyData.BasicAttackValue);
	
	CachedASC->SetAttributeValue(EPXAttribute::BasicAttackCD, EnemyData.BasicAttackCD);
	CachedASC->SetAttributeValue(EPXAttribute::CurAttackCD, EnemyData.BasicAttackCD);
	
	CachedASC->SetAttributeValue(EPXAttribute::BasicSpeed, EnemyData.BasicSpeed);
	CachedASC->SetAttributeValue(EPXAttribute::CurSpeed, EnemyData.BasicSpeed);
	
	CachedASC->SetAttributeValue(EPXAttribute::BasicAcceleration, EnemyData.BasicAcceleration);
	CachedASC->SetAttributeValue(EPXAttribute::CurAcceleration, EnemyData.BasicAcceleration);
	
	CachedASC->SetAttributeValue(EPXAttribute::BasicAcceleration, EnemyData.BasicAcceleration);
	CachedASC->SetAttributeValue(EPXAttribute::CurAcceleration, EnemyData.BasicAcceleration);

	CachedASC->SetAttributeValue(EPXAttribute::BasicRepelResist, EnemyData.BasicRepelResist);
	CachedASC->SetAttributeValue(EPXAttribute::CurRepelResist, EnemyData.BasicRepelResist);

	CachedASC->SetAttributeValue(EPXAttribute::BasicBodySizeScale, EnemyData.BasicBodySizeScale);
	CachedASC->SetAttributeValue(EPXAttribute::CurBodySizeScale, EnemyData.BasicBodySizeScale);
	
	CurRepelValue = EnemyData.RepelValue;
	
	LostEnemyTime = EnemyData.LostEnemyTime;
	ActionFieldsCanAttack = FGameplayTagContainer::CreateFromArray(DataAsset->ActionFieldsCanAttack);
	EnemyAIComponent->SetActionFieldDistance(EnemyData.ActionFieldDistance);
	WeakPointRefreshPeriod = EnemyData.WeakPointRefreshPeriod;

#pragma region GAS

	InitAbilitiesToGive.Append(DataAsset->InitAbilitiesToGive);
	InitEffects.Append(DataAsset->InitEffects);
	if (CachedASC)
	{
		for (auto Ability : InitAbilitiesToGive)
		{
			CachedASC->K2_GiveAbility(Ability);
		}

		FGameplayEffectContextHandle EffectContext = CachedASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		
		for (auto Effect : InitEffects)
		{
			if (!Effect) continue;
			
			FGameplayEffectSpecHandle SpecHandle = CachedASC->MakeOutgoingSpec(
				Effect, 
				1.0f, 
				EffectContext
			);
        
			if (SpecHandle.IsValid())
			{
				// 应用效果到自身
				CachedASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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
	
	// StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
	FightComponent = CreateDefaultSubobject<UFightComponent>(TEXT("FightComp"));
	EnemyAIComponent = CreateDefaultSubobject<UEnemyAIComponent>(TEXT("EnemyAIComponent"));
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));

	CachedASC = CreateDefaultSubobject<UPXEnemyASComponent>(TEXT("AbilitySystem"));
	CachedASC->SetIsReplicated(true);
	
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
	LoadData(EnemyLevel);
}


void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	StateComponent = NewObject<UStateComponent>(this, TEXT("StateComponent"));
	if (StateComponent)
	{
		StateComponent->RegisterComponent();
	}
	
	if (CachedASC)
	{
		if (const UPXAttributeSet* PXAttributeSet = CachedASC->GetSet<UPXAttributeSet>())
		{
			PXAttributeSet->OnPXAttributeChange.AddDynamic(this, &ThisClass::OnAttributeChanged);
		}
	}
}

void ABaseEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	OnEnemyDie.RemoveAll(this);
}

void ABaseEnemy::SetActionMove(const FVector& MoveVector,  const FName& CurveName, float SustainTime, bool bInterrupt, bool bCabBeInterrupt)
{
	if (!GetCapsuleComponent()) return;
	if (CurveName.IsNone()) return;
	if (SustainTime <= 0.0f) return;
	if (ActionMove.bActionMoving && !ActionMove.bCabBeInterrupt) return;
	if (ActionMove.bActionMoving && !bInterrupt) return;

	UEnemyAISubsystem* EnemyAISubsystem = UEnemyAISubsystem::GetInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(EnemyAISubsystem)

	const FCurveFloatData& ActionMoveCurveData = EnemyAISubsystem->GetActionMoveCurveData(CurveName);
	if (!IsValid(ActionMoveCurveData.Curve)) return;

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

	// 向下检查的悬崖高度
	float Height = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50;
	// 间距单位
	float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector Direction = MoveVector.GetSafeNormal2D();
	FVector TargetLocation = USpaceFuncLib::GetHorizontalFarestPosition(StartLocation, Direction, MoveVector.Size2D(), Height, Radius);

	// 该方向没有可行的点位
	if (StartLocation == TargetLocation) return;
	
	if (EnemyAIComponent)
	{
		EnemyAIComponent->MoveCheckAllies(TargetLocation, TargetLocation);
	}

	ActionMove.bActionMoving = true;
	ActionMove.CurTime = 0;
	ActionMove.SustainTime = SustainTime;
	ActionMove.StartLocation = StartLocation;
	ActionMove.TargetLocation = TargetLocation;
	ActionMove.CurveVector = ActionMoveCurveData.Curve;
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
	if (EnemyData.BasicSpeed <= 0.0f) return;
	
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
	
	UEnemyAISubsystem* EnemyAISubsystem = UEnemyAISubsystem::GetInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(EnemyAISubsystem)

	const FCurveFloatData& ActionMoveCurveData = EnemyAISubsystem->GetActionMoveCurveData(CurveName);
	if (!IsValid(ActionMoveCurveData.Curve)) return;

	float JumpDuration = FMath::FRandRange(0.15f, 0.25f) + FMath::FRandRange(0.4f, 0.45f) * FVector::Dist2D(StartLocation, TargetLocation) / EnemyData.BasicSpeed;
	
	ActionMove.bActionMoving = true;
	ActionMove.CurTime = 0;
	ActionMove.SustainTime = JumpDuration;
	ActionMove.StartLocation = StartLocation;
	ActionMove.TargetLocation = TargetLocation;
	ActionMove.CurveVector = ActionMoveCurveData.Curve;
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

void ABaseEnemy::OnDie_Implementation()
{
	if (AEnemyAIController* EnemyAI = GetController<AEnemyAIController>())
	{
		EnemyAI->StopAI();
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
	
	if (UDropSubsystem* DropSubsystem = UDropSubsystem::GetInstance(this))
	{
		DropSubsystem->SpawnItems(EnemyData.DropID_Rate, GetActorLocation());
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerPawn, ECR_Overlap);
	}
	
	SetInAttackEffect(false);
	SetInAttackState(false);
	
	if (APXCharacterPlayerState* PS = Cast<APXCharacterPlayerState>(UGameplayStatics::GetPlayerState(this, 0)))
	{
		PS->OnEnemyDie(this);
	}
	if (APXGameState* GS = Cast<APXGameState>(UGameplayStatics::GetGameState(this)))
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


void ABaseEnemy::OnHPChanged_Implementation(int32 OldValue, int32 NewValue)
{
	if (bDead) return;
	if (OldValue == NewValue) return;
	
	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(StateComponent)
	
	if (NewValue > 0)
	{
		if (NewValue < OldValue)
		{
			OnHurt(NewValue);
			int32 ChangedValue = OldValue - NewValue;
			if (ChangedValue > StateComponent->GetMaxHP() * 0.1)
			{
				SetHurt(true, StateComponent->CalHurtDuration(ChangedValue));
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

	if (PXCharacter.IsValid())
	{
		return (PXCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	}

	return FVector::ZeroVector;
}

UAbilitySystemComponent* ABaseEnemy::GetAbilitySystemComponent() const
{
	return CachedASC;
}

// void ABaseEnemy::BuffUpdate_Attack_Implementation()
// {
// 	if (BuffComponent)
// 	{
// 		CurAttackDamage = EnemyData.AttackDamage * (1 + BuffComponent->EffectedPercent_Attack) + BuffComponent->EffectedValue_Attack;
// 	}
// }

// void ABaseEnemy::BuffUpdate_Speed_Implementation()
// {
// 	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
// 	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
// 	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MovementComponent);
//
// 	MovementComponent->MaxWalkSpeed = EnemyData.MoveSpeed * (BuffComponent->EffectedPercent_Speed + 1.0f) + BuffComponent->EffectedValue_Speed;
// 	MovementComponent->MaxAcceleration = EnemyData.MoveAcceleration * (BuffComponent->EffectedPercent_Speed + 1.0f);
//
// 	if (MovementComponent->MaxWalkSpeed < EnemyData.MoveSpeed)
// 	{
// 		CustomTimeDilation = MovementComponent->MaxWalkSpeed / EnemyData.MoveSpeed;
// 	}
// }

// void ABaseEnemy::BuffUpdate_Sight_Implementation()
// {
// 	if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Controller) )
// 	{
// 		EnemyAIController->UpdateSightRadius(EnemyData.SightRadius * (BuffComponent->EffectedPercent_Sight + 1.0f) + BuffComponent->EffectedValue_Sight);
// 	}
// }

void ABaseEnemy::OnAttributeChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Movement)
	
	if (Attribute == UPXAttributeSet::GetHPAttribute())
	{
		OnHPChanged(OldValue, NewValue);
	}
	if (Attribute == UPXAttributeSet::GetCurSightAttribute())
	{
		if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Controller))
		{
			EnemyAIController->UpdateSightRadius(NewValue);
		}
	}
	if (Attribute == UPXAttributeSet::GetCurSpeedAttribute())
	{
		Movement->MaxWalkSpeed = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurAccelerationAttribute())
	{
		Movement->MaxAcceleration = NewValue;		
	}
	if (Attribute == UPXAttributeSet::GetCurBodySizeScaleAttribute())
	{
		SetActorScale3D(FVector(NewValue));
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
	return PXCharacter.Get();
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
	FName TimerName = FName(GetName() + "_DelayLosePlayer");
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
	[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		if (WeakThis->InAttackRange())
		{
			WeakThis->DelayLosePlayer();
		}
		else
		{
			WeakThis->GoPatrol();
		}
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
	if (!PXCharacter.IsValid()) return false;
	if (!Execute_IsAlive(PXCharacter.Get())) return false;
	
	return !bDead && !bInAttackState && !bHurt;
}

bool ABaseEnemy::Dash_Implementation()
{
	return false;

}

void ABaseEnemy::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Maker)
	
	OutDamage = InDamage;
	if (Execute_GetOwnCamp(this).HasTag(TAG("Enemy.BOSS")))
	{
		if (UAbilityComponent* AbilityComponent = Maker->GetComponentByClass<UAbilityComponent>())
		{
			float FoundR;
			if (AbilityComponent->FindExtendData(TAG("Ability.GiantSlayer.Set.ToBossDamagePlusPercent"),FoundR))
			{
				OutDamage *= 1 + FoundR;
			}
		}
	}

	// 暂时默认防御减半伤害
	if (bInDefendState)
	{
		OutDamage *= 0.5;
	}

	UTimerManagerFuncLib::SetDelay(this,[WeakThis = TWeakObjectPtr(this), Maker]
	{
		if (WeakThis.IsValid())
		{
			WeakThis->OnSensingPlayer(Maker);
		}
	}, 0.2);
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
		UTimerManagerFuncLib::SetDelay(this, [WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;
			if (WeakThis->GetCharacterMovement())
			{
				WeakThis->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			}
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DestroyComponent();
	}
}

void ABaseEnemy::OnRemoteAttackEffect_Implementation()
{
	IFight_Interface::OnRemoteAttackEffect_Implementation();
}

void ABaseEnemy::OnRemoteAttackEnd_Implementation()
{
	FName TimerName = FName(GetName() + "_InAttackState");
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), TimerName, 
	[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->SetInAttackState(false);
		}, Execute_GetAttackCD(this));
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
	FName TimerName = FName(GetName() + "_InAttackState");
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), TimerName, 
	[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->SetInAttackState(false);
		}, Execute_GetAttackCD(this));
}


APawn* ABaseEnemy::GetPawn_Implementation()
{
	return this;
}

float ABaseEnemy::GetAttackCD_Implementation()
{
	return CachedASC ? CachedASC->GetAttributeValue(EPXAttribute::CurAttackCD) : 1.0f;
}

int ABaseEnemy::GetAttackDamage_Implementation()
{
	return CachedASC ? CachedASC->GetAttributeValue(EPXAttribute::CurAttackValue) : 1.0f;
}

FVector ABaseEnemy::GetAttackRepel_Implementation()
{
	return CurRepelValue;
}

void ABaseEnemy::OnAnimVulnerableBegin_Implementation()
{
	if (StateComponent)
	{
		StateComponent->SetInvulnerable(true);
	}
}

void ABaseEnemy::OnAnimVulnerableEnd_Implementation()
{
	if (StateComponent)
	{
		StateComponent->SetInvulnerable(false);
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
	if (!EnemyAIComponent) return;
	if (!Execute_IsAlive(this)) return;
	if (!PXCharacter.IsValid()) return;
	
	
	bool PlayerAtRight = USpaceFuncLib::ActorAtActorRight(PXCharacter.Get(),this);

	FVector Dir_PlayerRight = PXCharacter->GetRightVectorWithBlendYaw();
	FVector Dir_PlayerToSelf = (PXCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	float DotValue = Dir_PlayerRight.Dot(Dir_PlayerToSelf);
	float Degree = FMath::Acos(DotValue) * 180 / PI;

	float CrossZ = Dir_PlayerRight.Cross(Dir_PlayerToSelf).Z;

	EWorldDirection RelativeDirection_PlayerToSelf = USpaceFuncLib::ActorAtActorWorldDirection(EnemyAIComponent->PXCharacter.Get(), this, PXCharacter->CurBlendYaw);

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
	if (!IsValid(ActionMove.CurveVector)) return;

	ActionMove.CurTime += DeltaSeconds;
	float MovePercent = ActionMove.CurTime / ActionMove.SustainTime;

	if (MovePercent <= 1)
	{

		FVector CurCurveVector = ActionMove.CurveVector->GetVectorValue(MovePercent);
		FVector CurLocation;
		CurLocation.X = FMath::Lerp(ActionMove.StartLocation.X , ActionMove.TargetLocation.X, CurCurveVector.X);
		CurLocation.Y = FMath::Lerp(ActionMove.StartLocation.Y , ActionMove.TargetLocation.Y, CurCurveVector.Y);

		// UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("CurLocation ActionMove.StartLocation.X: %f, ActionMove.TargetLocation.X: %f, CurCurveVector.X: %f"),
		// 	ActionMove.StartLocation.X, ActionMove.TargetLocation.X, CurCurveVector.X));

		
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
		CurLocation + FVector(0, 0, -2 * HalfHeight),
	TraceTypeQuery1, false, {this},
			EDrawDebugTrace::None, OutHit, true,
			FLinearColor::Yellow, FLinearColor::Blue, 1.0f);

	if (bHitFloor)
	{
		AddActorWorldOffset(FVector(0, 0, HalfHeight - OutHit.Distance));
	}
	else
	{
		if (GetCharacterMovement())
		{
			AddActorWorldOffset(FVector(0, 0, 0.2 * GetCharacterMovement()->GetGravityZ() * DeltaSeconds));
		}
	}
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
		Widget->SetVisibility(PXCharacter.IsValid());
	}
	
}






#undef LOCTEXT_NAMESPACE