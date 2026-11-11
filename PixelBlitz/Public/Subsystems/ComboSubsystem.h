// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/PXWorldSubsystem.h"
#include "UI/Player/ComboWeakPointCountWidget.h"
#include "ComboSubsystem.generated.h"


UCLASS()
class PIXELBLITZ_API UComboSubsystem : public UPXWorldSubsystem
{
	GENERATED_BODY()




	
public:
	
	static UComboSubsystem* GetInstance(const UObject* WorldContextObject);
	
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;
	
	UPROPERTY()
	UComboWeakPointCountWidget* ComboWeakPointCountWidget;
	
	/** 弱点连击上限：5 阶 × 每阶 5 连击 = 25；封顶增幅 = 0.17 + 25*0.01 + 4*0.05 = 62% */
	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	int MaxComboWeakPoint = 25;

	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	int CurComboWeakPoint = 0;

	/** 音阶阶段大小：每阶段包含的连击数（对应五声音阶一个八度的 5 个音） */
	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	int ComboWeakPointStageSize = 5;

	/** 阶段内衰减间隔（秒）：每过该时间掉 1 层连击 */
	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	float StageDecayTime = 4.f;

	/** 跨阶跌落间隔（秒）：处于某阶段首个连击时，跌出该阶段需要的时间（波浪节奏的"浪谷"） */
	UPROPERTY(BlueprintReadWrite, Category="ComboSubsystem")
	float StageDropDecayTime = 8.f;

	/** 音阶显化：当前连击所处阶段（从 1 起算，0 = 无连击） */
	UFUNCTION(BlueprintPure, Category="ComboSubsystem")
	int GetComboStage() const;

	/** 音阶显化：最大阶段数 */
	UFUNCTION(BlueprintPure, Category="ComboSubsystem")
	int GetMaxComboStage() const;

	/** 当前适用的衰减间隔（秒）：阶段首连击为跨阶间隔 8s，其余为阶内间隔 4s；无连击返回 0 */
	UFUNCTION(BlueprintPure, Category="ComboSubsystem")
	float GetCurrentDecayTime() const;

	/** 距下一次衰减的进度 0~1（UI 倒计时条用；无连击返回 0） */
	UFUNCTION(BlueprintPure, Category="ComboSubsystem")
	float GetDecayProgress() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ComboSubsystem")
	float GetDamagePlusPercent();

	/** 阶段内每层连击的额外伤害增幅（0.01 = 每层 +1%，连击之魂用）：叠加在阶段内基础 1% 之上；Set 语义，重复调用不叠加 */
	UFUNCTION(BlueprintCallable, Category="ComboSubsystem")
	void SetWithinStageBonusExtra(float ExtraPercent);

	// 当命中弱点时，累计弱点击破连击次数（有副作用：加连击、播音效，每次命中只调用一次）
	UFUNCTION(BlueprintCallable, Category="ComboSubsystem")
	int CalDamageByComboWeakPoint(int InitDamage);

	/** 连音播放工具：按顺序播放 SW_ComboWeakPoints 中 NoteIndices 下标的音频，如 2,5,7；StartDelay 为起始延迟（前置垫音用） */
	UFUNCTION(BlueprintCallable, Category="ComboSubsystem")
	void PlayComboWeakPointRun(const TArray<int32>& NoteIndices, float VolumeMultiplier = 1.0f, float NoteInterval = 0.0f, float StartDelay = 0.0f);

	/** 按当前连击数重排衰减定时器：阶段首连击用跨阶跌落间隔，其余用阶段内间隔。命中后调用可重置衰减计时 */
	UFUNCTION(BlueprintCallable, Category="ComboSubsystem")
	void ScheduleComboDecay();

	/** 额外累积连击（借力打力等天赋用）：加连击并同步刷新 UI / 衰减计时 / BGM 闪避；超过上限截断。返回累积后的连击数 */
	UFUNCTION(BlueprintCallable, Category="ComboSubsystem")
	int AddComboWeakPoint(int Amount = 1);

private:

	/** 封顶乐句轮播游标（配置了 ComboWeakPoint_CapRuns 时使用） */
	int CapRunCursor = 0;

	/** 封顶节奏游标：3 次随机单音 + 1 次五音乱序连音，循环（未配置封顶乐句时使用） */
	int CapCycleCursor = 0;

	/** 封顶溢出加成：封顶后每打满 5 次连击 +1%，无上限；跌落出封顶即清空 */
	float CapOverflowPercent = 0.f;

	/** 阶段内每层连击的额外增幅（连击之魂：Ability.ComboSoul.Set.DamagePlusPercent），叠加在 WithinStageBonus 上 */
	float WithinStageBonusExtra = 0.f;

	/** 最近一次重排衰减定时器的世界时间（进度条用） */
	float LastDecayScheduleTime = -1000.f;

	/** BGM 闪避 SoundMix 当前是否已激活 */
	bool bBgmDuckActive = false;

	/** 连击所处阶段（内部：Combo 从 0 起算，0 返回 0 阶） */
	int GetStageOfCombo(int Combo) const;

	/** 播放音阶转换前置垫音（SW_ComboWeakPoint_Impact），未配置则跳过 */
	void PlayComboWeakPointImpact(float VolumeMultiplier);

	/** BGM 闪避：连击激活时 Push SoundMix 压低 BGM，归零时 Pop 恢复（DA 未配置 SoundMix 则不生效） */
	void UpdateBgmDuck();

	/** 连音默认间隔（DA 配置，下限 0.02s） */
	float GetRunNoteIntervalSafe() const;

	/** 命中音量：随连击递增（ComboBefore = 命中前连击数） */
	float GetComboHitVolume(int ComboBefore) const;

	/** 播放一次弱点连击音阶音效：含封顶节奏、连音触发与低八度叠层（ComboBefore = 命中前连击数） */
	void PlayComboWeakPointHitSound(int ComboBefore, float Volume);

	/** 升阶宣告音效：前置垫音 + 该阶段 1~5 快速上行连音 */
	void PlayComboStageAscendSound(int Stage, float Volume);

	/** 衰减 1 层连击；跨阶跌落时播放该阶段的 5→1 倒连音 */
	void DecayComboStep();

	/** 惰性创建连击 UI（不存在时按 WidgetsDataAsset 创建并加入视口） */
	void EnsureComboWidget();

};
