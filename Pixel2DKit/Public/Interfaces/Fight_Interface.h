#pragma once
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "Fight_Interface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UFight_Interface : public UInterface
{
    GENERATED_BODY()
};

class PIXEL2DKIT_API IFight_Interface
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    FGameplayTagContainer GetOwnCamp(); // 获取己方阵营
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    FGameplayTagContainer GetEnemyCamp(); // 获取敌方阵营
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    AActor* GetTarget();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
    bool CanAttack();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    bool GetIsAttacking();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    bool IsAlive();



    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAttackHiting();  // 发起攻击命中
    
    // 被强效击退的效果，一般都是一样
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void PowerRepulsion(float Power); 

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnBeAttacked_Invulnerable();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    bool OnBeAttacked(AActor* maker, int damage);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    int DamagePlus(int inValue, AActor* ActorDamaged);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    int OnDefendingHit(int iniDamage);
    
};