// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasePXCharacter.h"

#include "AbilitySystemComponent.h"
#include "Animation/BasePixelAnimInstance.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Basic/PXGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "ISourceControlProvider.h"
#include "Character/PXCharacterDataAsset.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "Fight/Components/EnergyComponent.h"
#include "Subsystems/PXAnimSubsystem.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Character/Components/TalentComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "GAS/PXASComponent.h"
#include "PlayerState/PXCharacterPlayerState.h"
#include "SaveGame/PXSettingSaveGame.h"
#include "Sound/SoundCue.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Utilitys/SoundFuncLib.h"


class UDataTableSubsystem;

void ABasePXCharacter::LoadData()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataAsset);

	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);

	const FCharacterAttribute& Attribute = DataAsset->CharacterAttribute;
	const FCharacterAttribute& InheritAttribute = MainSaveGame->CharacterInheritAttribute;
	
	BasicMaxJumpCount = Attribute.BasicMaxJumpCount;
	CurMaxJumpCount = BasicMaxJumpCount;
	BasicJumpMaxHoldTime = DataAsset->CharacterAttribute.BasicJumpMaxHoldTime;
	BasicAttackInterval = DataAsset->CharacterAttribute.BasicAttackInterval;
	
	BasicSpringArmLength = Attribute.SpringArmLengthSight + InheritAttribute.SpringArmLengthSight;
	CurSpringArmLength = BasicSpringArmLength;
	
	BasicAttackValue = Attribute.BasicAttackValue + InheritAttribute.BasicAttackValue;

	if (EnergyComponent)
	{
		EnergyComponent->SetMaxEP(Attribute.MaxEnergy + InheritAttribute.MaxEnergy);
	}

	if (HealthComponent)
	{
		HealthComponent->ModifyMaxHP(Attribute.MaxHealth + InheritAttribute.MaxHealth, EStatChange::Reset, true);
		HealthComponent->RepelResistancePercent = Attribute.RepelResistPercent + InheritAttribute.RepelResistPercent;
	}
	
	BasicMoveSpeed = Attribute.WalkMoveSpeed + InheritAttribute.WalkMoveSpeed;
	BasicMoveAcceleration = Attribute.WalkMoveAcceleration + InheritAttribute.WalkMoveAcceleration;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = BasicMoveSpeed;
		GetCharacterMovement()->MaxAcceleration = BasicMoveAcceleration;
		GetCharacterMovement()->JumpZVelocity = Attribute.JumpHeight + InheritAttribute.JumpHeight;
	}
	
}


void ABasePXCharacter::Tick_SaveFallingStartTime()
{
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
	{
		if (!PreFrameFalling)
		{
			PreFrameFalling = true;
			if (UWorld* World = GetWorld())
			{
				FallingStartTime = World->TimeSeconds;
			}
		}
	}
}

void ABasePXCharacter::Tick_SpriteRotation()
{
	if (!GetCharacterMovement() || GetCharacterMovement()->Velocity.Size2D() < 1.0f) return;

	if (!GetSprite()) return;

	FVector Velocity;
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		Velocity = GetCharacterMovement()->Velocity;
	}
	else
	{
		Velocity = GetCharacterMovement()->GetCurrentAcceleration();
	}

	float d = FVector::DotProduct(Velocity.GetSafeNormal2D(0.1f), GetFaceToCamera());
	float cz = FVector::CrossProduct(Velocity.GetSafeNormal2D(0.1f), GetFaceToCamera()).Z;

	float toLeft = FMath::GetMappedRangeValueClamped(
		FVector2D(-0.5, 0.5),
		FVector2D(45, -45), d) + CurBlendYaw - 180;

	float toRight = FMath::GetMappedRangeValueClamped(
		FVector2D(-0.5, 0.5),
		FVector2D(-45, 45), d) + CurBlendYaw;
	
	float tmpYaw;
	if (DeltaBlendYaw != 0)
	{
		if (DeltaBlendYaw > 0)
		{
			tmpYaw = (d > 0 ? -90 : 0) + toLeft + (d > 0 && cz > 0 ? -60 : 0);
		}
		else
		{
			tmpYaw = (d > 0 ? 90 : 0) + toRight + (d > 0 && cz < 0 ? 60 : 0);
		}
	}
	else
	{
		// VelocityDotMoveForwardVector
		float d2 = FVector::DotProduct(Velocity.GetSafeNormal2D(), GetMoveForwardVector());
		if (d != 0)
		{
			if (d2 < 0)
			{
				if (PreSpriteLeft)
				{
					tmpYaw = toLeft;
				}
				else
				{
					if (d2 < -0.1)
					{
						tmpYaw = toLeft;
						PreSpriteLeft = true;
					}
					else
					{
						tmpYaw = toRight;
					}
				}
			}
			else
			{
				if (PreSpriteLeft)
				{
					if (d2 > 0.1)
					{
						tmpYaw = toRight;
						PreSpriteLeft = false;
					}
					else
					{
						tmpYaw = toLeft;
					}
				}
				else
				{
					tmpYaw = toRight;
				}
			}
		}
		else
		{
			if (d2 == 1)
			{
				tmpYaw = toRight;
				PreSpriteLeft = false;

			}
			else if (d2 == -1)
			{
				tmpYaw = toLeft;
				PreSpriteLeft = true;
			}
			else
			{
				return;
			}
		}
	}

	GetSprite()->SetWorldRotation(FRotator(0, tmpYaw, 0));
	DeltaBlendYaw = 0;
}

void ABasePXCharacter::Tick_SpringArmMotivation()
{
	if (!GetCharacterMovement()) return;
	if (!GetComponentByClass<USpringArmComponent>()) return;

	FVector Velocity = GetCharacterMovement()->Velocity;
	float d = FVector::DotProduct(GetFaceToCamera(), Velocity.GetSafeNormal());
	
	// 镜头偏转
	float pitch = FMath::Clamp(d,-0.3, 0.5) * -15 + CurBlendPitch;
	float yaw = FVector::DotProduct(GetMoveForwardVector(), Velocity.GetSafeNormal()) * 5 + CurBlendYaw - 90;
	USpringArmComponent* SpringArm = GetComponentByClass<USpringArmComponent>();
	SpringArm->SetRelativeRotation(FRotator(pitch, yaw, 0));
	
	if (UPXSaveGameSubsystem* SettingSaveGame = GetGameInstance()->GetSubsystem<UPXSaveGameSubsystem>())
	{
		if (UPXSettingSaveGame* SaveGame = SettingSaveGame->GetSettingData())
		{
			switch (SaveGame->VideoSetting_CameraMode)
			{
			case 0: // 镜头前瞻
				{
					Velocity.X *= 0.35;
					Velocity.Y *= 0.35;
					Velocity.Z *= 0.2;
					FVector Location = GetActorLocation() + Velocity;
					FVector FightOffset = (CurCameraOffset.IsNearlyZero(1) ? FightCenterForCameraOffset : CurCameraOffset) * CurSpringArmLength / 300 *
							(d > 0 ? 1 : FMath::GetMappedRangeValueClamped(FVector2D(-1, 0), FVector2D(0.2, 1), d));
					SpringArm->SetWorldLocation(Location + FightOffset);
					break;
				}
			case 1: // 镜头跟随
				{
					SpringArm->CameraLagSpeed = FMath::Pow(Velocity.Length(), 0.7) / 5.5 + 1;
					FVector FightOffset = (CurCameraOffset.IsNearlyZero(1) ? FightCenterForCameraOffset : CurCameraOffset) * CurSpringArmLength / 280;
					SpringArm->SetWorldLocation(GetActorLocation() + FightOffset);
					break;
				}
			default:
				{
					break;
				}
			}
		}
	}
	
	// 攻击命中导致的视野下降
	if (AttackHitComboNum > 0)
	{
		SpringArm->TargetArmLength = UKismetMathLibrary::Lerp( SpringArm->TargetArmLength,
			CurSpringArmLength * FMath::Max(0.75, 1 - AttackHitComboNum * 0.05),0.08);
	}
	else
	{
		SpringArm->TargetArmLength = UKismetMathLibrary::Lerp(SpringArm->TargetArmLength, CurSpringArmLength, 0.06);
	}
}

void ABasePXCharacter::Tick_CalCameraOffset()
{
	if (SurCameraOffset.Size() < 0.01) return;
	USpringArmComponent* SpringArm = GetComponentByClass<USpringArmComponent>();
	if (!IsValid(SpringArm)) return;
	
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	FVector DeltaLocation = SurCameraOffset * World->GetDeltaSeconds() * 2.5;
	SpringArm->AddWorldOffset(DeltaLocation);
	CurCameraOffset += DeltaLocation;
	SurCameraOffset -= DeltaLocation;
}


void ABasePXCharacter::LoadAbility_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent->AbilityDataTable);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC);
	
	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	
	UDataTableSubsystem* DataTableManager = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	// 暂且什么都不用做

}

ABasePXCharacter::ABasePXCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	EnergyComponent = CreateDefaultSubobject<UEnergyComponent>(TEXT("EnergyComponent"));
	FightComponent = CreateDefaultSubobject<UFightComponent>(TEXT("FightComponent"));
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	TalentComponent = CreateDefaultSubobject<UTalentComponent>(TEXT("TalentComponent"));

	// ASC 初始化
}

bool ABasePXCharacter::HasEffectGameplayTag(const FGameplayTag Tag) const
{
	return EffectGameplayTags.Contains(Tag);
}

float ABasePXCharacter::GetEffectGameplayTag(const FGameplayTag Tag) const
{
	if (EffectGameplayTags.Contains(Tag))
	{
		return EffectGameplayTags[Tag];
	}
	return 0.0f;
}

void ABasePXCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitScale = GetActorScale3D();
	SetFalling(GetCharacterMovement()->IsFalling());

	LoadData();

	
	// AttributeSet 初始化
	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		// const auto EffectContext = AbilitySystem->MakeEffectContext();
		// const auto SpecHandle = AbilitySystem->MakeOutgoingSpec(TestGE->GetClass(), 1, EffectContext);
		// if (SpecHandle.IsValid())
		// {
		// 	AbilitySystem->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystem);
		// }
		
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
}

void ABasePXCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScaleTimerHandle);
	}

	OnPlayerAttackStart.RemoveAll(this);
	OnPlayerDie.RemoveAll(this);
}

void ABasePXCharacter::Falling()
{
	Super::Falling();
	SetFalling(true);
}

int ABasePXCharacter::GetWeaponInitDamage_Implementation()
{
	FGameplayTag AttackDamagePlusTag = FGameplayTag::RequestGameplayTag("AbilitySet.SwordPlay.AttackDamagePlus");
	int AttackValue = BasicAttackValue;
	if (EffectGameplayTags.Contains(AttackDamagePlusTag))
	{
		 AttackValue += EffectGameplayTags[AttackDamagePlusTag];
	}
	
	return AttackValue;
}

UAbilitySystemComponent* ABasePXCharacter::GetAbilitySystemComponent() const
{
	
	if(const APXCharacterPlayerState* CharacterPlayerState = Cast<APXCharacterPlayerState>(GetPlayerState()))
	{
		return CharacterPlayerState->GetAbilitySystemComponent();
	}

	return nullptr;
}


UPXAttributeSet* ABasePXCharacter::GetAttributeSet() const
{
	return AttributeSetBase.Get();
}

FVector ABasePXCharacter::GetMoveForwardVector()
{
	return FRotator(0, CurBlendYaw,0 ).RotateVector(FVector(1,0,0));
}

FVector ABasePXCharacter::GetFaceToCamera()
{
	return FRotator(0, CurBlendYaw + 90,0 ).RotateVector(FVector(1,0,0));
}

void ABasePXCharacter::AddCameraOffset(const FVector& V)
{
	SurCameraOffset += V;
}

bool ABasePXCharacter::GetIsAttacking()
{
	return bInAttackEffect;
}

bool ABasePXCharacter::GetIsDefending()
{
	return false;
}

void ABasePXCharacter::SetDead(bool V)
{
	bDead = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bDead")), V);
}

void ABasePXCharacter::SetHurt(const bool V, const float duration)
{
	bHurt = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bHurt")), V);
	
	if (!bHurt) return;
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
[this]{
			SetHurt(false,0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, duration, false);
}

void ABasePXCharacter::SetJumping(bool V, const float time)
{
	bJumping = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bJumping")), V);
	
	if (!bJumping) return;

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
[this]{
			SetJumping(false,0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, time, false);
}

void ABasePXCharacter::SetFalling(bool V)
{
	bFalling = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bFalling")), V);
}

void ABasePXCharacter::SetLanding(bool V, float time)
{
	bLanding = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bLanding")), V);

	if (!bLanding) return;

	PreFrameFalling = false;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
[this]{
			SetLanding(false,0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, time, false);
}

void ABasePXCharacter::OnDie_Implementation()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->Velocity = FVector(0, 0, 0);
	}
	
	OnPlayerDie.Broadcast();
}


void ABasePXCharacter::SetDashing(bool V)
{
	bDashing = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bDashing")), V);
}

void ABasePXCharacter::SetAttackAnimToggle(bool V)
{
	bAttackAnimToggle = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bAttackAnimToggle")), V);
}

void ABasePXCharacter::SetAttackHolding(bool V)
{
	bAttackHolding = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bAttackHolding")), V);
}

void ABasePXCharacter::SetAttackFire(bool V)
{
	bAttackFire = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bAttackFire")), V);
}

void ABasePXCharacter::SetMoving(bool V)
{
	bMoving = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bMoving")), V);
}

void ABasePXCharacter::EndNormalAttack()
{
	bInAttackStatus = false;
	SetAttackAnimToggle(false);
	SetAttackFire(false);
	SetAttackHolding(false);
}


void ABasePXCharacter::AddViewYaw_Implementation(float V, bool bPlayerControl)
{
}

float ABasePXCharacter::GetFallingTime()
{
	if (UWorld* World = GetWorld())
	{
		return World->TimeSeconds - FallingStartTime;
	}
	return 0;
}



void ABasePXCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Tick_SaveFallingStartTime();
	Tick_SpriteRotation();
	Tick_SpringArmMotivation();
	Tick_CalCameraOffset();
	
	if (IsValid(GetCharacterMovement()))
	{
		SetMoving(GetCharacterMovement()->Velocity.Size() > 1.0f);
	}
}

FVector ABasePXCharacter::GetDashDirection()
{
	FVector Velocity = GetVelocity();
	if (Velocity.IsZero())
	{
		if (USceneComponent* SC = GetSprite())
		{
			return SC->GetForwardVector();
		}
		return GetActorForwardVector();
	}

	FVector	Acceleration = GetCharacterMovement()->GetCurrentAcceleration();
	if (Acceleration.IsZero())
	{
		if (USceneComponent* SC = GetSprite())
		{
			return SC->GetForwardVector();
		}
		return GetActorForwardVector();
	}

	return (Velocity + Acceleration * 2).GetSafeNormal2D();
}

bool ABasePXCharacter::IsAlive_Implementation()
{
	return HealthComponent->GetCurrentHP() > 0;
}

FGameplayTagContainer ABasePXCharacter::GetOwnCamp_Implementation()
{
	return IFight_Interface::GetOwnCamp_Implementation();
}

FGameplayTagContainer ABasePXCharacter::GetEnemyCamp_Implementation()
{
	return IFight_Interface::GetEnemyCamp_Implementation();
}

AActor* ABasePXCharacter::GetTarget_Implementation()
{
	return IFight_Interface::GetTarget_Implementation();
}

void ABasePXCharacter::OnAttackHiting_Implementation()
{
	if (bDashing)
	{
		AttackHitDashing++;
	}
	AttackHitComboNum++;
	if (AttackHitTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(AttackHitTimerHandle);
	}
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this]
	{
		AttackHitComboNum = 0;
	});
	GetWorldTimerManager().SetTimer(AttackHitTimerHandle, TimerDelegate, 2, false);
}

void ABasePXCharacter::PowerRepulsion_Implementation(float Power)
{
	IFight_Interface::PowerRepulsion_Implementation(Power);
}

void ABasePXCharacter::OnBeAttacked_Invulnerable_Implementation()
{
	IFight_Interface::OnBeAttacked_Invulnerable_Implementation();
}

void ABasePXCharacter::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage)
{
	OutDamage = InDamage;
	int SurDamage = InDamage;
	
	if (AbilityComponent)
	{
		AbilityComponent->OnBeAttacked(Maker, InDamage, SurDamage);
	}
	if (SurDamage <= 0)
	{
		OutDamage = 0;
		return;
	}

	InDamage = SurDamage;
	
	if (TalentComponent)
	{
		TalentComponent->OnBeAttacked(Maker, InDamage, SurDamage);
	}
	if (SurDamage <= 0)
	{
		OutDamage = 0;
		return;
	}
	
	return;
}



int ABasePXCharacter::DamagePlus_Implementation(int inValue, AActor* ActorAcceptDamage)
{
	return 0;
}

int ABasePXCharacter::OnDefendingHit_Implementation(int inValue)
{
	return inValue;
}

void ABasePXCharacter::OnAttackHolding_Implementation()
{
	IFight_Interface::OnAttackHolding_Implementation();
}

void ABasePXCharacter::OnDefendingHitEffect_Implementation()
{
	IFight_Interface::OnDefendingHitEffect_Implementation();
}

void ABasePXCharacter::OnAnimVulnerableBegin_Implementation()
{
	IFight_Interface::OnAnimVulnerableBegin_Implementation();
}

void ABasePXCharacter::OnAnimVulnerableEnd_Implementation()
{
	IFight_Interface::OnAnimVulnerableEnd_Implementation();
}

void ABasePXCharacter::OnDashEffectBegin_Implementation()
{
	IFight_Interface::OnDashEffectBegin_Implementation();
}

void ABasePXCharacter::OnDashEffectEnd_Implementation()
{
	IFight_Interface::OnDashEffectEnd_Implementation();
}

UAbilityComponent* ABasePXCharacter::GetAbilityComponent_Implementation()
{
	return AbilityComponent;
}


APawn* ABasePXCharacter::GetPawn_Implementation()
{
	return this;
}


void ABasePXCharacter::BuffEffect_Speed_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime)
{
	IBuff_Interface::BuffEffect_Speed_Implementation(Tag, Percent, Value, SustainTime);
	if (BuffComponent && BuffComponent->Implements<UBuff_Interface>())
	{
		IBuff_Interface::Execute_BuffEffect_Speed(BuffComponent, Tag, Percent, Value, SustainTime);
	}
}

void ABasePXCharacter::BuffUpdate_Speed_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MovementComponent);

	MovementComponent->MaxWalkSpeed = BasicMoveSpeed * (BuffComponent->EffectedPercent_Speed + 1.0f) + BuffComponent->EffectedValue_Speed;
	MovementComponent->MaxAcceleration = BasicMoveAcceleration * (BuffComponent->EffectedPercent_Speed + 1.0f);
}

void ABasePXCharacter::BuffEffect_Attack_Implementation(FGameplayTag Tag, float Percent, int32 Value, float SustainTime)
{
	IBuff_Interface::BuffEffect_Attack_Implementation(Tag, Percent, Value, SustainTime);
	if (BuffComponent && BuffComponent->Implements<UBuff_Interface>())
	{
		IBuff_Interface::Execute_BuffEffect_Attack(BuffComponent, Tag, Percent, Value, SustainTime);
	}
}

void ABasePXCharacter::BuffUpdate_Attack_Implementation()
{
	IBuff_Interface::BuffUpdate_Attack_Implementation();
}

void ABasePXCharacter::BuffEffect_Sight_Implementation(FGameplayTag Tag, float Percent, float Value, float SustainTime)
{
	IBuff_Interface::BuffEffect_Sight_Implementation(Tag, Percent, Value, SustainTime);
	if (BuffComponent && BuffComponent->Implements<UBuff_Interface>())
	{
		IBuff_Interface::Execute_BuffEffect_Sight(BuffComponent, Tag, Percent, Value, SustainTime);
	}
}

void ABasePXCharacter::BuffUpdate_Sight_Implementation()
{
	IBuff_Interface::BuffUpdate_Sight_Implementation();
}

int32 ABasePXCharacter::Buff_CalDamage_Implementation(int32 InDamage)
{
	return IBuff_Interface::Buff_CalDamage_Implementation(InDamage);
}

void ABasePXCharacter::AddBuff_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor,
	bool Permanent)
{
	IBuff_Interface::AddBuff_Implementation(Tag, BuffName, TextColor, Permanent);
	if (BuffComponent && BuffComponent->Implements<UBuff_Interface>())
	{
		IBuff_Interface::Execute_AddBuff(BuffComponent, Tag, BuffName, TextColor, Permanent);
	}
}

void ABasePXCharacter::RemoveBuff_Implementation(FGameplayTag Tag, bool OnlySelf)
{
	IBuff_Interface::RemoveBuff_Implementation(Tag, OnlySelf);
	if (BuffComponent && BuffComponent->Implements<UBuff_Interface>())
	{
		IBuff_Interface::Execute_RemoveBuff(BuffComponent, Tag, OnlySelf);
	}
}

float ABasePXCharacter::GetShortSightResistancePercent_Implementation()
{
	return IBuff_Interface::GetShortSightResistancePercent_Implementation();
}

float ABasePXCharacter::GetSlowDownResistancePercent_Implementation()
{
	return IBuff_Interface::GetSlowDownResistancePercent_Implementation();
}


void ABasePXCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
                                                        const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation,
	                                        TimeDelta);
	SetFalling(true);
	SetMoving(false);
	GetCharacterMovement()->MaxWalkSpeed = BasicMoveSpeed;
}

void ABasePXCharacter::Jump()
{
	Super::Jump();
	SetJumping(true);
	// SetFalling(true); // 取决于落地翻滚时间从起跳还是下落的开始算
}


void ABasePXCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (DataAsset && DataAsset->LandedSound)
	{
		USoundFuncLib::PlaySoundAtLocation(DataAsset->LandedSound.Get(), GetActorLocation(), 1.0f);
	}
	
	SetLanding(true);
	SetJumping(false);
	SetFalling(false);
}

void ABasePXCharacter::SetScale(const float targetValue)
{
	ScaleLerpValue = 0;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
		[this, targetValue]()
		{
			SetActorScale3D(InitScale * FMath::Lerp(ScaleCurValue, targetValue, ScaleLerpValue));
			ScaleLerpValue += 0.01f;
			if (ScaleLerpValue > 1)
			{
				ScaleCurValue = targetValue;
				SetActorScale3D(InitScale * ScaleCurValue);
				GetWorldTimerManager().ClearTimer(ScaleTimerHandle);
			}
		});
	GetWorldTimerManager().SetTimer(ScaleTimerHandle, TimerDel, 0.01f, true);
}

void ABasePXCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput) return;
	if (Action_MoveBack)
	{
		EnhancedInput->BindAction(
			Action_MoveBack,
			ETriggerEvent::Triggered,
			this,
			&ABasePXCharacter::MoveY
		);
	}
	
	if (Action_MoveFront)
	{
		EnhancedInput->BindAction(
			Action_MoveFront,
			ETriggerEvent::Triggered,
			this,
			&ABasePXCharacter::MoveY
		);
	}

	if (Action_MoveLeft)
	{
		EnhancedInput->BindAction(
			Action_MoveLeft,
			ETriggerEvent::Triggered,
			this,
			&ABasePXCharacter::MoveX
		);
	}

	if (Action_MoveRight)
	{
		EnhancedInput->BindAction(
			Action_MoveRight,
			ETriggerEvent::Triggered,
			this,
			&ABasePXCharacter::MoveX
		);
	}

	if (Action_MoveGP)
	{
		EnhancedInput->BindAction(
			Action_MoveGP,
			ETriggerEvent::Triggered,
			this,
			&ABasePXCharacter::Move2D
		);
	}
}

void ABasePXCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
	FVector velocity = GetCharacterMovement()->Velocity.GetSafeNormal();
	FVector dir = WorldDirection.GetSafeNormal2D() * ScaleValue;

	// 还需通过 **AbilityComponent** 补充技能【急转向】逻辑
	if (EffectGameplayTags.Contains(FGameplayTag::RequestGameplayTag("AbilitySet.Brake")))
	{
		if (dir.Dot(velocity) < -0.7 && GetCharacterMovement()->IsMovingOnGround()) // 接近反方向
		{
			float speed = GetCharacterMovement()->Velocity.Length();
			GetCharacterMovement()->Velocity = 0.2 * speed * dir;
		}
	}

}

void ABasePXCharacter::MoveX(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue == 0) return;
	AddMovementInput(GetMoveForwardVector(), AxisValue, false);
	
}

void ABasePXCharacter::MoveY(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue == 0) return;
	AddMovementInput(GetFaceToCamera(), AxisValue, false);
	
}

void ABasePXCharacter::Move2D(const FInputActionValue& Value)
{
	FVector2D AxisValue = Value.Get<FVector2D>();
	if (AxisValue.X == 0 && AxisValue.Y == 0) return;
	MoveX(AxisValue.X);
	MoveY(AxisValue.Y);
}

