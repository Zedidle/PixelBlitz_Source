// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/HealthComponent.h"

#include "PaperFlipbookComponent.h"
#include "SceneRenderTargetParameters.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/Fight_Interface.h"



int UHealthComponent::CalAcceptDamage(int initDamage, AActor* instigator)
{
	AActor* owner = GetOwner();
	if (!owner) return initDamage;
	
	int tmpDamageAdd = 0;
	if (instigator && instigator->Implements<UFight_Interface>())
	{
		tmpDamageAdd = IFight_Interface::Execute_DamagePlus(instigator, initDamage, GetOwner());
	}

	if (!owner->Implements<UFight_Interface>()) return initDamage;

	FGameplayTagContainer ownerTags = IFight_Interface::Execute_GetOwnCamp(owner);
	if (ownerTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Player"))))
	{
		return tmpDamageAdd + initDamage;
	}

	if (GetCurrentHealth() <= 0) return 0;
	
	// 判断是否为BOSS
	if (ownerTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Enemy.BOSS"))))
	{
		// 技能【巨人杀手】处理，AbilityComponent_CPP
		float xxx = 1.5;
		initDamage = FMath::RoundToInt(xxx * initDamage);
	}
	
	return initDamage + tmpDamageAdd;
}

void UHealthComponent::OnHurtInvulnerable()
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
		[this]
		{
			InvulnerableForDuration(InvulnerableDuration);
		});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1, false, OnHurtInvulnerableDelay);
}

void UHealthComponent::InvulnerableForDuration(float duration)
{
	bInvulnerable = true;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
		[this, &TimerHandle]
		{
			bInvulnerable = false;
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1, false, duration);
}

void UHealthComponent::FlashForDuration(FLinearColor color, float duration, bool force) 
{
	if (bInvulnerable && !force) return;

	float curTime = 0;
	bool bFlashing = false; // 是否在受伤闪烁
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
		[this, &TimerHandle, &curTime, &duration, &bFlashing, &color]
		{
			if (curTime > duration)
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			}

			curTime+=0.2;
			bFlashing = !bFlashing;
			if (UPaperFlipbookComponent* PF = GetOwner()->GetComponentByClass<UPaperFlipbookComponent>())
			{
				PF->SetSpriteColor(bFlashing ? color : FLinearColor::White);
			}
		});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 0.2, true);
	
}

FVector UHealthComponent::GetRepel(FVector IncomeRepel, AActor* Instigator)
{
	return IncomeRepel;
}

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int UHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

void UHealthComponent::DecreaseHealth(int Damage, const FVector& KnockbackForce, AActor* Instigator, bool bForce,
                                      bool bCauseInvul, bool bInner)
{
	if (Damage <= 0) return;
	AActor* owner = GetOwner();
	if (!owner->Implements<UFight_Interface>()) return;

	// 玩家受伤逻辑
	FGameplayTag PlayerTag = FGameplayTag::RequestGameplayTag(FName("Player"));
	if (IFight_Interface::Execute_GetOwnCamp(owner).HasTag(PlayerTag))
	{
		
		
	}

	// 怪物受伤逻辑
	FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag(FName("Enemy"));
	if (IFight_Interface::Execute_GetOwnCamp(owner).HasTag(EnemyTag))
	{
		if (bInvulnerable && !bForce)
		{
			if (!owner->Implements<UFight_Interface>()) return;
			IFight_Interface::Execute_OnBeAttacked_Invulnerable(owner);
		}
	}

	int calDamage = CalAcceptDamage(Damage, Instigator);
	calDamage = IFight_Interface::Execute_OnDefendingHit(owner, calDamage);

	int preHealth = CurrentHealth;
	CurrentHealth = FMath::Max(CurrentHealth - calDamage, 0);
	OnHealthChanged.Broadcast(CurrentHealth, preHealth - CurrentHealth, EStatChange::Decrease, Instigator, bInner);

	if (bCauseInvul)
	{
		OnHurtInvulnerable();
	}
	if (!bInner)
	{
		FlashForDuration();
	}
	
}

void UHealthComponent::IncreaseHealth()
{
}

void UHealthComponent::KnockBack(FVector Repel, AActor* Instigator)
{
	if (bInRock)
	{
		// SpawnFloatingText 击退无效
		return;
	}

	if (bInvulnerable) return;

	Repel = GetRepel(Repel, Instigator) * KnockBackMultiplier;
	
	if (UCharacterMovementComponent* MovementComponent = GetOwner()->GetComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->AddImpulse(Repel , true);
	}
	else if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
	{
		if (PrimitiveComponent->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics)
		{
			PrimitiveComponent->AddImpulse(Repel, FName(), true);
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}

	// 如果受力大于300则属于强效击退
	if (Repel.Size() > 300)
	{
		if (GetOwner()->Implements<UFight_Interface>())
		{
			if (IFight_Interface::Execute_GetOwnCamp(GetOwner())
				.HasTag(FGameplayTag::RequestGameplayTag(FName("Player"))))
			{
				
			}
		}
		
	}

	

	
	FVector ownerLocation = GetOwner()->GetActorLocation();

	
	
	
	
	
}
