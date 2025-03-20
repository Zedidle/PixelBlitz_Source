#pragma once
#include "UObject/Interface.h"
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
    AActor* GetTarget();
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    bool GetIsAttacking();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
    bool IsAlive();
    
};