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
    FGameplayTagContainer GetOwnCamp() const; // 获取己方阵营
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    FGameplayTagContainer GetEnemyCamp(); // 获取敌方阵营
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    AActor* GetTarget();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
    bool CanAttack();
    
    UFUNCTION(Category="Fight_Interface")
    virtual bool GetIsAttacking() = 0;

    UFUNCTION(Category="Fight_Interface")
    virtual bool GetIsDefending() = 0;
    
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
    void OnBeAttacked(AActor* Maker, int Damage, int& OutDamage, bool bForce);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    int DamagePlus(int InDamage, AActor* Receiver);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    int OnDefendingHit(int iniDamage);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnDefendingHitEffect();
    
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAttackHolding();

    // 血量为 0 时触发
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnDie();
    
    // 死亡动画播放结束时触发，需手动挂载，配合 ZDAN_OnDieEnd
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAnimDieEnd();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnRemoteAttackEffect();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnRemoteAttackEnd();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAnimVulnerableBegin();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAnimVulnerableEnd();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnDashEffectBegin();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnDashEffectEnd();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnDefendEffectBegin();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnDefendEffectEnd();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAttackEffect();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAttackEffectBegin();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnAttackEffectEnd();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    void OnPickGold();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    bool FindEffectGameplayTag(const FGameplayTag    Tag, float& Result);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    APawn* GetPawn();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    FVector GetAttackRepel();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    int GetAttackDamage();
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    float GetAttackInterval();
};