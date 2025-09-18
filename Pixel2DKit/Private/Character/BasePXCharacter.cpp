// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasePXCharacter.h"

#include "AbilitySystemComponent.h"
#include "Animation/BasePXAnimInstance.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Basic/PXGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Basic/PXGameMode.h"
#include "Basic/PXGameState.h"
#include "Basic/PXPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Character/PXCharacterDataAsset.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "Fight/Components/EnergyComponent.h"
#include "Subsystems/PXAnimSubsystem.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Character/Components/TalentComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "GameFramework/PlayerStart.h"
#include "GAS/PXASComponent.h"
#include "Input/PXInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/PXCharacterPlayerState.h"
#include "SaveGame/PXSettingSaveGame.h"
#include "Settings/CameraShakeSettings.h"
#include "Settings/CustomResourceSettings.h"
#include "Settings/UserWidgetSettings.h"
#include "Sound/SoundCue.h"
#include "Subsystems/AchievementSubsystem.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/PXGameplayStatics.h"
#include "Utilitys/SoundFuncLib.h"
#include "Utilitys/UserWidgetFuncLib.h"


class UPXInputComponent;
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
	
	BasicMaxJumpCount = Attribute.BasicMaxJumpCount + InheritAttribute.BasicMaxJumpCount;
	CurMaxJumpCount = BasicMaxJumpCount;
	
	BasicSpringArmLength = Attribute.SpringArmLengthSight + InheritAttribute.SpringArmLengthSight;
	CurSpringArmLength = BasicSpringArmLength;

	BasicJumpMaxHoldTime = Attribute.BasicJumpMaxHoldTime + InheritAttribute.BasicJumpMaxHoldTime;
	JumpMaxHoldTime = BasicJumpMaxHoldTime;

	BasicAirControl = Attribute.BasicAirControl + InheritAttribute.BasicAirControl;
	
	BasicAttackValue = Attribute.BasicAttackValue + InheritAttribute.BasicAttackValue;
	BasicAttackInterval = Attribute.BasicAttackInterval + InheritAttribute.BasicAttackInterval;

	if (EnergyComponent)
	{
		EnergyComponent->SetMaxEP(Attribute.MaxEP + InheritAttribute.MaxEP);
	}

	if (HealthComponent)
	{
		HealthComponent->ModifyMaxHP(Attribute.MaxHP + InheritAttribute.MaxHP, EStatChange::Reset, true);
		HealthComponent->RepelResistancePercent = Attribute.RepelResistPercent + InheritAttribute.RepelResistPercent;
	}

	
	MaxWalkSpeed = Attribute.MaxWalkSpeed + InheritAttribute.MaxWalkSpeed;
	MaxAcceleration = Attribute.MaxAcceleration + InheritAttribute.MaxAcceleration;
	JumpZVelocity = Attribute.JumpZVelocity + InheritAttribute.JumpZVelocity;
	GravityScale = Attribute.GravityScale + InheritAttribute.GravityScale;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
		GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
		GetCharacterMovement()->GravityScale = Attribute.GravityScale + InheritAttribute.GravityScale;
		GetCharacterMovement()->AirControl = BasicAirControl;
	}

#pragma region GAS
	InitAbilitiesToGive.Append(DataAsset->InitAbilitiesToGive);
	InitEffects.Append(DataAsset->InitEffects);
	// AttributeSet 初始化
	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
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

	float d = FVector::DotProduct(Velocity.GetSafeNormal2D(0.1f), GetVectorFaceToCamera());
	float cz = FVector::CrossProduct(Velocity.GetSafeNormal2D(0.1f), GetVectorFaceToCamera()).Z;

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
		float d2 = FVector::DotProduct(Velocity.GetSafeNormal2D(), GetRightVectorWithBlendYaw());
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
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SpringArm)
	FVector Velocity = GetCharacterMovement()->Velocity;
	
	float d = FVector::DotProduct(GetVectorFaceToCamera(), Velocity.GetSafeNormal());
	bool CharacterMoveToCamera = d > 0;
	
	// 镜头偏转
	float pitch = FMath::Clamp(d,-0.3, 0.5) * -15 + CurBlendPitch;
	float yaw = FVector::DotProduct(GetRightVectorWithBlendYaw(), Velocity.GetSafeNormal()) * 5 + CurBlendYaw - 90;
	SpringArm->SetRelativeRotation(FRotator(pitch, yaw, 0));
	
	if (UPXSaveGameSubsystem* SettingSaveGame = GetGameInstance()->GetSubsystem<UPXSaveGameSubsystem>())
	{
		if (UPXSettingSaveGame* SaveGame = SettingSaveGame->GetSettingData())
		{
			switch (SaveGame->VideoSetting_CameraMode)
			{
			case 0: // 镜头前瞻
				{
					Velocity *= FrontViewFactor;
					FVector Location = GetActorLocation() + Velocity;
					// 偏移因子，如果玩家朝向镜头移动将会偏移得更多
					float OffsetFactor = CharacterMoveToCamera ? 1 : 0.5;

					FVector BlendOffset = CurMoveCameraOffset * CurSpringArmLength / 300 * OffsetFactor;
					BlendOffset = FMath::Lerp(BlendOffset, CurFightCameraOffset, 0.5);
					
					SpringArm->SetWorldLocation(Location + BlendOffset);
					break;
				}
			case 1: // 镜头跟随
				{
					SpringArm->CameraLagSpeed = FMath::Pow(Velocity.Length(), 0.7) / 5.5 + 1;
					FVector FightOffset = (CurMoveCameraOffset.IsNearlyZero(1) ? CurFightCameraOffset : CurMoveCameraOffset) * CurSpringArmLength / 280;
					
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
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SpringArm)
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	if (RemMoveCameraOffset.Size() > 0.1)
	{
		FVector DeltaMoveCameraOffset = RemMoveCameraOffset * World->GetDeltaSeconds() * 2.5;
		SpringArm->AddWorldOffset(DeltaMoveCameraOffset);
		CurMoveCameraOffset += DeltaMoveCameraOffset;
		RemMoveCameraOffset -= DeltaMoveCameraOffset;
	}

	CurFightCameraOffset = FMath::Lerp(CurFightCameraOffset, TargetFightCameraOffset, FightCameraOffsetSpeedFactor);
}

void ABasePXCharacter::SetFightCameraOffset(FVector Offset)
{
	
}


void ABasePXCharacter::LoadAbility_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent)
	if (AbilityComponent->AbilityDataTables.IsEmpty()) return;

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
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 10.f;
	SpringArm->CameraRotationLagSpeed = 10.f;
	SpringArm->CameraLagMaxDistance = 5.f;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	JumpMaxCount = 9999;
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
	if (GetCharacterMovement())
	{
		SetFalling(GetCharacterMovement()->IsFalling());
		GetCharacterMovement()->AirControlBoostVelocityThreshold = 600;
		GetCharacterMovement()->AirControlBoostMultiplier = 10.0f;
	}

	LoadData();
	LoadWeapon();



	if (HealthComponent)
	{
		HealthComponent->OnHPChanged.AddDynamic(this, &ThisClass::OnHPChanged);
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
	if (HealthComponent)
	{
		HealthComponent->OnHPChanged.RemoveAll(this);
	}
}

bool ABasePXCharacter::SelfCanJump_Implementation()
{
	if (bDashing)
	{
		return false;
	}
	if (bJumping)
	{
		return false;
	}
	if (CurJumpCount >= CurMaxJumpCount)
	{
		return false;
	}
	return true;
}

void ABasePXCharacter::JumpStart_Implementation()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.Jump")));
	}
}

void ABasePXCharacter::Jump()
{
	Super::Jump();
	SetJumping(true);
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.StepOnHead")));
	}
}

void ABasePXCharacter::OnJumped_Implementation()
{
	CurJumpCount++;
	if (CurJumpCount == 1)
	{
		JumpStartTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	}
	if (GetCharacterMovement())
	{
		
		if (GetCharacterMovement()->IsWalking())
		{
			if (USoundCue* SC = DataAsset->JumpSoundOnGround.LoadSynchronous())
			{
				USoundFuncLib::PlaySoundAtLocation(SC, GetActorLocation());
			}
		}
		else
		{
			if (USoundCue* SC = DataAsset->JumpSoundInAir.LoadSynchronous())
			{
				USoundFuncLib::PlaySoundAtLocation(SC, GetActorLocation());
			}
		}
	}
}

void ABasePXCharacter::Falling()
{
	Super::Falling();
	SetFalling(true);
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
	CurJumpCount = 0;
	if (const UCameraShakeSettings* CameraShakeSettings = GetDefault<UCameraShakeSettings>())
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShakeSettings->PlayerLandedShake, GetActorLocation(), 0.0f, 500.0f, 0.0f, true);
	}
	if (bDead)
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->DisableMovement();
		}
	}

	// 舞空术成就
	if (UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - JumpStartTime > 10.0f)
	{
		if (UAchievementSubsystem* AchievementSubsystem = GetGameInstance()->GetSubsystem<UAchievementSubsystem>())
		{
			AchievementSubsystem->CompleteAchievement("10");
		}
	}
}

void ABasePXCharacter::LoadWeapon(TSubclassOf<ABaseWeapon> WeaponClass)
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	if (WeaponClass)
	{
		Weapon = World->SpawnActor<ABaseWeapon>(WeaponClass);
	}
	else
	{
		if (DataAsset->WeaponClass)
		{
			Weapon = World->SpawnActor<ABaseWeapon>(DataAsset->WeaponClass);
		}
	}
	if (Weapon)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		Weapon->AttachToActor(this, Rules);
		Weapon->SetCharacterOwner(this);
	}
}


void ABasePXCharacter::ReadyToStart_Implementation()
{
	if (TalentComponent)
	{
		TalentComponent->LoadTalents();
	}
	if (AbilityComponent)
	{
		AbilityComponent->LoadAbility();
	}
	if (BuffComponent)
	{
		BuffComponent->SetBuffStateWidgetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

int ABasePXCharacter::GetAttackDamage_Implementation()
{
	FGameplayTag AttackDamagePlusTag = FGameplayTag::RequestGameplayTag("AbilitySet.SwordPlay.AttackDamagePlus");
	int AttackValue = BasicAttackValue;
	if (Weapon)
	{
		AttackValue += Weapon->GetWeaponDamage();
	}
	if (EffectGameplayTags.Contains(AttackDamagePlusTag))
	{
		 AttackValue += EffectGameplayTags[AttackDamagePlusTag];
	}

	AttackValue = Execute_Buff_CalInitDamage(this, AttackValue);
	
	return AttackValue;
}

void ABasePXCharacter::Revive_Implementation()
{
	if (HealthComponent)
	{
		HealthComponent->SetHP(HealthComponent->GetMaxHP());
	}
	if (EnergyComponent)
	{
		EnergyComponent->SetEP(EnergyComponent->GetMaxEP() * 0.5);
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	SetDead(false);
	SetDashing(false);
	SetFalling(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	if (APXPlayerController* PC = GetController<APXPlayerController>())
	{
		PC->OnCharacterControl(true);
	}
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

FVector ABasePXCharacter::GetRightVectorWithBlendYaw()
{
	return FRotator(0, CurBlendYaw,0 ).RotateVector(FVector(1,0,0));
}

FVector ABasePXCharacter::GetVectorFaceToCamera()
{
	return FRotator(0, CurBlendYaw + 90,0 ).RotateVector(FVector(1,0,0));
}

void ABasePXCharacter::AddCameraOffset(const FVector& V)
{
	RemMoveCameraOffset += V;
}

bool ABasePXCharacter::GetIsAttacking()
{
	return bInAttackEffect || bAttackHolding;
}

bool ABasePXCharacter::GetIsDefending()
{
	return false;
}


void ABasePXCharacter::CameraOffset_BulletTime(float SustainTime, FVector CameraOffset, float GlobalTimeRate)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Camera);
	FVector TempOffset;
	if (CameraOffset.IsZero())
	{
		TempOffset = 0.2 * (GetActorLocation() - Camera->GetComponentLocation());
	}
	else
	{
		TempOffset = CameraOffset;
	}
	AddCameraOffset(TempOffset);
	CameraOffsetForBulletTime += TempOffset;
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalTimeRate);
	UTimerSubsystemFuncLib::SetDelay(GetWorld(), 
	[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this), TempOffset]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->AddCameraOffset(-TempOffset);
				WeakThis->CameraOffsetForBulletTime -= TempOffset;
				UGameplayStatics::SetGlobalTimeDilation(WeakThis->GetWorld(), 1.0f);
			}
		}, SustainTime
	);
}

void ABasePXCharacter::OutOfControl(float SustainTime)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 0;
	}

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), "OutOfControl",
		[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this)]{
			if (WeakThis.IsValid())
			{
				IBuff_Interface::Execute_BuffUpdate_Speed(WeakThis.Get());
			}
		}, SustainTime
	);
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
	if (duration > 0)
	{
		UTimerSubsystemFuncLib::SetDelay(GetWorld(),
	[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this)]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->SetHurt(false, 0);
			}
		}, duration);
	}
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

void ABasePXCharacter::CheckNearDeath_Implementation()
{
	if (HealthComponent)
	{
		const UUserWidgetSettings* Settings = GetDefault<UUserWidgetSettings>();
		if (Settings && Settings->NearDeathWidgetClass)
		{
			if (HealthComponent->GetHPPercent() < 0.3)
			{
				UUserWidgetFuncLib::AddWidget(Settings->NearDeathWidgetClass, ESlateVisibility::HitTestInvisible, false);
			}
			else
			{
				UUserWidgetFuncLib::HideWidget(Settings->NearDeathWidgetClass);
			}
		}
	}
}

void ABasePXCharacter::OnHPChanged_Implementation(int32 NewValue, int32 ChangedValue, EStatChange ChangeStat,
                                                  AActor* Maker, bool bInner)
{
	if (bDead) return;

	if(NewValue <= 0)
	{
		SetDead(true);

		bool MakerImplementsFight = Maker->Implements<UFight_Interface>(); 
		if (!MakerImplementsFight || ReviveTimes <= 0)
		{
			if (UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>())
			{
				bool GameEnd = false;
				GameInstance->OnPlayerDead(GameEnd);
				if (GameEnd)
				{
					UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
					const UUserWidgetSettings* Settings = GetDefault<UUserWidgetSettings>();
					if (Settings && Settings->NearDeathWidgetClass)
					{
						UUserWidgetFuncLib::AddWidget(Settings->NearDeathWidgetClass, ESlateVisibility::Visible, false);
					}
					if (APXPlayerController* PC = GetController<APXPlayerController>())
					{
						PC->OnCharacterControl(false);
					}
					if (HealthComponent)
					{
						HealthComponent->DestroyComponent();
					}
					UTimerSubsystemFuncLib::SetDelay(GetWorld(),[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this)]
					{
						if (WeakThis.IsValid())
						{
							if (WeakThis->GetCharacterMovement() && !WeakThis->GetCharacterMovement()->IsFalling())
							{
								WeakThis->GetCharacterMovement()->DisableMovement();
							}
						}
					},1);
				}
				else
				{
					OnDie();
				}
			}
		}
		
		if (MakerImplementsFight)
		{
			if (ReviveTimes > 0)
			{
				ReviveTimes --;
				if (ReviveTimes == 0)
				{
					Execute_RemoveBuff(this, FGameplayTag::RequestGameplayTag("Buff.Talent.Revive"), true);
				}
				if (const UCustomResourceSettings* ResourceSettings = GetDefault<UCustomResourceSettings>())
				{
					if (UNiagaraSystem* NS_Revive = ResourceSettings->NS_Revive.LoadSynchronous())
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Revive, GetActorLocation());
						
						// UNiagaraFunctionLibrary::SpawnSystemAttached(NS_Revive, GetRootComponent(), FName(),
						// 	FVector(0), FRotator(0), EAttachLocation::Type::KeepWorldPosition, true);
					}
					if (USoundCue* SC_Revive = ResourceSettings->SC_Revive.LoadSynchronous())
					{
						USoundFuncLib::PlaySound2D(SC_Revive, 1.f);
					}
				}
				if (APXPlayerController* PC = GetController<APXPlayerController>())
				{
					PC->OnCharacterControl(false);
				}
				if (HealthComponent)
				{
					HealthComponent->InvulnerableForDuration(2.5);
				}
				UTimerSubsystemFuncLib::SetDelay(GetWorld(),
					[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this)]
				{
					if (WeakThis.IsValid())
					{
						WeakThis->Revive();
					}
				}, 2);
			}
		}
	}
	else
	{
		CheckNearDeath();
		if (NewValue == 1)
		{
			HasOneHealthPoint = true;
		}
		if (ChangeStat == EStatChange::Decrease)
		{
			USoundFuncLib::PlaySoundAtLocation(DataAsset->HurtSound.LoadSynchronous(), GetActorLocation(), 3.f);
			if (!bInner)
			{
				if (UPXMainSaveGame* MainSaveGame = UPXSaveGameSubSystemFuncLib::GetMainData(GetWorld()))
				{
					MainSaveGame->HurtTimes ++;
				}
				CameraOffset_BulletTime(0.03, FVector(0), 0.1f);
				OutOfControl(UCommonFuncLib::DealDeltaTime(0.2));
				SetHurt(true);
			}
		}
	}
}

void ABasePXCharacter::OnDie_Implementation()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->Velocity = FVector(0, 0, 0);
	}

	if (PlayerStatusWidget)
	{
		PlayerStatusWidget->RefreshLife();
	}

	if (APXPlayerController* PC = GetController<APXPlayerController>())
	{
		PC->OnCharacterControl(false);
	}

	if (AActor* RespawnPoint = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerRespawnPoint::StaticClass()))
	{
		if (APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld()))
		{
			GS->PassDayTime(DataAsset->RevivePassDayTime, false, DataAsset->ReviveDelayTime);
		}
		
		UTimerSubsystemFuncLib::SetDelay(GetWorld(),
			[WeakThis = TWeakObjectPtr<ThisClass>(this), RespawnPoint]
			{
				if (WeakThis.IsValid() && IsValid(RespawnPoint))
				{
					WeakThis->SetActorLocation(RespawnPoint->GetActorLocation());
					WeakThis->Revive();
				}
			}, DataAsset->ReviveDelayTime);
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
	SetAttackAnimToggle(false);
}


void ABasePXCharacter::AddViewYaw(float V, bool bPlayerControl)
{
	if (bPlayerControl)
	{
		bViewYawChangingByPlayerControl = true;
		AddBlendYaw(UCommonFuncLib::DealDeltaTime(V) * 1.6);	
		UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), "AddViewYaw",
			[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this)]
			{
				if (WeakThis.IsValid())
				{
					WeakThis->bViewYawChangingByPlayerControl = false;
				}
			}, 0.5);
	}
	else
	{
		if (!bViewYawChangingByPlayerControl)
		{
			AddBlendYaw(UCommonFuncLib::DealDeltaTime(V) * 1.6);	
		}
	}
}

void ABasePXCharacter::SetViewPitch(float V)
{
	float Pitch = FMath::Clamp(UCommonFuncLib::DealDeltaTime(V) * 1.6 + CurBlendPitch, -50, 20);
	SetBlendPitch(Pitch);
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), "SetViewPitch",
	[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this), Pitch]
	{
		if (WeakThis.IsValid())
		{
			UPXSaveGameSubsystem* SaveGameSubsystem = WeakThis->GetGameInstance()->GetSubsystem<UPXSaveGameSubsystem>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);

			UPXSettingSaveGame* SettingSaveGame = SaveGameSubsystem->GetSettingData();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SettingSaveGame);

			SettingSaveGame->GameSetting_ViewPitch = Pitch;
			SaveGameSubsystem->SaveSettingData();
		}
	}, 0.5);
}

void ABasePXCharacter::PreReadyToStart_Implementation()
{
	if (PlayerStatusWidget)
	{
		PlayerStatusWidget->RefreshLife();
	}
	else
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC);
		
		const UPXAttributeSet* PXAttributeSet = ASC->GetSet<UPXAttributeSet>();
		PlayerStatusWidget = CreateWidget<UBasePlayerStatusWidget>(GetWorld(), DataAsset->PlayerStatusWidgetClass);
		if (PlayerStatusWidget)
		{
			PlayerStatusWidget->Init(PXAttributeSet);
			PlayerStatusWidget->AddToViewport();
		}
	}
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

void ABasePXCharacter::ToStartPoint_Implementation()
{
	CustomTimeDilation = 0.5;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->SetMovementMode(MOVE_Falling, 0);

		UTimerSubsystemFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr<ABasePXCharacter>(this)]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->CustomTimeDilation = 1.0f;
			}
		}, 1.0f);
	}

	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		if (APXGameMode* GameMode = UPXGameplayStatics::GetGameMode(GetWorld()))
		{
			if (GameMode->PlayerRespawnPoint)
			{
				GameMode->PlayerRespawnPoint->SetActorLocation(PlayerStart->GetActorLocation());
				SetActorLocation(PlayerStart->GetActorLocation());
			}
		}
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
	const UCustomResourceSettings* ResourceSettings = GetDefault<UCustomResourceSettings>();
	if (ResourceSettings && ResourceSettings->NS_HitSmoke)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ResourceSettings->NS_SpeedUP.LoadSynchronous(),
			GetRootComponent(),
			FName(""),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::KeepRelativeOffset,
			true,
			true,
			ENCPoolMethod::None,
			true
		);
		NiagaraComponent->SetVariableFloat(FName("Power"), Power);
		bRepulsion = true;
		OutOfControl(Power/1000);
		
		if (GetWorld())
		{
			TWeakObjectPtr<ABasePXCharacter> WeakThis = this;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [WeakThis]
			{
				if (WeakThis.IsValid())
				{
					ABasePXCharacter* PxCharacter = WeakThis.Get();
					PxCharacter->bRepulsion = false;
					UCommonFuncLib::SpawnFloatingTextDefault("Buff/BuffEffectText", "Buff_DeStun",
					PxCharacter->GetActorLocation(),
					FLinearColor(0.68, 0.35, 1.0f, 1.0f),
						FVector2D(0.8,0.8)
					);
				}
				
			}, Power/1000, false);
		}
	}
}

void ABasePXCharacter::OnBeAttacked_Invulnerable_Implementation()
{
	if (bDashing)
	{
		PerfectDodgeTimes++;
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			ASC->TryActivateAbilitiesByTag(
				FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.PerfectDodge"))
			);			
		}
		CameraOffset_BulletTime(0.03f);
	}
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
	}
}



int ABasePXCharacter::DamagePlus_Implementation(int InDamage, AActor* Receiver)
{
	int Result = 0;
	
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		float AirFightPlus = 0;
		if (Execute_FindEffectGameplayTag(this, FGameplayTag::RequestGameplayTag("AbilitySet.InAir.DamagePlus"), AirFightPlus))
		{
			Result += AirFightPlus;
		}
	}

	return Result;
}

int ABasePXCharacter::OnDefendingHit_Implementation(int inValue)
{
	return inValue;
}

void ABasePXCharacter::OnAttackHolding_Implementation()
{
	IFight_Interface::OnAttackHolding_Implementation();
	SetAttackHolding(true);
}

void ABasePXCharacter::OnDefendingHitEffect_Implementation()
{
	IFight_Interface::OnDefendingHitEffect_Implementation();
}

void ABasePXCharacter::OnAnimVulnerableBegin_Implementation()
{
	if (HealthComponent)
	{
		HealthComponent->SetInvulnerable(true);
	}
}

void ABasePXCharacter::OnAnimVulnerableEnd_Implementation()
{
	if (HealthComponent)
	{
		HealthComponent->SetInvulnerable(false);
	}
}

void ABasePXCharacter::OnAttackEffect_Implementation()
{
	IFight_Interface::OnAttackEffect_Implementation();
	if (Weapon)
	{
		Weapon->Use();
	}
}

void ABasePXCharacter::OnAttackEffectBegin_Implementation()
{
	bInAttackEffect = true;
	bAttackStartup = false;
	SetAttackFire(false);
	Execute_OnAttackEffect(this);
}

void ABasePXCharacter::OnAttackEffectEnd_Implementation()
{
	bInAttackEffect = false;
}

void ABasePXCharacter::OnDashEffectBegin_Implementation()
{
	InDashEffect = true;
}

void ABasePXCharacter::OnDashEffectEnd_Implementation()
{
	InDashEffect = false;

	if (TalentComponent)
	{
		TalentComponent->OnDashEnd();
	}
}

UAbilityComponent* ABasePXCharacter::GetAbilityComponent_Implementation()
{
	return AbilityComponent;
}

bool ABasePXCharacter::FindEffectGameplayTag_Implementation(const FGameplayTag Tag, float& Result)
{
	if (EffectGameplayTags.Contains(Tag))
	{
		Result = EffectGameplayTags[Tag];
		return true;
	}
	Result = 0;
	return false;
}


APawn* ABasePXCharacter::GetPawn_Implementation()
{
	return this;
}

float ABasePXCharacter::GetAttackInterval_Implementation()
{
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag("AbilitySet.AttackAccPercent");
	if (EffectGameplayTags.Contains(Tag))
	{
		return BasicAttackInterval / (1 + EffectGameplayTags[Tag]);
	}

	return BasicAttackInterval;
}

void ABasePXCharacter::OnAttackRelease_Implementation()
{
	SetAttackFire(true);
}


void ABasePXCharacter::OnAttackHoldingRelease_Implementation()
{
	SetAttackFire(true);
	SetAttackHolding(false);
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

	MovementComponent->MaxWalkSpeed = MaxWalkSpeed * (BuffComponent->EffectedPercent_Speed + 1.0f) + BuffComponent->EffectedValue_Speed;
	MovementComponent->MaxAcceleration = MaxAcceleration * (BuffComponent->EffectedPercent_Speed + 1.0f);
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
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent)
	CurSpringArmLength = BasicSpringArmLength * (BuffComponent->EffectedPercent_Sight + 1.0f) + BuffComponent->EffectedValue_Sight;

	if (PlayerStatusWidget)
	{
		PlayerStatusWidget->UpdateDark(CurSpringArmLength / BasicSpringArmLength);
	}

}

int32 ABasePXCharacter::Buff_CalInitDamage_Implementation(int32 InDamage)
{
	int LocalDamage = InDamage;
	if (BuffComponent)
	{
		LocalDamage += InDamage * (BuffComponent->EffectedPercent_Attack);
		LocalDamage += BuffComponent->EffectedValue_Attack;
	}
	
	if (TalentComponent)
	{
		TalentComponent->OnBuffCalDamage();
		LocalDamage += TalentComponent->GetAttackDamagePlus();
	}

	return LocalDamage;
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
	float Result = 0.0f;

	// 近视抵抗天赋
	
	return Result;
}

float ABasePXCharacter::GetSlowDownResistancePercent_Implementation()
{
	float Result = 0.0f;
	
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag("TalentSet.SpeedDownResistance");
	if (EffectGameplayTags.Contains(Tag))
	{
		Result += EffectGameplayTags[Tag];
	}
	
	return Result;
}


void ABasePXCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
                                                        const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation,
	                                        TimeDelta);
	SetFalling(true);
	SetMoving(false);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
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
	UPXInputComponent* EnhancedInput = Cast<UPXInputComponent>(PlayerInputComponent);
	if (!EnhancedInput) return;
	if (DataAsset)
	{
		EnhancedInput->SetInputConfig(DataAsset->InputConfig.LoadSynchronous());
		EnhancedInput->BindActionByTagName("InputAction.MoveBack", ETriggerEvent::Triggered, this, &ABasePXCharacter::MoveY);
		EnhancedInput->BindActionByTagName("InputAction.MoveFront", ETriggerEvent::Triggered, this, &ABasePXCharacter::MoveY);
		EnhancedInput->BindActionByTagName("InputAction.MoveLeft", ETriggerEvent::Triggered,this, &ABasePXCharacter::MoveX);
		EnhancedInput->BindActionByTagName("InputAction.MoveRight", ETriggerEvent::Triggered,this, &ABasePXCharacter::MoveX);
		EnhancedInput->BindActionByTagName("InputAction.MoveGP", ETriggerEvent::Triggered,this, &ABasePXCharacter::Move2D);
		EnhancedInput->BindActionByTagName("InputAction.NormalAttack", ETriggerEvent::Triggered,this, &ABasePXCharacter::TryToAttack);
		EnhancedInput->BindActionByTagName("InputAction.NormalAttack", ETriggerEvent::Completed,this, &ABasePXCharacter::AttackRelease);
		EnhancedInput->BindActionByTagName("InputAction.Jump", ETriggerEvent::Started,this, &ABasePXCharacter::TryToJump);
		EnhancedInput->BindActionByTagName("InputAction.Jump", ETriggerEvent::Completed,this, &ABasePXCharacter::JumpRelease);
	}
}

void ABasePXCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
	FVector velocity = GetCharacterMovement()->Velocity.GetSafeNormal();
	FVector dir = WorldDirection.GetSafeNormal2D() * ScaleValue;
	
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
	AddMovementInput(GetRightVectorWithBlendYaw(), AxisValue, false);
	
}

void ABasePXCharacter::MoveY(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue == 0) return;
	AddMovementInput(GetVectorFaceToCamera(), AxisValue, false);
	
}

void ABasePXCharacter::Move2D(const FInputActionValue& Value)
{
	FVector2D AxisValue = Value.Get<FVector2D>();
	if (AxisValue.X == 0 && AxisValue.Y == 0) return;
	MoveX(AxisValue.X);
	MoveY(AxisValue.Y);
}

void ABasePXCharacter::TryToAttack()
{
	if (!IFight_Interface::Execute_CanAttack(this)) return;
	
	UPXASComponent* ASC = Cast<UPXASComponent>(GetAbilitySystemComponent());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC)
	
	if (ASC->TryActivateAbilityByTagName("Ability.NormalAttack"))
	{
		bAttackStartup = true;
		SetAttackAnimToggle(true);
		OnPlayerAttackStart.Broadcast();
	}
}

void ABasePXCharacter::AttackRelease()
{
	EndNormalAttack();
	if (bAttackHolding)
	{
		OnAttackHoldingRelease();
	}
	else
	{
		OnAttackRelease();
	}
}

void ABasePXCharacter::TryToJump()
{
	bool CanTry = false;
	if (AbilityComponent)
	{
		AbilityComponent->OnKeyPressed("InputAction.Jump", CanTry);
	}
	if (!CanTry) return;

	if (SelfCanJump())
	{
		JumpStart();
	}
	else
	{
		UTimerSubsystemFuncLib::SetDelayLoop(GetWorld(),"TryToJump",
		[WeakThis = TWeakObjectPtr<ABasePXCharacter>(this)]{
				if (WeakThis.IsValid())
				{
					if (WeakThis->SelfCanJump())
					{
						WeakThis->JumpStart();
						UTimerSubsystemFuncLib::CancelDelay(WeakThis->GetWorld(),"TryToJump");
					}
				}
		}, 0.016f, 0.2f);
	}
}

void ABasePXCharacter::JumpRelease()
{
	StopJumping();
}
