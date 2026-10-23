#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "Skill_StepWindWave.generated.h"

UCLASS()
class PIXELBLITZ_API ASkill_StepWindWave : public ABaseSkill
{
	GENERATED_BODY()

public:
	ASkill_StepWindWave();

	virtual bool OnCrossPlatformLanding() override;
};
