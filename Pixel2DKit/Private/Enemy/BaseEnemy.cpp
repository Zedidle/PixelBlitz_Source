// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"

#include "NiagaraFunctionLibrary.h"
#include "PaperFlipbookComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "Utilitys/SpaceFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/BasePXAnimInstance.h"
#include "Core/PXGameMode.h"
#include "Core/PXGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyDataAsset.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/PXEnemyAttributeSet.h"
#include "Perception/PawnSensingComponent.h"
#include "Player/PXCharacterPlayerState.h"
#include "Player/PXPlayerState.h"
#include "Sound/SoundCue.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/DropSubsystem.h"
#include "Subsystems/PXAnimSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/SoundFuncLib.h"

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

	UDataTable* DataTable = DataAsset->EnemyLevelDataTable.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	UDataTableSubsystem* DataTableManager = GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	EnemyData = DataTableManager->FindRow<FEnemyData>(DataTable, EnemyLevel);

	HealthComponent->ModifyMaxHP(EnemyData.HP, EStatChange::Reset, true);
	SetActorScale3D(FVector(EnemyData.BodyScale));
	DropData = EnemyData.Drop;
	LoadLookDeterrence(EnemyData.LookDeterrence);

	CurAttackDamage = EnemyData.AttackDamage;
	BasicAttackInterval = EnemyData.AttackInterval;
	
	ActionFieldsCanAttack = FGameplayTagContainer::CreateFromArray(DataAsset->ActionFieldsCanAttack);
	EnemyAIComponent->AttackRange = EnemyData.AttackRange;
	EnemyAIComponent->SetActionFieldDistance(EnemyData.ActionFieldDistance);
	GetCharacterMovement()->MaxAcceleration = EnemyData.MoveAcceleration;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData.MoveSpeed;

	CurAttackRepel = EnemyData.AttackKnockbackForce;

	PawnSensingComponent->SightRadius = EnemyData.SightRadius;
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

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	FightComponent = CreateDefaultSubobject<UFightComponent>(TEXT("FightComp"));
	EnemyAIComponent = CreateDefaultSubobject<UEnemyAIComponent>(TEXT("EnemyAIComponent"));
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));

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

void ABaseEnemy::LoadLookDeterrence_Implementation(int32 Level)
{
}

void ABaseEnemy::OnDie_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	UDropSubsystem* DropSubsystem = GetGameInstance()->GetSubsystem<UDropSubsystem>();
	DropSubsystem->SpawnItems(DropData, GetActorLocation(), 0.2f);

	if (PawnSensingComponent)
	{
		PawnSensingComponent->DestroyComponent();
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
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
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
		if (DataAsset)
		{
			if (USoundCue* DieSound = DataAsset->DieSound.LoadSynchronous())
			{
				USoundFuncLib::PlaySoundAtLocation(DieSound, GetActorLocation(), 1.0f);
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
		if (GetCapsuleComponent())
		{
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerPawn, ECollisionResponse::ECR_Ignore);
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
			USoundFuncLib::PlaySoundAtLocation(HurtSound, GetActorLocation(), 1.0f);
		}

		if (UNiagaraSystem* NS_Hurt = DataAsset->NS_Hurt.LoadSynchronous())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Hurt, GetActorLocation());
		}
	}
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
}

void ABaseEnemy::BuffUpdate_Sight_Implementation()
{
	if (PawnSensingComponent)
	{
		PawnSensingComponent->SightRadius = EnemyData.SightRadius * (BuffComponent->EffectedPercent_Sight + 1.0f) + BuffComponent->EffectedValue_Sight;
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
	return PixelCharacter;
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

void ABaseEnemy::DelayLosePlayer_Implementation()
{
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), FName(GetName() + "_DelayLosePlayer_Implementation"),
		[WeakThis = TWeakObjectPtr<ThisClass>(this)]
		{
			if (!WeakThis.IsValid()) return;
			if (WeakThis->bDead) return;

			WeakThis->SetAttackAnimToggle(false);

			UTimerSubsystemFuncLib::SetDelay(WeakThis->GetWorld(), [WeakThis]
			{
				if (!WeakThis.IsValid()) return;
				WeakThis->SetPixelCharacter(nullptr);
			}, 2);
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
	if (!IsValid(PixelCharacter)) return false;
	if (!Execute_IsAlive(PixelCharacter)) return false;
	
	return !bDead && !bInAttackState;
}

bool ABaseEnemy::Dash_Implementation()
{
	return false;

}

void ABaseEnemy::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage)
{
	OutDamage = InDamage;
	int SurDamage = InDamage;

	if (Execute_GetOwnCamp(this).HasTag(
				FGameplayTag::RequestGameplayTag("Enemy.BOSS")))
	{
		float Result;
		if (Maker->Implements<UFight_Interface>() && Execute_FindEffectGameplayTag(Maker,
			FGameplayTag::RequestGameplayTag("AbilitySet.ToBossDamagePlusPercent"),
			Result))
		{
			SurDamage = SurDamage * (1 + Result);
		}
	}
	
	OutDamage = SurDamage;
}




void ABaseEnemy::OnBeAttacked_Invulnerable_Implementation()
{
	IFight_Interface::OnBeAttacked_Invulnerable_Implementation();
}

int ABaseEnemy::DamagePlus_Implementation(int InDamage, AActor* Receiver)
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

void ABaseEnemy::OnAnimDieEnd_Implementation()
{

}

void ABaseEnemy::OnRemoteAttackEffect_Implementation()
{
	IFight_Interface::OnRemoteAttackEffect_Implementation();
}

void ABaseEnemy::OnRemoteAttackEnd_Implementation()
{
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), FName(GetName() + "InAttackState"), 
	[WeakThis = TWeakObjectPtr<ThisClass>(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->SetInAttackState(false);
		}, BasicAttackInterval);
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
	
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), FName(GetName() + "InAttackState"), 
	[WeakThis = TWeakObjectPtr<ThisClass>(this)]
		{
			if (!WeakThis.IsValid()) return;

			WeakThis->SetInAttackState(false);
		}, BasicAttackInterval);
}

UAbilityComponent* ABaseEnemy::GetAbilityComponent_Implementation()
{
	return AbilityComponent;
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
	if (GetIsAttacking()) return;
	if (!Execute_IsAlive(this)) return;
	if (!IsValid(PixelCharacter)) return;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	bool PlayerAtRight = USpaceFuncLib::ActorAtActorRight(PixelCharacter,this);
	float f =  PlayerAtRight ? 180 : 0;

	FVector L;
	FRotator R; // 凑数，后面修改
	UGameplayStatics::GetPlayerCameraManager(World, 0 )->GetActorEyesViewPoint(L,R);
	R = UKismetMathLibrary::FindLookAtRotation (L, GetActorLocation());

	
	FVector PlayerForward = PixelCharacter->GetRightVectorWithBlendYaw();
	FVector PlayerToSelf = (GetActorLocation() - PixelCharacter->GetActorLocation()).GetSafeNormal2D();
	float c = PlayerForward.Dot(PlayerToSelf);
	float d = FMath::Acos(c) * 180 / PI;

	float z = PlayerForward.Cross(PlayerToSelf).Z;
	
	float yaw = f + R.Yaw - 90;

	// 判断怪物是否在玩家的东侧或西侧
	FGameplayTag ActionField = EnemyAIComponent->GetActionFieldByPlayer();
	if (!ActionField.IsValid()) return;
	
	if (ActionField.MatchesTag(FGameplayTag::RequestGameplayTag("ActionField.East")))
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
	else if (ActionField.MatchesTag(FGameplayTag::RequestGameplayTag("ActionField.West")))
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
	else if (ActionField.MatchesTag(FGameplayTag::RequestGameplayTag("ActionField.North")))
	{
		if (PlayerAtRight)
		{
			SetActorRotation(FRotator(0, PixelCharacter->CurBlendYaw + 45, 0));
		}
		else
		{
			SetActorRotation(FRotator(0, PixelCharacter->CurBlendYaw + 135, 0));
		}
	}
	else if (ActionField.MatchesTag(FGameplayTag::RequestGameplayTag("ActionField.South")))
	{
		if (PlayerAtRight)
		{
			SetActorRotation(FRotator(0,PixelCharacter->CurBlendYaw - 45, 0));
		}
		else
		{
			SetActorRotation(FRotator(0, PixelCharacter->CurBlendYaw - 135, 0));
		}
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
