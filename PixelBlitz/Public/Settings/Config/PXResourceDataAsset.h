// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PXCustomStruct.h"
#include "PXResourceDataAsset.generated.h"

class USoundMix;

class APlayerRespawnPoint;
class UNiagaraSystem;

UCLASS(BlueprintType)
class PIXELBLITZ_API UPXResourceDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY( EditAnywhere, Category = "Custom | FootStep")
	TMap<EWeather, TSoftObjectPtr<UNiagaraSystem>> WeatherTypeToNiagara;

	UPROPERTY( EditAnywhere, Category = "Custom | FootStep")
	TMap<EWeather, TSoftObjectPtr<USoundBase>> WeatherTypeToSound;

	UPROPERTY( EditAnywhere, Category = "Custom | Audio")
	TSoftObjectPtr<USoundAttenuation> Attenuation_Default;


	UPROPERTY( EditAnywhere, Category = "Ability | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_Mobiliarbus;

	UPROPERTY( EditAnywhere, Category = "Ability | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_Revive;

	UPROPERTY( EditAnywhere, Category = "Ability | SoundCue")
	TSoftObjectPtr<USoundCue> SC_Revive;


	
	UPROPERTY( EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitSmoke;
	UPROPERTY( EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitBlood;
	UPROPERTY( EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitBlood_Little;


	

	
	
	UPROPERTY( EditAnywhere, Category = "GameplayFlow | SoundCue")
	TSoftObjectPtr<USoundCue> SC_OnDie;
	
	UPROPERTY( EditAnywhere, Category = "GameplayFlow | Actor")
	TSubclassOf<APlayerRespawnPoint> PlayerRespawnPointClass;


	UPROPERTY( EditAnywhere, Category = "Combo | Sound")
	TArray<TSoftObjectPtr<USoundBase>> SW_ComboWeakPoints;

	/** 音阶转换前置垫音：升阶/跌阶连音前播放的冲击音（信号枪），留空则不播 */
	UPROPERTY( EditAnywhere, Category = "Combo | Sound")
	TSoftObjectPtr<USoundBase> SW_ComboWeakPoint_Impact;

	/** 连击激活期间的 BGM 闪避 SoundMix：压低 Music 类音量给音阶让位，连击归零自动恢复；留空不生效 */
	UPROPERTY( EditAnywhere, Category = "Combo | Sound")
	TSoftObjectPtr<USoundMix> ComboWeakPoint_SoundMix;

	/** 低八度叠层音量倍率：同一音符降八度再叠一遍以增加力量感，0 = 关闭 */
	UPROPERTY( EditAnywhere, Category = "Combo | Sound", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ComboWeakPoint_SubOctaveVolume = 0.6f;

	/** 连击连音配置：达到 TriggerCombo 连击时按 NoteIndices 下标序列播放连音 */
	UPROPERTY( EditAnywhere, Category = "Combo | Sound")
	TArray<FComboWeakPointRun> ComboWeakPoint_Runs;

	/** 封顶乐句：达到最大连击后每次命中轮播一条；留空则退化为最高阶段内随机取音 */
	UPROPERTY( EditAnywhere, Category = "Combo | Sound")
	TArray<FComboWeakPointRun> ComboWeakPoint_CapRuns;

	/** 连音默认间隔（秒）：Run 未单独配置 NoteInterval 时使用 */
	UPROPERTY( EditAnywhere, Category = "Combo | Sound", meta = (ClampMin = "0.02", ClampMax = "0.3"))
	float ComboWeakPoint_RunNoteInterval = 0.07f;



	UPROPERTY( EditAnywhere, Category = "Buff | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_SpeedUP;
	UPROPERTY( EditAnywhere, Category = "Buff | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_SpeedDown;
};
