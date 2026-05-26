#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "Skill_StepStrike.generated.h"

UCLASS()
class PIXEL2DKIT_API ASkill_StepStrike : public ABaseSkill
{
	GENERATED_BODY()

public:
	ASkill_StepStrike();

	virtual bool OnCrossPlatformLanding() override;
	virtual bool OnAttackFinish() override;
	virtual bool OnAttackSkillEnd() override;

protected:
	
	bool ClearStoredBuff();
};
