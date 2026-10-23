#include "Fight/Skills/Talent/Skill_StepStrike.h"

#include "Character/BasePXCharacter.h"
#include "Character/Components/BuffComponent.h"

ASkill_StepStrike::ASkill_StepStrike()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ASkill_StepStrike::OnCrossPlatformLanding()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	if (!PXCharacter) return false;
	if (!PXCharacter->BuffComponent) return false;

	if (!Super::OnCrossPlatformLanding()) return false;
	
	PXCharacter->BuffComponent->AddBuffByTag(AbilityTag);
	return true;
}

bool ASkill_StepStrike::OnAttackFinish()
{
	return ClearStoredBuff();
}

bool ASkill_StepStrike::OnAttackSkillEnd()
{
	return ClearStoredBuff();
}

bool ASkill_StepStrike::ClearStoredBuff()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	if (!PXCharacter) return false;
	if (!PXCharacter->BuffComponent) return false;
	if (!PXCharacter->BuffComponent->BuffExist(AbilityTag)) return false;

	RemoveSelfBuff();
	return true;
}
