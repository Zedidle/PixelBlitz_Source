#include "Fight/Skills/Talent/Skill_StepWindWave.h"

#include "Character/BasePXCharacter.h"
#include "Character/Components/BuffComponent.h"

ASkill_StepWindWave::ASkill_StepWindWave()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ASkill_StepWindWave::OnCrossPlatformLanding()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	if (!PXCharacter) return false;
	if (!PXCharacter->BuffComponent) return false;

	if (!Super::OnCrossPlatformLanding()) return false;

	PXCharacter->BuffComponent->AddBuffByTag(AbilityTag);
	return true;
}
