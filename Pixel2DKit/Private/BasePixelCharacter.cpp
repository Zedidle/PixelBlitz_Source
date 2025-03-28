// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePixelCharacter.h"
#include "Anim/BasePixelAnimInstance.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Basic/BasePixelGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Fight/Components/FightComponent.h"
#include "Fight/Components/HealthComponent.h"


void ABasePixelCharacter::Tick_SaveFallingStartTime()
{
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
	{
		if (!PreFrameFalling)
		{
			PreFrameFalling = true;
			if (UWorld* World = GetWorld())
			{
// 				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red,
// FString::Printf(TEXT("Tick_SaveFallingStartTime: %d"),  __LINE__));
				FallingStartTime = World->TimeSeconds;
			}
		}
	}
}

void ABasePixelCharacter::Tick_SpriteRotation_cpp()
{
	if (!GetCharacterMovement() || GetCharacterMovement()->Velocity.Size2D() < 1.0f) return;

	if (!GetSprite()) return;
	
	FVector Velocity = GetCharacterMovement()->Velocity;

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

void ABasePixelCharacter::Tick_SpringArmMotivation_cpp()
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
	
	if (UBasePixelGameInstance* GameInstance = Cast<UBasePixelGameInstance>(GetGameInstance()))
	{
		switch (GameInstance->VideoSetting_CameraMode)
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

void ABasePixelCharacter::Tick_CalCameraOffset()
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


ABasePixelCharacter::ABasePixelCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HealthComponent_CPP = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent_CPP"));
	FightComponent = CreateDefaultSubobject<UFightComponent>(TEXT("FightComponent"));
}

UHealthComponent* ABasePixelCharacter::GetHealthComponent() const
{
	return HealthComponent_CPP;
}

UFightComponent* ABasePixelCharacter::GetFightComponent() const
{
	return FightComponent;
}

void ABasePixelCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitScale = GetActorScale3D();
	SetFalling(GetCharacterMovement()->IsFalling());
}

void ABasePixelCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScaleTimerHandle);
	}
	
}

void ABasePixelCharacter::Falling()
{
	Super::Falling();
	SetFalling(true);
}

FVector ABasePixelCharacter::GetMoveForwardVector()
{
	return FRotator(0, CurBlendYaw,0 ).RotateVector(FVector(1,0,0));
}

FVector ABasePixelCharacter::GetFaceToCamera()
{
	return FRotator(0, CurBlendYaw + 90,0 ).RotateVector(FVector(1,0,0));
}

void ABasePixelCharacter::AddCameraOffset(const FVector& V)
{
	SurCameraOffset += V;
}

void ABasePixelCharacter::SetDead(bool V)
{
	bDead = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetDead(V);
	}
}

void ABasePixelCharacter::SetHurt(const bool V, const float duration)
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
			SetHurt(false,0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, duration, false);
}

void ABasePixelCharacter::SetJumping(bool V, const float time)
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
			SetJumping(false,0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, time, false);
}

void ABasePixelCharacter::SetFalling(bool V)
{
	bFalling = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetFalling(V);
	}
}

void ABasePixelCharacter::SetLanding(bool V, float time)
{
	bLanding = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetLanding(V);
	}

	if (!bLanding) return;

	PreFrameFalling = false;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
[this]{
			SetLanding(false,0);
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, time, false);
}


void ABasePixelCharacter::SetDashing(bool V)
{
	bDashing = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetDashing(V);
	}
}

void ABasePixelCharacter::SetAttackStart(bool V)
{
	bAttackStart = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetAttackStart(V);
	}
}

void ABasePixelCharacter::SetAttackHolding(bool V)
{
	bAttackHolding = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetAttackHolding(V);
	}
}

void ABasePixelCharacter::SetAttackFire(bool V)
{
	bAttackFire = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetAttackFire(V);
	}
}

void ABasePixelCharacter::SetMoving(bool V)
{
	bMoving = V;
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(GetAnimInstance()))
	{
		AnimInst->SetMoving(V);
	}
}

float ABasePixelCharacter::GetFallingTime()
{
	if (UWorld* World = GetWorld())
	{
		return World->TimeSeconds - FallingStartTime;
	}
	return 0;
}



void ABasePixelCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Tick_SaveFallingStartTime();
	Tick_SpriteRotation_cpp();
	Tick_SpringArmMotivation_cpp();
	Tick_CalCameraOffset();
	
	if (GetCharacterMovement())
	{
		SetMoving(GetCharacterMovement()->Velocity.Size() > 1.0f);
	}
	
}

FVector ABasePixelCharacter::GetDashDirection()
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

FGameplayTagContainer ABasePixelCharacter::GetOwnCamp_Implementation()
{
	return IFight_Interface::GetOwnCamp_Implementation();
}

FGameplayTagContainer ABasePixelCharacter::GetEnemyCamp_Implementation()
{
	return IFight_Interface::GetEnemyCamp_Implementation();
}

AActor* ABasePixelCharacter::GetTarget_Implementation()
{
	return IFight_Interface::GetTarget_Implementation();
}

void ABasePixelCharacter::OnAttackHiting_Implementation()
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

void ABasePixelCharacter::PowerRepulsion_Implementation(float Power)
{
	IFight_Interface::PowerRepulsion_Implementation(Power);
}

void ABasePixelCharacter::OnBeAttacked_Invulnerable_Implementation()
{
	IFight_Interface::OnBeAttacked_Invulnerable_Implementation();
}

bool ABasePixelCharacter::OnBeAttacked_Implementation(AActor* maker, int damage)
{
	return IFight_Interface::OnBeAttacked_Implementation(maker, damage);
}


int ABasePixelCharacter::DamagePlus_Implementation(int inValue, AActor* ActorAcceptDamage)
{
	return 0;
}

int ABasePixelCharacter::OnDefendingHit_Implementation(int inValue)
{
	return inValue;
}

void ABasePixelCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
                                                           const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation,
	                                        TimeDelta);
	SetFalling(true);
	SetMoving(false);
	GetCharacterMovement()->MaxWalkSpeed = BasicMoveSpeed;
}

void ABasePixelCharacter::Jump()
{
	Super::Jump();
	SetJumping(true);
	// SetFalling(true); // 取决于落地翻滚时间从起跳还是下落的开始算
}


void ABasePixelCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	SetLanding(true);
	SetJumping(false);
	SetFalling(false);
}

void ABasePixelCharacter::SetScale(const float targetValue)
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

void ABasePixelCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
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
			&ABasePixelCharacter::MoveY
		);
	}
	
	if (Action_MoveFront)
	{
		EnhancedInput->BindAction(
			Action_MoveFront,
			ETriggerEvent::Triggered,
			this,
			&ABasePixelCharacter::MoveY
		);
	}

	if (Action_MoveLeft)
	{
		EnhancedInput->BindAction(
			Action_MoveLeft,
			ETriggerEvent::Triggered,
			this,
			&ABasePixelCharacter::MoveX
		);
	}

	if (Action_MoveRight)
	{
		EnhancedInput->BindAction(
			Action_MoveRight,
			ETriggerEvent::Triggered,
			this,
			&ABasePixelCharacter::MoveX
		);
	}

	if (Action_MoveGP)
	{
		EnhancedInput->BindAction(
			Action_MoveGP,
			ETriggerEvent::Triggered,
			this,
			&ABasePixelCharacter::Move2D
		);
	}
}

void ABasePixelCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
	FVector velocity = GetCharacterMovement()->Velocity.GetSafeNormal();
	FVector dir = WorldDirection.GetSafeNormal2D() * ScaleValue;

	// 还需通过 **AbilityComponent** 补充技能【急转向】逻辑
	if (dir.Dot(velocity) < -0.7 && GetCharacterMovement()->IsMovingOnGround()) // 接近反方向
	{
		float speed = GetCharacterMovement()->Velocity.Length();
		GetCharacterMovement()->Velocity = 0.5 * speed * dir;
	}
}

void ABasePixelCharacter::MoveX(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue == 0) return;
	AddMovementInput(GetMoveForwardVector(), AxisValue, false);
	
}

void ABasePixelCharacter::MoveY(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue == 0) return;
	AddMovementInput(GetFaceToCamera(), AxisValue, false);
	
}

void ABasePixelCharacter::Move2D(const FInputActionValue& Value)
{
	FVector2D AxisValue = Value.Get<FVector2D>();
	if (AxisValue.X == 0 && AxisValue.Y == 0) return;
	MoveX(AxisValue.X);
	MoveY(AxisValue.Y);
}

