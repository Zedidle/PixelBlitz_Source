#pragma once
#include "UObject/Interface.h"
#include "EnemyAI_Interface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UEnemyAI_Interface : public UInterface
{
	GENERATED_BODY()
};

class PIXEL2DKIT_API IEnemyAI_Interface
{
	GENERATED_BODY()
    
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	float GetRandomMoveRange();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAtttckRange_EnemyAI();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void OnReachedAttackLocation_EnemyAI();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool CanMove_EnemyAI();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool Dash_EnemyAI();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void OnReachedEnemyX_EnemyAI();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void OnReachedEnemyY_EnemyAI();

};