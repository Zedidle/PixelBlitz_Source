// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasePXCharacter.h"

#include "AbilitySystemComponent.h"
#include "Animation/BasePXAnimInstance.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Core/PXGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PXGameplayTags.h"
#include "Core/PXGameMode.h"
#include "Core/PXGameState.h"
#include "Controller/PXPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Character/PXCharacterDataAsset.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/StateComponent.h"
#include "Subsystems/PXAnimSubsystem.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "GAS/PXASComponent.h"
#include "GeometryCollection/GeometryCollectionParticlesData.h"
#include "Input/PXInputComponent.h"
#include "Item/BaseInteractableItem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PXCharacterPlayerState.h"
#include "Player/PXLocalPlayer.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/PXSettingsShared.h"
#include "Settings/Config/PXCameraSourceDataAsset.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXForceFeedbackEffectDataAsset.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Settings/Config/PXWidgetsDataAsset.h"
#include "Sound/SoundCue.h"
#include "Subsystems/AchievementSubsystem.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "UI/UIManager.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/PXGameplayStatics.h"
#include "Utilitys/SoundFuncLib.h"
#include "Utilitys/UserWidgetFuncLib.h"


#define LOCTEXT_NAMESPACE "PX"

class UPXInputComponent;
class UDataTableSubsystem;

void ABasePXCharacter::LoadData()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataAsset);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);

	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);

	const FCharacterAttribute& Attribute = DataAsset->CharacterAttribute;

	CachedASC->SetAttributeValue(EPXAttribute::BasicMaxHP, Attribute.BasicMaxHP);
	CachedASC->SetAttributeValue(EPXAttribute::CurMaxHP, Attribute.BasicMaxHP);
	CachedASC->SetAttributeValue(EPXAttribute::HP, Attribute.BasicMaxHP);

	CachedASC->SetAttributeValue(EPXAttribute::BasicMaxEP, Attribute.BasicMaxEP);
	CachedASC->SetAttributeValue(EPXAttribute::CurMaxEP, Attribute.BasicMaxEP);
	CachedASC->SetAttributeValue(EPXAttribute::EP, Attribute.BasicMaxEP);
	
	CachedASC->SetAttributeValue(EPXAttribute::BasicRepelResist, Attribute.BasicRepelResist);
	CachedASC->SetAttributeValue(EPXAttribute::CurRepelResist, Attribute.BasicRepelResist);
	CachedASC->SetAttributeValue(EPXAttribute::BasicDashSpeed, Attribute.BasicDashSpeed);
	CachedASC->SetAttributeValue(EPXAttribute::CurDashSpeed, Attribute.BasicDashSpeed);
	CachedASC->SetAttributeValue(EPXAttribute::BasicDashCD, Attribute.BasicDashCD);
	CachedASC->SetAttributeValue(EPXAttribute::CurDashCD, Attribute.BasicDashCD);
	CachedASC->SetAttributeValue(EPXAttribute::BasicAirControl, Attribute.BasicAirControl);
	CachedASC->SetAttributeValue(EPXAttribute::CurAirControl, Attribute.BasicAirControl);
	CachedASC->SetAttributeValue(EPXAttribute::BasicSpeed, Attribute.BasicSpeed);
	CachedASC->SetAttributeValue(EPXAttribute::CurSpeed, Attribute.BasicSpeed);
	CachedASC->SetAttributeValue(EPXAttribute::BasicAcceleration, Attribute.BasicAcceleration);
	CachedASC->SetAttributeValue(EPXAttribute::CurAcceleration, Attribute.BasicAcceleration);
	CachedASC->SetAttributeValue(EPXAttribute::BasicJumpSpeed, Attribute.BasicJumpSpeed);
	CachedASC->SetAttributeValue(EPXAttribute::CurJumpSpeed, Attribute.BasicJumpSpeed);
	CachedASC->SetAttributeValue(EPXAttribute::BasicGravityScale, Attribute.BasicGravityScale);
	CachedASC->SetAttributeValue(EPXAttribute::CurGravityScale, Attribute.BasicGravityScale);
	CachedASC->SetAttributeValue(EPXAttribute::BasicJumpMaxHoldTime, Attribute.BasicJumpMaxHoldTime);
	CachedASC->SetAttributeValue(EPXAttribute::CurJumpMaxHoldTime, Attribute.BasicJumpMaxHoldTime);
	CachedASC->SetAttributeValue(EPXAttribute::BasicSight, Attribute.BasicSight);
	CachedASC->SetAttributeValue(EPXAttribute::CurSight, Attribute.BasicSight);
	CachedASC->SetAttributeValue(EPXAttribute::BasicAttackValue, Attribute.BasicAttackValue);
	CachedASC->SetAttributeValue(EPXAttribute::CurAttackValue, Attribute.BasicAttackValue);
	CachedASC->SetAttributeValue(EPXAttribute::BasicAttackCD, Attribute.BasicAttackCD);
	CachedASC->SetAttributeValue(EPXAttribute::CurAttackCD, Attribute.BasicAttackCD);
	CachedASC->SetAttributeValue(EPXAttribute::BasicRepelValue, Attribute.BasicRepelValue);
	CachedASC->SetAttributeValue(EPXAttribute::CurRepelValue, Attribute.BasicRepelValue);
	CachedASC->SetAttributeValue(EPXAttribute::BasicMaxJumpCount, Attribute.BasicMaxJumpCount);
	CachedASC->SetAttributeValue(EPXAttribute::CurMaxJumpCount, Attribute.BasicMaxJumpCount);


#pragma region GAS
	InitEffects.Append(DataAsset->InitEffects);
	// AttributeSet 初始化
	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent())
	{
		for (auto Ability : DataAsset->InitAbilitiesToGive)
		{
			AbilitySystem->K2_GiveAbility(Ability);
		}

		FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		
		for (auto Effect : InitEffects)
		{
			if (!Effect) continue;
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


FVector ABasePXCharacter::GetSpriteForwardVector()
{
	if (GetSprite())
	{
		return GetSprite()->GetForwardVector();
	}

	return FVector::ForwardVector;
}

void ABasePXCharacter::Tick_SaveFallingStartTime(float DeltaSeconds)
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

void ABasePXCharacter::Tick_SpriteRotation(float DeltaSeconds)
{
	if (!GetCharacterMovement() || GetCharacterMovement()->Velocity.Size2D() < 1.0f) return;

	if (!GetSprite()) return;
	FVector Acceleration = GetCharacterMovement()->GetCurrentAcceleration();

	FVector Velocity;
	if (Acceleration.IsZero())
	{
		Velocity = GetCharacterMovement()->Velocity;
	}
	else
	{
		Velocity = GetCharacterMovement()->GetCurrentAcceleration();
	}

	float Dot_VelocityToCamera = FVector::DotProduct(Velocity.GetSafeNormal2D(MinDot_VelocityToCamera), GetVectorFaceToCamera().GetSafeNormal2D());
	float CP_VelocityToCamera_Z = FVector::CrossProduct(Velocity.GetSafeNormal2D(MinCP_VelocityToCamera), GetVectorFaceToCamera().GetSafeNormal2D()).Z;

	bool MoveForward = Dot_VelocityToCamera < -0.1f;
	bool MoveBack = Dot_VelocityToCamera > 0.1f;
	bool MoveLeft = CP_VelocityToCamera_Z < -0.01f;
	bool MoveRight = CP_VelocityToCamera_Z > 0.01f;

	if (MoveForward)
	{
		if (TurnRight)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, -0.5), FVector2D(0, -45), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
		else if (TurnLeft)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, -0.5), FVector2D(-180, -135), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
		else if (MoveRight)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, -0.5), FVector2D(0, -45), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
		else if (MoveLeft)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, -0.5), FVector2D(-180, -135), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
	}
	else if (MoveBack)
	{
		if (TurnRight)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, 0.5), FVector2D(180, 135), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
		else if (TurnLeft)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, 0.5), FVector2D(0, 45), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
		else if (MoveRight)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, 0.5), FVector2D(180, 135), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
		else if (MoveLeft)
		{
			float RotateYaw = FMath::GetMappedRangeValueClamped( FVector2D(0, 0.5), FVector2D(0, 45), Dot_VelocityToCamera);
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + RotateYaw, 0));
		}
	}
	else
	{
		if (MoveRight)
		{
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw, 0));
		}
		else if (MoveLeft)
		{
			GetSprite()->SetWorldRotation(FRotator(0, CurBlendYaw + 180, 0));
		}
	}

	
	if (DeltaBlendYaw > 0.1f)
	{
		TurnLeft = true;
		TurnRight = false;
	}
	else if (DeltaBlendYaw < -0.1f)
	{
		TurnLeft = false;
		TurnRight = true;
	}
	DeltaBlendYaw = 0;
}

void ABasePXCharacter::Tick_SpringArmMotivation(float DeltaSeconds)
{
	if (!GetCharacterMovement()) return;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SpringArm)

	UPXSettingsShared* SettingsShared = UPXGameplayStatics::GetSettingsShared(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SettingsShared)

	FVector Velocity = GetCharacterMovement()->Velocity;
	
	CurDot_VelocityToCamera = FMath::Lerp(CurDot_VelocityToCamera, FVector::DotProduct(GetVectorFaceToCamera(), Velocity.GetSafeNormal()), DeltaSeconds);

	// 镜头偏转
	float yaw = FVector::DotProduct(GetRightVectorWithBlendYaw(), Velocity.GetSafeNormal()) * 5 + CurBlendYaw - 90;
	SpringArm->SetRelativeRotation(FRotator(CurBlendPitch, yaw, 0));

	
	// 镜头偏移
	if (Velocity.Size() > 0)
	{
		// 朝向 / 反向 摄像机的偏移削减比例
		float DeOffsetPercent = 0.3 * FMath::Abs(CurDot_VelocityToCamera);
		Velocity *= 1 - DeOffsetPercent;

		switch (SettingsShared->GetCameraFollowMode())
		{
		case ECameraFollowMode::Preview: // 镜头前瞻
			{
				Velocity *= FrontViewFactor;
				// 设置
				float PreviewCameraFactor = 0.9;
				float VelocitySize = FMath::Pow(Velocity.Size(), PreviewCameraFactor);
				// 假设视角是90°
				FVector Offset = Velocity.GetSafeNormal() * FMath::Min(VelocitySize, CurSpringArmLength / 3);
				AddCameraOffset(FName("Move"), Offset);
				break;
			}
		case  ECameraFollowMode::Behind: // 镜头跟随
			{
				Velocity *= FollowViewFactor;
				// 设置
				float FollowCameraFactor = 0.8;
				float VelocitySize = FMath::Pow(Velocity.Size(), FollowCameraFactor);
				SpringArm->CameraLagSpeed = VelocitySize / 6 + 1;
				FVector Offset = Velocity.GetSafeNormal() * VelocitySize;
				AddCameraOffset(FName("Move"), Offset);
				break;
			}
		}
	}
	else
	{
		RemoveCameraOffset(FName("Move"));
	}


	TArray<FVector> OffsetValues;
	CameraOffsetMap.GenerateValueArray(OffsetValues);
	
	// 一般都是计算水平偏移，与Z无关
	FVector NewTargetCameraOffset;
	if (UCommonFuncLib::CalAverageByArray(OffsetValues, NewTargetCameraOffset))
	{
		CurCameraOffset = FMath::Lerp(CurCameraOffset, NewTargetCameraOffset, SettingsShared->GetCameraOffsetSpeed() * DeltaSeconds);
	}
	else
	{
		CurCameraOffset = FMath::Lerp(CurCameraOffset, FVector(0), SettingsShared->GetCameraOffsetSpeed() * DeltaSeconds);
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
	FString::Printf(TEXT("Tick_SpringArmMotivation CurCameraOffset : %s"), *CurCameraOffset.ToString()));
	
	FVector NewSpringArmLocation = GetActorLocation() + CurCameraOffset;
	NewSpringArmLocation.Z = FMath::Lerp(PreSpringArmZ, NewSpringArmLocation.Z, 0.1);
	
	SpringArm->SetWorldLocation(NewSpringArmLocation);
	PreSpringArmZ = NewSpringArmLocation.Z;
	
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

void ABasePXCharacter::AddCameraOffset(FName OffsetName, FVector Offset, float SustainTime)
{
	if (CameraOffsetMap.Contains(OffsetName))
	{
		CameraOffsetMap[OffsetName] = Offset;
	}
	else
	{
		CameraOffsetMap.Add(OffsetName, Offset);
	}

	if (SustainTime > 0)
	{
		UTimerManagerFuncLib::SetDelay(GetWorld(), 
	[WeakThis = TWeakObjectPtr(this), OffsetName]
		{
			if (!WeakThis.IsValid()) return;
			
			WeakThis->RemoveCameraOffset(OffsetName);
		}, SustainTime);
	}
}

void ABasePXCharacter::RemoveCameraOffset(FName OffsetName)
{
	if (CameraOffsetMap.Contains(OffsetName))
	{
		CameraOffsetMap.Remove(OffsetName);
	}
}


void ABasePXCharacter::LoadAbility_Implementation()
{
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
	FightComponent = CreateDefaultSubobject<UFightComponent>(TEXT("FightComponent"));
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 10.f;
	SpringArm->CameraRotationLagSpeed = 10.f;
	SpringArm->CameraLagMaxDistance = 5.f;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	JumpMaxCount = 9999;

	if (GetSprite())
	{
		GetSprite()->CastShadow = true;
	}
}

void ABasePXCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 莫名其妙在构造函数生成的会失效
	StateComponent = NewObject<UStateComponent>(this, TEXT("StateComponent"));
	if (StateComponent)
	{
		StateComponent->RegisterComponent();
	}
	
	InitScale = GetActorScale3D();

	if (SpringArm)
	{
		PreSpringArmZ = SpringArm->GetComponentLocation().Z;
	}
	
	if (GetCharacterMovement())
	{
		SetFalling(GetCharacterMovement()->IsFalling());
		GetCharacterMovement()->AirControlBoostVelocityThreshold = 600;
		GetCharacterMovement()->AirControlBoostMultiplier = 10.0f;
	}

	CachedASC = Cast<UPXASComponent>(GetAbilitySystemComponent());
	if (CachedASC)
	{
		if (const UPXAttributeSet* PXAttributeSet = CachedASC->GetSet<UPXAttributeSet>())
		{
			PXAttributeSet->OnPXAttributeChange.AddDynamic(this, &ThisClass::OnAttributeChanged);
		}
	}
	
	LoadData();
	LoadWeapon();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle_OnLevelLoading = MessageSubsystem.RegisterListener(PXGameplayTags::GameplayFlow_OnLevelLoading, this, &ThisClass::OnLevelLoading);
	ListenerHandle_OnLevelLoaded = MessageSubsystem.RegisterListener(PXGameplayTags::GameplayFlow_OnLevelLoaded, this, &ThisClass::OnLevelLoaded);
	
	if (AbilityComponent)
	{
		AbilityComponent->InitAbilities();
	}
}

void ABasePXCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UTimerManagerFuncLib::CancelDelay(this, "ABasePXCharacter::SetScale");
	
	OnPlayerAttackStart.RemoveAll(this);
	OnPlayerDie.RemoveAll(this);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle_OnLevelLoading);
	MessageSubsystem.UnregisterListener(ListenerHandle_OnLevelLoaded);

	if (CachedASC)
	{
		if (const UPXAttributeSet* PXAttributeSet = CachedASC->GetSet<UPXAttributeSet>())
		{
			PXAttributeSet->OnPXAttributeChange.RemoveDynamic(this, &ThisClass::OnAttributeChanged);
		}
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
	if (CurJumpCount >= MaxJumpCount)
	{
		return false;
	}
	return true;
}

void ABasePXCharacter::JumpStart_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	CachedASC->TryActivateAbilityByTagName("Ability.Jump");
}

void ABasePXCharacter::Jump()
{
	Super::Jump();
	SetJumping(true);

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	CachedASC->TryActivateAbilityByTagName("Ability.StepOnHead");
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

	if (AbilityComponent)
	{
		AbilityComponent->OnLanding();
	}
	
	if (DataAsset && DataAsset->LandedSound)
	{
		USoundFuncLib::PlaySoundAtLocation(DataAsset->LandedSound.Get(), GetActorLocation());
	}
	
	SetLanding(true);
	SetJumping(false);
	SetFalling(false);
	CurJumpCount = 0;
	
	if (const UPXCustomSettings* CustomSettings = GetDefault<UPXCustomSettings>())
	{
		if (UPXCameraResourceDataAsset* DA = CustomSettings->CameraResourceDataAsset.LoadSynchronous())
		{
			UGameplayStatics::PlayWorldCameraShake(GetWorld(), DA->PlayerLandedShake, GetActorLocation(), 0.0f, 500.0f, 0.0f, true);
		}
	}
	
	if (bDead)
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->DisableMovement();
		}
	}
	
#pragma region 舞空术成就
	if (JumpStartTime > 0 && UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - JumpStartTime > 10.0f)
	{
		if (UAchievementSubsystem* AchievementSubsystem = UAchievementSubsystem::GetInstance(this))
		{
			AchievementSubsystem->CompleteAchievement("10");
		}
	}
	JumpStartTime = 0;
#pragma endregion
}

void ABasePXCharacter::LoadWeapon(TSubclassOf<ABaseWeapon> WeaponClass)
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);
	
	if (WeaponClass)
	{
		Weapon = World->SpawnActor<ABaseWeapon>(WeaponClass);
	}
	else
	{
		if (DataAsset && DataAsset->WeaponClass)
		{
			Weapon = World->SpawnActor<ABaseWeapon>(DataAsset->WeaponClass);
		}
	}
	if (Weapon)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
		Weapon->AttachToActor(this, Rules);
		Weapon->SetCharacterOwner(this);

		// 附加基础攻击力
		CachedASC->ModifyAttributeValue(EPXAttribute::BasicAttackValue,Weapon->GetWeaponDamage());
	}
}


void ABasePXCharacter::ReadyToStart_Implementation()
{
	if (AbilityComponent)
	{
		AbilityComponent->LoadTalents();
		AbilityComponent->LoadAbilities();
	}
	if (BuffComponent)
	{
		BuffComponent->SetBuffStateWidgetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

int ABasePXCharacter::GetAttackDamage_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(CachedASC, 1)
	return CachedASC->GetAttributeValue(EPXAttribute::CurAttackValue);
	
	// 统一设置
	// FGameplayTag AttackDamagePlusTag = TAG("Ability.SwordPlay.Set.AttackDamagePlus");
	// int AttackValue = CachedASC->GetAttributeValue(EPXAttribute::CurAttackValue);
	// if (Weapon)
	// {
	// 	AttackValue += Weapon->GetWeaponDamage();
	// }
	//
	// AttackValue = CalInitDamage(AttackValue);
	// return AttackValue;
}

FVector ABasePXCharacter::GetAttackRepel_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(CachedASC, FVector::ZeroVector)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(AbilityComponent, FVector::ZeroVector)

	float RepelValue = CachedASC->GetAttributeValue(EPXAttribute::CurRepelValue);
	if (Weapon)
	{
		RepelValue += Weapon->GetRepelPower();
	}

	FVector V = GetVelocity();
	FVector RepelByVelocity = {
		(V.X < 0 ? -1 : 1) * FMath::Pow(FMath::Abs(V.X), 0.9),
		(V.Y < 0 ? -1 : 1) * FMath::Pow(FMath::Abs(V.Y), 0.9),
		 (V.Z < 0 ? -1 : 1) * FMath::Pow(FMath::Abs(V.Z), 0.8)
	};
	
	FVector SpriteForwardVector = GetSpriteForwardVector();
	FVector Result = {
		SpriteForwardVector.X * RepelValue,
		SpriteForwardVector.Y * RepelValue,
		RepelValue
	};

	Result += RepelByVelocity * VelocityRepelFactor;

	// 后续完全可以改成 AttributeEffects 的形式
	float FoundR;
	if (AbilityComponent->FindExtendData(TAG("Ability.Force.Set.PlusPercent"), FoundR))
	{
		Result *= 1 + FoundR;
	}
	return Result;
}

void ABasePXCharacter::Revive_Implementation()
{
	if (StateComponent)
	{
		StateComponent->SetHP(StateComponent->GetMaxHP());
		StateComponent->SetEP(StateComponent->GetMaxEP() * 0.5);
		
	}
	CheckNearDeath();
	
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
	// return FRotator(0, CurBlendYaw + 90,0 ).RotateVector(FVector(1,0,0));
	return FRotator(CurBlendPitch, CurBlendYaw + 90,0 ).RotateVector(FVector(1,0,0));
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
	AddCameraOffset(FName("BulletTime"), TempOffset, SustainTime);
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalTimeRate);
	UTimerManagerFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
	{
		UGameplayStatics::SetGlobalTimeDilation(WeakThis->GetWorld(), 1.0f);
	}, SustainTime);
}

void ABasePXCharacter::OutOfControl(float SustainTime)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 0;
	}

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	FName TimerName = FName(GetName() + "_OutOfControl");
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,[WeakThis = TWeakObjectPtr(this)]{
		if (!WeakThis.IsValid()) return;
		if (!WeakThis->CachedASC) return;
		if (!WeakThis->GetCharacterMovement()) return;

		WeakThis->GetCharacterMovement()->MaxWalkSpeed = WeakThis->CachedASC->GetAttributeValue(EPXAttribute::CurSpeed);
		
	}, SustainTime);
}

FVector ABasePXCharacter::GetDashSpeed()
{
	float DashSpeed = GetDashSpeedValue();
	if (DashSpeed <= 0) return FVector(0);
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(MovementComponent, FVector::ZeroVector);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(AbilityComponent, FVector::ZeroVector);
	
	FVector Result = DashSpeed * GetDashDirection();
	FVector MovementVelocity = GetCharacterMovement()->Velocity;
	Result.X = Result.X * DashInitSpeedPercent + MovementVelocity.X * DashNewDirSpeedPercent;
	Result.Y = Result.Y * DashInitSpeedPercent + MovementVelocity.Y * DashNewDirSpeedPercent;
	Result.Z += MovementVelocity.Z * 0.1;

	// 属性引导优化
	return Result;
}

void ABasePXCharacter::SetDead(bool V)
{
	bDead = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bDead")), V);
	
	if(GetCharacterMovement())
	{	
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}

void ABasePXCharacter::SetHurt(const bool V, const float duration)
{
	bHurt = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bHurt")), V);
	
	if (!bHurt) return;
	if (duration > 0)
	{
		UTimerManagerFuncLib::SetDelay(GetWorld(),[WeakThis = TWeakObjectPtr(this)]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->SetHurt(false, 0);
			}
		}, duration);
	}
}

void ABasePXCharacter::SetJumping(bool V, float time)
{
	bJumping = V;
	UPXAnimSubsystem::SetAnimInstanceProperty(GetAnimInstance(), FName(TEXT("bJumping")), V);
	
	if (!bJumping) return;

	UTimerManagerFuncLib::SetDelay(GetWorld(),[WeakThis = TWeakObjectPtr(this)]
	{
		if (WeakThis.IsValid())
		{
			WeakThis->SetJumping(false, 0);
		}
	}, time);
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
	UTimerManagerFuncLib::SetDelay(GetWorld(),[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->SetLanding(false, 0);
	}, time);
}

void ABasePXCharacter::CheckNearDeath_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(StateComponent)
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXWidgetsDataAsset* WidgetsDataAsset = Settings->WidgetsDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetsDataAsset)

	if (WidgetsDataAsset->NearDeathWidgetClass)
	{
		if (StateComponent->GetHPPercent() < 0.3)
		{
			UUserWidgetFuncLib::AddWidget(WidgetsDataAsset->NearDeathWidgetClass, ESlateVisibility::HitTestInvisible, false);
		}
		else
		{
			UUserWidgetFuncLib::HideWidget(WidgetsDataAsset->NearDeathWidgetClass);
		}
	}
}

void ABasePXCharacter::OnHPChanged(int32 OldValue, int32 NewValue)
{
	if (bDead) return;
	if (OldValue == NewValue) return;

	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(StateComponent)

	int ChangedValue = OldValue - NewValue;
	float ChangedHPPercent = static_cast<float>(ChangedValue) / StateComponent->GetMaxHP();
	
	const UPXCustomSettings* CustomSettings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CustomSettings)
	UPXForceFeedbackEffectDataAsset* ForceFeedbackEffectDataAsset = CustomSettings->ForceFeedbackEffectDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ForceFeedbackEffectDataAsset)


	if (NewValue < OldValue && (Execute_GetOwnCamp(this).HasTag(TAG("Player")) || GetController()->IsLocalController()))
	{
		if (NewValue <= 0)
		{
			UPXGameplayStatics::SpawnForceFeedbackAttached(this, ForceFeedbackEffectDataAsset->PlayerDie.LoadSynchronous(), Owner->GetRootComponent());
		}
		else
		{
			if (ChangedHPPercent <= 0.1)
			{
				UPXGameplayStatics::SpawnForceFeedbackAttached(this, ForceFeedbackEffectDataAsset->LittleDamage.LoadSynchronous(), Owner->GetRootComponent());
			}
			else if (ChangedHPPercent <= 0.25)
			{
				UPXGameplayStatics::SpawnForceFeedbackAttached(this, ForceFeedbackEffectDataAsset->NormalDamage.LoadSynchronous(), Owner->GetRootComponent());
			}
			else
			{
				UPXGameplayStatics::SpawnForceFeedbackAttached(this, ForceFeedbackEffectDataAsset->HugeDamage.LoadSynchronous(), Owner->GetRootComponent());
			}
		}
	}

	if (NewValue > 0)
	{
		// 生命值恢复或降低
		CheckNearDeath();
		if (NewValue == 1)
		{
			HasOneHealthPoint = true;
		}
		if (NewValue < OldValue)
		{
			if (ChangedHPPercent >= 0.02)
			{
				USoundFuncLib::PlaySoundAtLocation(DataAsset->HurtSound.LoadSynchronous(), GetActorLocation());
				if (UPXMainSaveGame* MainSaveGame = UPXSaveGameSubSystemFuncLib::GetMainData(GetWorld()))
				{
					MainSaveGame->HurtTimes ++;
				}
			}
			if (ChangedHPPercent >= 0.1)
			{
				CameraOffset_BulletTime(ChangedHPPercent / 10, FVector(0), 0.1f);
				OutOfControl(UCommonFuncLib::DealDeltaTime(ChangedHPPercent / 20));
				SetHurt(true);
			}
		}
	}
	else
	{
		bInAttackEffect = false;
		SetAttackAnimToggle(false);
		SetDead(true);

		int RemReviveTimes = -1;
		if (AbilityComponent && !StateComponent->DieByFalling)
		{
			AbilityComponent->OnDying(RemReviveTimes);
		}
		
		if (RemReviveTimes >= 0)
		{
			// 原地复活
			if (APXPlayerController* PC = GetController<APXPlayerController>())
			{
				PC->OnCharacterControl(false);
			}
			if (StateComponent)
			{
				StateComponent->InvulnerableForDuration(2.5);
			}
			UTimerManagerFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
			{
				if (!WeakThis.IsValid()) return;
					WeakThis->Revive();
			}, 2);
		}
		else
		{
			bool GameEnd = false;
			GameInstance->OnPlayerDead(GameEnd);
			
			if (GameEnd)
			{
				// 游戏结束
				if (APXPlayerController* PC = GetController<APXPlayerController>())
				{
					PC->OnCharacterControl(false);
				}
				if (PlayerStatusWidget)
				{
					PlayerStatusWidget->RemoveFromParent();
				}
				if (BuffComponent)
				{
					BuffComponent->DestroyComponent();
				}
				if (StateComponent)
				{
					StateComponent->DestroyComponent();
				}
				UTimerManagerFuncLib::SetDelay(GetWorld(),[WeakThis = TWeakObjectPtr(this)]
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
				// 正常从复活点复活
				if (AActor* RespawnPoint = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerRespawnPoint::StaticClass()))
				{
					if (APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld()))
					{
						GS->PassDayTime(DataAsset->RevivePassDayTime, false, false, DataAsset->ReviveDelayTime);
					}
	
					UTimerManagerFuncLib::SetDelay(GetWorld(),
					[WeakThis = TWeakObjectPtr(this), RespawnPoint]
					{
						if (WeakThis.IsValid() && IsValid(RespawnPoint))
						{
							WeakThis->SetActorLocation(RespawnPoint->GetActorLocation());
							WeakThis->Revive();
						}
					}, DataAsset->ReviveDelayTime);
				}
			}

			Execute_OnDie(this);
		}

		StateComponent->DieByFalling = false;
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


void ABasePXCharacter::AddViewYaw(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue == 0) return;

	// UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("AddViewYaw: %f"), AxisValue));	
	AddViewYaw(AxisValue);
}

void ABasePXCharacter::AddViewYaw(float AxisValue)
{
	UPXSettingsShared* Settings = UPXGameplayStatics::GetSettingsShared(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	if (UPXGameplayStatics::IsGamepadControlling(this))
	{
		int Invert = Settings->GetInvertHorizontalAxis_Gamepad() ? -1 : 1;
		float Sensitivity = Settings->GetGamepadLookSensitivity();

		AddBlendYaw( Invert * Sensitivity * UCommonFuncLib::DealDeltaTime(AxisValue));
	}
	else
	{
		int Invert = Settings->GetInvertHorizontalAxis_Mouse() ? -1 : 1;
		float Sensitivity = Settings->GetViewPointSensitivityYaw_Mouse();

		AddBlendYaw( Invert * Sensitivity * UCommonFuncLib::DealDeltaTime(AxisValue));
	}
}


void ABasePXCharacter::AddViewPitch(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue == 0) return;
	
	UPXSettingsShared* Settings = UPXGameplayStatics::GetSettingsShared(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Controller)

	// const UCommonInputSub
	if (UPXGameplayStatics::IsGamepadControlling(this))
	{
		int Invert = Settings->GetInvertVerticalAxis_Gamepad() ? -1 : 1;
		float Sensitivity = Settings->GetGamepadLookSensitivity();
		float Pitch = FMath::Clamp( Invert * Sensitivity * UCommonFuncLib::DealDeltaTime(AxisValue) + CurBlendPitch, ViewPitchRange.X, ViewPitchRange.Y);
		SetBlendPitch(Pitch);
	}
	else
	{
		int Invert = Settings->GetInvertVerticalAxis_Mouse() ? -1 : 1;
		float Sensitivity = Settings->GetViewPointSensitivityPitch_Mouse();
		float Pitch = FMath::Clamp( Invert * Sensitivity * UCommonFuncLib::DealDeltaTime(AxisValue) + CurBlendPitch, ViewPitchRange.X, ViewPitchRange.Y);
		SetBlendPitch(Pitch);
	}
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
	Tick_SaveFallingStartTime(DeltaSeconds);
	Tick_SpriteRotation(DeltaSeconds);
	Tick_SpringArmMotivation(DeltaSeconds);
	
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

		UTimerManagerFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
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
				GameMode->PlayerRespawnPoint->SetLocation(PlayerStart->GetActorLocation());
				SetActorLocation(PlayerStart->GetActorLocation());
			}
		}
	}
}

FVector ABasePXCharacter::GetDashDirection()
{
	USceneComponent* SC = GetSprite();
	if (!SC) return GetActorForwardVector();

	FVector Velocity = GetVelocity();
	if (Velocity.IsZero())
	{
		return SC->GetForwardVector();
	}

	FVector	Acceleration = GetCharacterMovement()->GetCurrentAcceleration();
	if (Acceleration.IsZero())
	{
		return Velocity.GetSafeNormal2D();
	}

	return Acceleration.GetSafeNormal2D();
}

bool ABasePXCharacter::IsAlive_Implementation()
{
	return !bDead;
}

FGameplayTagContainer ABasePXCharacter::GetOwnCamp_Implementation() const
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

void ABasePXCharacter::OnAttackHit_Implementation(AActor* Receiver)
{
	AttackHitComboNum++;
	UTimerManagerFuncLib::SetRetriggerableDelay(this, "Player_AttackHitCombo",
		[WeakThis = TWeakObjectPtr(this)]
	{
		if (WeakThis.IsValid())
		{
			WeakThis->AttackHitComboNum = 0;
		}
	}, 2);
	
	if (AbilityComponent)
	{
		Execute_OnAttackHit(AbilityComponent, Receiver);
	}
}

void ABasePXCharacter::OnSkillHit_Implementation()
{
	if (AbilityComponent)
	{
		Execute_OnSkillHit(AbilityComponent);
	}
}

void ABasePXCharacter::PowerRepulsion_Implementation(float Power)
{
	bRepulsion = true;
	OutOfControl(Power/1000);

	UTimerManagerFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;

		WeakThis->bRepulsion = false;
		UCommonFuncLib::SpawnFloatingText(LOCTEXT("BUFF_DE-STUN", "解除硬直"),
			WeakThis->GetActorLocation(),
		FLinearColor(0.68, 0.35, 1.0f, 1.0f),
			FVector2D(0.8,0.8)
		);
	}, Power/1000);
}

void ABasePXCharacter::OnBeAttacked_Invulnerable_Implementation()
{
	if (bDashing)
	{
		PerfectDodgeTimes++;

		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
		CachedASC->TryActivateAbilityByTagName("Ability.PerfectDodge");

		CameraOffset_BulletTime(0.03f);
	}
}

void ABasePXCharacter::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce)
{
	OutDamage = InDamage;
	if (AbilityComponent)
	{
		Execute_OnBeAttacked(AbilityComponent, Maker, OutDamage, OutDamage, bForce);
	}
}


int ABasePXCharacter::DamagePlus_Implementation(int InDamage, AActor* Receiver)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(AbilityComponent, InDamage)
	
	int Result = 0;
	
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		float AirFightPlus;
		if (AbilityComponent->FindExtendData(TAG("Ability.AirFight.Set.DamagePlus"), AirFightPlus))
		{
			Result += AirFightPlus;
		}
	}

	return Result;
}

void ABasePXCharacter::OnAttackWeakPoint_Implementation(AActor* Receiver)
{
	if (AbilityComponent)
	{
		Execute_OnAttackWeakPoint(AbilityComponent, Receiver);
	}
}

int ABasePXCharacter::OnDefendingHit_Implementation(int inValue)
{
	return inValue;
}

void ABasePXCharacter::OnAttackHolding_Implementation()
{
	IFight_Interface::OnAttackHolding_Implementation();
	SetAttackHolding(true);
	
	const UPXCustomSettings* CustomSettings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CustomSettings)
	UPXForceFeedbackEffectDataAsset* ForceFeedbackEffectDataAsset = CustomSettings->ForceFeedbackEffectDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ForceFeedbackEffectDataAsset)
	
	UPXGameplayStatics::SpawnForceFeedbackAttached(this,ForceFeedbackEffectDataAsset->AttackHolding.LoadSynchronous(), GetOwner()->GetRootComponent());

}

void ABasePXCharacter::OnDefendingHitEffect_Implementation()
{
	IFight_Interface::OnDefendingHitEffect_Implementation();
}

void ABasePXCharacter::OnAnimVulnerableBegin_Implementation()
{
	if (StateComponent)
	{
		StateComponent->SetInvulnerable(true);
	}
}

void ABasePXCharacter::OnAnimVulnerableEnd_Implementation()
{
	if (StateComponent)
	{
		StateComponent->SetInvulnerable(false);
	}
}

void ABasePXCharacter::OnAttackEffect_Implementation()
{
	if (Weapon)
	{
		Weapon->Use();
	}

	const UPXCustomSettings* CustomSettings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CustomSettings)
	UPXForceFeedbackEffectDataAsset* ForceFeedbackEffectDataAsset = CustomSettings->ForceFeedbackEffectDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ForceFeedbackEffectDataAsset)
	
	UPXGameplayStatics::SpawnForceFeedbackAttached(this,ForceFeedbackEffectDataAsset->Attack.LoadSynchronous(), GetOwner()->GetRootComponent());

	if (AbilityComponent)
	{
		Execute_OnAttackEffect(AbilityComponent);
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

void ABasePXCharacter::OnPickGold_Implementation()
{
	if (AbilityComponent)
	{
		Execute_OnPickGold(AbilityComponent);
	}
}

void ABasePXCharacter::OnDashEffectBegin_Implementation()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent)
	
	InDashEffect = true;
	AbilityComponent->OnSkillStart();
	if (bAttackStartup)
	{
		SetAttackAnimToggle(false);
		bAttackStartup = false;
		bInAttackEffect = false;
		
		AbilityComponent->OnAttackDash();
	}
}

void ABasePXCharacter::OnDashEffectEnd_Implementation()
{
	InDashEffect = false;

	if (AbilityComponent)
	{
		Execute_OnDashEffectEnd(AbilityComponent);
	}
}

APawn* ABasePXCharacter::GetPawn_Implementation()
{
	return this;
}

float ABasePXCharacter::GetAttackCD_Implementation()
{
	return CachedASC ? CachedASC->GetAttributeValue(EPXAttribute::CurAttackCD) : 1.0f;
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

void ABasePXCharacter::OnKillEnemy_Implementation()
{
	if (AbilityComponent)
	{
		AbilityComponent->OnKillEnemy();
	}
}




// float ABasePXCharacter::GetSlowDownResistancePercent()
// {
// 	float Result = 0.0f;
// 	
// 	FGameplayTag Tag = TAG("CommonSet.SpeedDownResistance");
// 	if (EffectGameplayTags.Contains(Tag))
// 	{
// 		Result += EffectGameplayTags[Tag];
// 	}
// 	
// 	return Result;
// }


void ABasePXCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
                                                        const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation,
	                                        TimeDelta);
	SetFalling(true);
	SetMoving(false);
	// GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ABasePXCharacter::SetScale(const float targetValue)
{
	ScaleLerpValue = 0;

	UTimerManagerFuncLib::SetDelayLoop(this, "ABasePXCharacter::SetScale", [WeakThis = TWeakObjectPtr(this), targetValue]
	{
		if (!WeakThis.IsValid()) return;

		WeakThis->SetActorScale3D(WeakThis->InitScale * FMath::Lerp(WeakThis->ScaleCurValue, targetValue, WeakThis->ScaleLerpValue));
		WeakThis->ScaleLerpValue += 0.01f;
		if (WeakThis->ScaleLerpValue > 1)
		{
			WeakThis->ScaleCurValue = targetValue;
			WeakThis->SetActorScale3D(WeakThis->InitScale * WeakThis->ScaleCurValue);
			UTimerManagerFuncLib::CancelDelay(WeakThis.Get(), "ABasePXCharacter::SetScale");
		}
	}, 0.02f);
}

int32 ABasePXCharacter::CalInitDamage(int32 InDamage)
{
	// int LocalDamage = InDamage;
	// if (BuffComponent)
	// {
	// 	LocalDamage += InDamage * BuffComponent->EffectedPercent_Attack;
	// 	LocalDamage += BuffComponent->EffectedValue_Attack;
	// }

	// if (AbilityComponent)
	// {
	// 	AbilityComponent->OnBuffCalDamage();
	// 	LocalDamage += AbilityComponent->GetAttackDamagePlus();
	// }
	//
	// return LocalDamage;
	return InDamage;
}

void ABasePXCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!IsPlayerControlled()) return;

	UPXInputComponent* EnhancedInput = Cast<UPXInputComponent>(PlayerInputComponent);
	if (!EnhancedInput) return;
	if (DataAsset)
	{
		EnhancedInput->SetInputConfig(DataAsset->InputConfig.LoadSynchronous());
		EnhancedInput->BindActionByTagName("InputAction.Move", ETriggerEvent::Triggered,this, &ABasePXCharacter::Input_Move2D);
		EnhancedInput->BindActionByTagName("InputAction.ViewPitch", ETriggerEvent::Triggered,this, &ABasePXCharacter::AddViewPitch);
		EnhancedInput->BindActionByTagName("InputAction.ViewYaw", ETriggerEvent::Triggered,this, &ABasePXCharacter::AddViewYaw);
		
		EnhancedInput->BindActionByTagName("InputAction.NormalAttack", ETriggerEvent::Triggered,this, &ABasePXCharacter::TryToAttack);
		EnhancedInput->BindActionByTagName("InputAction.NormalAttack", ETriggerEvent::Completed,this, &ABasePXCharacter::AttackRelease);
		EnhancedInput->BindActionByTagName("InputAction.Jump", ETriggerEvent::Started,this, &ABasePXCharacter::TryToJump);
		EnhancedInput->BindActionByTagName("InputAction.Jump", ETriggerEvent::Completed,this, &ABasePXCharacter::JumpRelease);
		EnhancedInput->BindActionByTagName("InputAction.Interact", ETriggerEvent::Started,this, &ABasePXCharacter::Interact);
		EnhancedInput->BindActionByTagName("InputAction.Skill", ETriggerEvent::Started,this, &ABasePXCharacter::TryUseSkill);
	}
}

void ABasePXCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent)

	FVector velocity = GetCharacterMovement()->Velocity.GetSafeNormal();
	FVector dir = WorldDirection.GetSafeNormal2D() * ScaleValue;

	float FoundR;
	if (AbilityComponent->FindExtendData(TAG("Ability.Brake.Set.Value"), FoundR))
	{
		if (dir.Dot(velocity) < -0.7 && GetCharacterMovement()->IsMovingOnGround()) // 接近反方向
		{
			float speed = GetCharacterMovement()->Velocity.Length();
			GetCharacterMovement()->Velocity = FoundR * speed * dir;
		}
	}
}


void ABasePXCharacter::Input_Move2D(const FInputActionValue& Value)
{
	FVector2D AxisValue = Value.Get<FVector2D>();
	if (AxisValue.X == 0 && AxisValue.Y == 0) return;

	AddMovementInput(GetRightVectorWithBlendYaw(), AxisValue.X, false);
	AddMovementInput(GetVectorFaceToCamera(), -AxisValue.Y, false);
}

void ABasePXCharacter::TryToAttack()
{
	if (!Execute_CanAttack(this)) return;
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	if (CachedASC->TryActivateAbilityByTagName("Ability.NormalAttack"))
	{
		bAttackStartup = true;
		SetAttackAnimToggle(true);

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->Velocity.X /= 1.5;
			GetCharacterMovement()->Velocity.Y /= 1.5;
			GetCharacterMovement()->Velocity.Z /= 5;
		}
		
		// 近战时按下按键时 广播
		if (DataAsset && DataAsset->NormalAttackType == EAttackType::Melee)
		{
			OnPlayerAttackStart.Broadcast(DataAsset->NormalAttackType, GetSpriteForwardVector());
		}
	}
}

void ABasePXCharacter::AttackRelease()
{
	EndNormalAttack();

	// 远战攻击释放时 广播 ， 后续有什么近战蓄力技能的释放再考虑额外处理 ？ 
	if (DataAsset && DataAsset->NormalAttackType == EAttackType::Projectile)
	{
		OnPlayerAttackStart.Broadcast(DataAsset->NormalAttackType, GetSpriteForwardVector());
	}
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
	if (SelfCanJump())
	{
		JumpStart();
		return;
	}
	
	UTimerManagerFuncLib::SetDelayLoop(GetWorld(),"TryToJump",[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		if (WeakThis->SelfCanJump())
		{
			WeakThis->JumpStart();
			UTimerManagerFuncLib::CancelDelay(WeakThis->GetWorld(),"TryToJump");
		}
	}, 0.01f, 0.15f);
}

void ABasePXCharacter::JumpRelease()
{
	StopJumping();
}

void ABasePXCharacter::Interact()
{
	bool CanTry = false;
	if (AbilityComponent)
	{
		AbilityComponent->OnInteract(CanTry);
	}
	if (!CanTry) return;

	// 尝试交互
	if (!InteractableItems.IsEmpty())
	{
		for (int i = InteractableItems.Num() - 1; i >= 0; i--)
		{
			auto Item = InteractableItems[i];
			InteractableItems.RemoveAt(i);
			if (!Item.IsValid()) continue;

			Item->OnInteractEffect(this);
			break;
		}
	}
}


void ABasePXCharacter::TryUseSkill()
{
	BP_TryUseSkill();
}

void ABasePXCharacter::AddInteractableItem(ABaseInteractableItem* Item)
{
	InteractableItems.Add(Item);
}

void ABasePXCharacter::RemoveInteractableItem(ABaseInteractableItem* Item)
{
	if (InteractableItems.Contains(Item))
	{
		InteractableItems.Remove(Item);
	}
}

void ABasePXCharacter::OnAttributeChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Movement)

	if (Attribute == UPXAttributeSet::GetHPAttribute())
	{
		OnHPChanged(OldValue, NewValue);
	}
	if (Attribute == UPXAttributeSet::GetCurSpeedAttribute())
	{
		Movement->MaxWalkSpeed = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurAccelerationAttribute())
	{
		Movement->MaxAcceleration = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurJumpSpeedAttribute())
	{
		Movement->JumpZVelocity = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurGravityScaleAttribute())
	{
		Movement->GravityScale = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurAirControlAttribute())
	{
		Movement->AirControl = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurJumpMaxHoldTimeAttribute())
	{
		JumpMaxHoldTime = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurMaxJumpCountAttribute())
	{
		MaxJumpCount = NewValue;
	}
	if (Attribute == UPXAttributeSet::GetCurSightAttribute())
	{
		FVector Location = GetActorLocation();

		float BasicSight = CachedASC->GetAttributeValue(EPXAttribute::BasicSight);
		CurSpringArmLength = NewValue;
		if (PlayerStatusWidget)
		{
			PlayerStatusWidget->UpdateDark(NewValue / BasicSight);
		}
		if (BasicSight > NewValue)
		{
			UCommonFuncLib::SpawnFloatingText(LOCTEXT("Buff_Myopia", "短视"),
				Location + FVector(10.0, 0, 20),
				FLinearColor::White,
				FVector2D(0.9, 0.9)
			);
		}
		else
		{
			UCommonFuncLib::SpawnFloatingText(LOCTEXT("Buff_Hyperopia", "远视"),
				Location + FVector(-50, 0, 20),
			FLinearColor::Gray);
		}
	}

	if (Attribute == UPXAttributeSet::GetCurSpeedAttribute())
	{
		FVector Location = GetActorLocation();
		
		float BasicSpeed = CachedASC->GetAttributeValue(EPXAttribute::BasicSpeed);
		if (BasicSpeed > NewValue)
		{
			UCommonFuncLib::SpawnFloatingText( LOCTEXT("BUFF_SLOWDOWN", "减速"),
					Location + FVector(20, 0, 20),
					FLinearColor::Gray);
		}
		else if (NewValue > OldValue)
		{
			UCommonFuncLib::SpawnFloatingText( LOCTEXT("BUFF_SPEEDUP", "加速"),
					Location + FVector(-30, 0, 20),
					FLinearColor::White);
		}
	}
}

float ABasePXCharacter::GetDashSpeedValue()
{
	return CachedASC ? CachedASC->GetAttributeValue(EPXAttribute::CurDashSpeed) : 0.0f;
}

void ABasePXCharacter::OnLevelLoading_Implementation(FGameplayTag Channel, const FDefaultEmptyMessage& Message)
{

}

void ABasePXCharacter::OnLevelLoaded_Implementation(FGameplayTag Channel, const FDefaultEmptyMessage& Message)
{
}



#undef LOCTEXT_NAMESPACE
