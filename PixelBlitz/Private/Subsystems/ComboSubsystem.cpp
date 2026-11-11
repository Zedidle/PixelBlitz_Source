// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ComboSubsystem.h"

#include "PixelBlitz.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Settings/Config/PXWidgetsDataAsset.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/SoundFuncLib.h"
#include "Utilitys/UserWidgetFuncLib.h"

// 波浪式增幅常量
constexpr float ComboBasePercent = 0.08f;    // 基础增幅
constexpr float WithinStageBonus = 0.01f;    // 阶段内每次连击 +1%
constexpr float StageUpBonus = 0.05f;        // 升阶一次性 +5%
constexpr float CapOverflowStep = 0.01f;     // 封顶溢出：每打满一轮 +1%，无上限
constexpr int CapSinglesPerCycle = 3;        // 封顶节奏：每轮 3 次随机单音 + 1 次五音乱序连音

namespace
{
	const FName ComboDecayTimerName = "UComboSubsystem::ComboDecay";
}

int UComboSubsystem::GetStageOfCombo(int Combo) const
{
	const int StageSize = FMath::Max(ComboWeakPointStageSize, 1);
	return Combo <= 0 ? 0 : (Combo - 1) / StageSize + 1;
}

int UComboSubsystem::GetComboStage() const
{
	return GetStageOfCombo(CurComboWeakPoint);
}

int UComboSubsystem::GetMaxComboStage() const
{
	const int StageSize = FMath::Max(ComboWeakPointStageSize, 1);
	return (MaxComboWeakPoint + StageSize - 1) / StageSize;
}

float UComboSubsystem::GetCurrentDecayTime() const
{
	if (CurComboWeakPoint <= 0) return 0.f;
	const int StageSize = FMath::Max(ComboWeakPointStageSize, 1);
	const bool bAtStageStart = CurComboWeakPoint > 1 && (CurComboWeakPoint - 1) % StageSize == 0;
	return bAtStageStart ? StageDropDecayTime : StageDecayTime;
}

float UComboSubsystem::GetDecayProgress() const
{
	if (CurComboWeakPoint <= 0 || !GetWorld()) return 0.f;
	const float Interval = GetCurrentDecayTime();
	if (Interval <= 0.f) return 0.f;
	const float Elapsed = GetWorld()->GetTimeSeconds() - LastDecayScheduleTime;
	return FMath::Clamp(Elapsed / Interval, 0.f, 1.f);
}

float UComboSubsystem::GetDamagePlusPercent()
{
	if (CurComboWeakPoint <= 0)
	{
		return 0;
	}
	const int Stage = GetStageOfCombo(CurComboWeakPoint);
	// 阶段内每层连击增幅 = 基础 1% + 天赋额外（连击之魂 +1%，即每层 2%）
	const float WithinStageTotal = WithinStageBonus + WithinStageBonusExtra;
	return ComboBasePercent + WithinStageTotal * CurComboWeakPoint + StageUpBonus * (Stage - 1) + CapOverflowPercent;
}

void UComboSubsystem::SetWithinStageBonusExtra(float ExtraPercent)
{
	WithinStageBonusExtra = FMath::Max(ExtraPercent, 0.f);

	// 增幅变化立即传导到 UI（连击未激活时无显示，无需刷新）
	if (CurComboWeakPoint > 0 && IsValid(ComboWeakPointCountWidget))
	{
		ComboWeakPointCountWidget->UpdateCount(CurComboWeakPoint, GetDamagePlusPercent() * 100);
	}
}

UComboSubsystem* UComboSubsystem::GetInstance(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;
	
	return World->GetSubsystem<UComboSubsystem>();
}

void UComboSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 预加载连击音阶与打击层：总量 <1MB，避免战斗中首次播放的同步加载卡顿
	if (const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>())
	{
		if (const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous())
		{
			for (const TSoftObjectPtr<USoundBase>& SoundPtr : ResourceDataAsset->SW_ComboWeakPoints)
			{
				SoundPtr.LoadSynchronous();
			}
			ResourceDataAsset->SW_ComboWeakPoint_Impact.LoadSynchronous();
		}
	}
}

void UComboSubsystem::Deinitialize()
{
	Super::Deinitialize();
	if (ComboWeakPointCountWidget)
	{
		ComboWeakPointCountWidget->RemoveFromRoot();
	}
}

int UComboSubsystem::CalDamageByComboWeakPoint(int InitDamage)
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Settings, InitDamage)

	// 命中音阶音效（含封顶节奏与连音触发）：以本次命中前的连击数为准
	const float Volume = GetComboHitVolume(CurComboWeakPoint);
	PlayComboWeakPointHitSound(CurComboWeakPoint, Volume);

	EnsureComboWidget();
	const int PrevCombo = CurComboWeakPoint;
	CurComboWeakPoint = FMath::Min(CurComboWeakPoint + 1, MaxComboWeakPoint);

	// 升阶宣告：从 N 阶升入 N+1 阶时，前置垫音 + 新阶段的 1~5 快速上行连音
	if (GetStageOfCombo(CurComboWeakPoint) > GetStageOfCombo(PrevCombo) && CurComboWeakPoint > 1)
	{
		PlayComboStageAscendSound(GetStageOfCombo(CurComboWeakPoint), Volume);
	}

	if (IsValid(ComboWeakPointCountWidget))
	{
		ComboWeakPointCountWidget->UpdateCount(CurComboWeakPoint, GetDamagePlusPercent() * 100);
	}

	// 波浪式衰减：每次命中后重置衰减计时
	ScheduleComboDecay();
	UpdateBgmDuck();	// 连击激活：开启 BGM 闪避（幂等，激活期间只 Push 一次）

	return InitDamage * (1 + GetDamagePlusPercent());
}

void UComboSubsystem::ScheduleComboDecay()
{
	if (CurComboWeakPoint <= 0) return;

	if (const UWorld* World = GetWorld())
	{
		LastDecayScheduleTime = World->GetTimeSeconds();
	}

	// 处于某阶段首个连击时，跌出该阶段需要双倍时间（8s），其余 4s 掉 1 层
	const int StageSize = FMath::Max(ComboWeakPointStageSize, 1);
	const bool bAtStageStart = CurComboWeakPoint > 1 && (CurComboWeakPoint - 1) % StageSize == 0;
	const float Delay = bAtStageStart ? StageDropDecayTime : StageDecayTime;

	UTimerManagerFuncLib::SetRetriggerableDelay(this, ComboDecayTimerName,
		[WeakThis = TWeakObjectPtr(this)]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->DecayComboStep();
			}
		}, Delay);
}

int UComboSubsystem::AddComboWeakPoint(int Amount)
{
	if (Amount <= 0 || CurComboWeakPoint >= MaxComboWeakPoint) return CurComboWeakPoint;

	const int PrevCombo = CurComboWeakPoint;
	CurComboWeakPoint = FMath::Min(CurComboWeakPoint + Amount, MaxComboWeakPoint);

	// 每一层新增连击都按弱点命中处理：播放对应音阶单音（封顶时走封顶节奏）
	for (int Combo = PrevCombo; Combo < CurComboWeakPoint; ++Combo)
	{
		PlayComboWeakPointHitSound(Combo, GetComboHitVolume(Combo));
	}

	EnsureComboWidget();

	// 升阶宣告：额外连击跨过阶段边界时，同样播放前置垫音 + 新阶段上行连音
	// （PrevCombo > 0：从 0 起连的第一击只是"激活"，不算升阶，不播宣告连音）
	if (PrevCombo > 0 && GetStageOfCombo(CurComboWeakPoint) > GetStageOfCombo(PrevCombo))
	{
		PlayComboStageAscendSound(GetStageOfCombo(CurComboWeakPoint), GetComboHitVolume(PrevCombo));
	}

	if (IsValid(ComboWeakPointCountWidget))
	{
		ComboWeakPointCountWidget->UpdateCount(CurComboWeakPoint, GetDamagePlusPercent() * 100);
	}

	// 额外连击同样视为一次命中：重排衰减计时并保持 BGM 闪避
	ScheduleComboDecay();
	UpdateBgmDuck();

	return CurComboWeakPoint;
}

void UComboSubsystem::EnsureComboWidget()
{
	if (ComboWeakPointCountWidget) return;

	if (const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>())
	{
		if (const UPXWidgetsDataAsset* WidgetsDataAsset = Settings->WidgetsDataAsset.LoadSynchronous())
		{
			if (WidgetsDataAsset->ComboWeakPointCountWidget)
			{
				ComboWeakPointCountWidget = CreateWidget<UComboWeakPointCountWidget>(GetWorld(), WidgetsDataAsset->ComboWeakPointCountWidget);
				ComboWeakPointCountWidget->AddToViewport(100);
			}
		}
	}
}

void UComboSubsystem::DecayComboStep()
{
	const int PrevCombo = CurComboWeakPoint;
	if (PrevCombo <= 0) return;

	CurComboWeakPoint = PrevCombo - 1;

	// 跌落出封顶：溢出加成与封顶节奏一并清空
	if (CurComboWeakPoint < MaxComboWeakPoint)
	{
		CapOverflowPercent = 0.f;
		CapCycleCursor = 0;
	}

	if (IsValid(ComboWeakPointCountWidget))
	{
		if (CurComboWeakPoint == 0)
		{
			ComboWeakPointCountWidget->Fade(0.2);
		}
		else
		{
			ComboWeakPointCountWidget->UpdateCount(CurComboWeakPoint, GetDamagePlusPercent() * 100);
		}
	}

	if (CurComboWeakPoint == 0)
	{
		UpdateBgmDuck();	// 连击归零：解除 BGM 闪避
		return;
	}

	// 跌阶宣告：跌出某阶段时，前置垫音 + 该阶段的 5~1 快速倒连音
	if (GetStageOfCombo(CurComboWeakPoint) < GetStageOfCombo(PrevCombo))
	{
		TArray<int32> DescendRun;
		const int StageStart = (GetStageOfCombo(PrevCombo) - 1) * FMath::Max(ComboWeakPointStageSize, 1);
		for (int i = ComboWeakPointStageSize - 1; i >= 0; --i)
		{
			DescendRun.Add(StageStart + i);
		}
		PlayComboWeakPointImpact(0.8f);
		PlayComboWeakPointRun(DescendRun, 0.8f, 0.f, GetRunNoteIntervalSafe());
	}

	ScheduleComboDecay();
}

float UComboSubsystem::GetComboHitVolume(int ComboBefore) const
{
	// 音量随连击递增：越连越响
	constexpr float BaseVolume = 0.85f;
	constexpr float VolumeBoost = 0.35f;
	const float ComboRatio = MaxComboWeakPoint > 0
		? static_cast<float>(ComboBefore) / static_cast<float>(MaxComboWeakPoint)
		: 0.f;
	return BaseVolume + VolumeBoost * ComboRatio;
}

void UComboSubsystem::PlayComboStageAscendSound(int Stage, float Volume)
{
	if (Stage <= 0) return;

	TArray<int32> AscendRun;
	const int StageStart = (Stage - 1) * FMath::Max(ComboWeakPointStageSize, 1);
	for (int i = 0; i < ComboWeakPointStageSize; ++i)
	{
		AscendRun.Add(StageStart + i);
	}
	PlayComboWeakPointImpact(Volume);
	PlayComboWeakPointRun(AscendRun, Volume, 0.f, GetRunNoteIntervalSafe());
}

void UComboSubsystem::PlayComboWeakPointHitSound(int ComboBefore, float Volume)
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)

	const int StageSize = FMath::Max(ComboWeakPointStageSize, 1);
	const bool bAtCap = MaxComboWeakPoint > 0 && ComboBefore >= MaxComboWeakPoint;

	// 封顶乐句轮播：达到最大连击后，每次命中轮播一条封顶乐句
	if (bAtCap && ResourceDataAsset->ComboWeakPoint_CapRuns.Num() > 0)
	{
		const int RunIndex = CapRunCursor % ResourceDataAsset->ComboWeakPoint_CapRuns.Num();
		++CapRunCursor;
		if (const FComboWeakPointRun* Run = &ResourceDataAsset->ComboWeakPoint_CapRuns[RunIndex]; Run->NoteIndices.Num() > 0)
		{
			PlayComboWeakPointRun(Run->NoteIndices, Volume * Run->Volume, Run->NoteInterval);
		}
		return;
	}

	if (ResourceDataAsset->SW_ComboWeakPoints.Num() <= 0) return;

	// 封顶节奏（未配置封顶乐句时）：3 次随机单音 + 第 4 次五音乱序连音，循环往复
	bool bCapRunHit = false;
	TArray<int32> CapRunNotes;

	int NoteIndex = INDEX_NONE;
	if (bAtCap)
	{
		const int StageStart = (GetStageOfCombo(MaxComboWeakPoint) - 1) * StageSize;
		const int TopNoteCount = FMath::Max(FMath::Min(StageSize, ResourceDataAsset->SW_ComboWeakPoints.Num() - StageStart), 1);

		if (CapCycleCursor < CapSinglesPerCycle)
		{
			++CapCycleCursor;
			NoteIndex = StageStart + FMath::RandRange(0, TopNoteCount - 1);
		}
		else
		{
			CapCycleCursor = 0;
			bCapRunHit = true;
			// 溢出积累：封顶状态每打满一轮（连音触发时）+1%，无上限
			CapOverflowPercent += CapOverflowStep;
			for (int i = 0; i < TopNoteCount; ++i)
			{
				CapRunNotes.Add(StageStart + i);
			}
			// Fisher-Yates 洗牌：五音乱序
			for (int i = CapRunNotes.Num() - 1; i > 0; --i)
			{
				const int j = FMath::RandRange(0, i);
				Swap(CapRunNotes[i], CapRunNotes[j]);
			}
		}
	}
	else
	{
		NoteIndex = ResourceDataAsset->SW_ComboWeakPoints.IsValidIndex(ComboBefore)
			? ComboBefore
			: ResourceDataAsset->SW_ComboWeakPoints.Num() - 1;
	}

	if (bCapRunHit)
	{
		PlayComboWeakPointRun(CapRunNotes, Volume, 0.f);
		return;
	}

	if (NoteIndex == INDEX_NONE) return;

	USoundBase* NoteSound = ResourceDataAsset->SW_ComboWeakPoints[NoteIndex].LoadSynchronous();
	if (!NoteSound) return;

	USoundFuncLib::PlaySound2D(NoteSound, Volume);

	// 低八度叠层：加厚力量感（ComboWeakPoint_SubOctaveVolume = 0 时关闭）
	const float SubOctaveVolume = ResourceDataAsset->ComboWeakPoint_SubOctaveVolume;
	if (SubOctaveVolume > 0.f)
	{
		USoundFuncLib::PlaySound2D(NoteSound, Volume * SubOctaveVolume, 0.5f);
	}

	// 连击连音：达到配置的连击数时，按 NoteIndices 下标序列播放连音
	const int ComboCount = ComboBefore + 1;
	for (const FComboWeakPointRun& Run : ResourceDataAsset->ComboWeakPoint_Runs)
	{
		if (Run.TriggerCombo == ComboCount)
		{
			PlayComboWeakPointRun(Run.NoteIndices, Volume * Run.Volume, Run.NoteInterval);
		}
	}
}

void UComboSubsystem::PlayComboWeakPointRun(const TArray<int32>& NoteIndices, float VolumeMultiplier, float NoteInterval, float StartDelay)
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)

	if (NoteIndices.Num() == 0) return;

	// 未单独配置间隔时，使用 DA 的默认间隔；下限 0.02s
	const float Interval = FMath::Max(
		NoteInterval > 0.f ? NoteInterval : ResourceDataAsset->ComboWeakPoint_RunNoteInterval,
		0.02f);

	for (int i = 0; i < NoteIndices.Num(); ++i)
	{
		const int32 Index = NoteIndices[i];
		if (!ResourceDataAsset->SW_ComboWeakPoints.IsValidIndex(Index)) continue;

		const TSoftObjectPtr<USoundBase> SoundPtr = ResourceDataAsset->SW_ComboWeakPoints[Index];
		UTimerManagerFuncLib::SetDelaySafe(this, this, [SoundPtr, VolumeMultiplier]()
		{
			if (USoundBase* Sound = SoundPtr.LoadSynchronous())
			{
				USoundFuncLib::PlaySound2D(Sound, VolumeMultiplier);
			}
		}, StartDelay + Interval * i);
	}
}

void UComboSubsystem::PlayComboWeakPointImpact(float VolumeMultiplier)
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)

	if (TSoftObjectPtr<USoundBase> ImpactPtr = ResourceDataAsset->SW_ComboWeakPoint_Impact; ImpactPtr.IsValid())
	{
		if (USoundBase* ImpactSound = ImpactPtr.LoadSynchronous())
		{
			USoundFuncLib::PlaySound2D(ImpactSound, VolumeMultiplier);
		}
	}
}

float UComboSubsystem::GetRunNoteIntervalSafe() const
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	if (!Settings) return 0.07f;

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	return ResourceDataAsset ? FMath::Max(ResourceDataAsset->ComboWeakPoint_RunNoteInterval, 0.02f) : 0.07f;
}

void UComboSubsystem::UpdateBgmDuck()
{
	if (GetDefault<UPXCustomSettings>() == nullptr) return;
	const UPXResourceDataAsset* ResourceDataAsset = GetDefault<UPXCustomSettings>()->ResourceDataAsset.LoadSynchronous();
	if (ResourceDataAsset == nullptr || !ResourceDataAsset->ComboWeakPoint_SoundMix.IsValid()) return;

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	USoundMix* SoundMix = ResourceDataAsset->ComboWeakPoint_SoundMix.LoadSynchronous();
	if (SoundMix == nullptr) return;

	if (CurComboWeakPoint > 0 && !bBgmDuckActive)
	{
		bBgmDuckActive = true;
		UGameplayStatics::PushSoundMixModifier(World, SoundMix);
	}
	else if (CurComboWeakPoint <= 0 && bBgmDuckActive)
	{
		bBgmDuckActive = false;
		UGameplayStatics::PopSoundMixModifier(World, SoundMix);
	}
}
