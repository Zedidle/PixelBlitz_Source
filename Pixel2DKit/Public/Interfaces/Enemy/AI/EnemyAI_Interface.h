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
	bool InAttackRange();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void OnReachedLocation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool CanMove();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool Dash();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerEastNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerEastMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerEastFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerEastRemote(float Distance);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerWestNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerWestMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerWestFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerWestRemote(float Distance);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerNorthNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerNorthMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerNorthFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerNorthRemote(float Distance);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerSouthNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerSouthMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerSouthFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void ActionAtPlayerSouthRemote(float Distance);



	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_EastNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_EastMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_EastFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_EastRemote(float Distance);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_WestNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_WestMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_WestFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_WestRemote(float Distance);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_NorthNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_NorthMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_NorthFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_NorthRemote(float Distance);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_SouthNear(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_SouthMid(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_SouthFar(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	bool InAttackRange_SouthRemote(float Distance);

	
};