// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperDeathEffect.generated.h"

class UProceduralMeshComponent;
class UPaperFlipbookComponent;
class UPaperSprite;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UTexture2D;

/** 单块纸片碎片（飞行 / 落地 / 融化）的运行时状态 */
USTRUCT()
struct FPaperShardState
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> MeshComp = nullptr;

	/** 根局部空间速度（纸面朝上视为 +Z，与角色旋转基本一致） */
	FVector Velocity = FVector::ZeroVector;

	/** 每帧自转角速度（Pitch 翻面 / Roll 抖动） */
	FRotator AngularVelocity = FRotator::ZeroRotator;

	/** 该碎片落地的世界 Z */
	float GroundZ = 0.f;

	/** 碎片下方探到地面；为 false 表示飞出平台，只坠落不落地 */
	bool bHasGround = true;

	bool bLanded = false;
	float MeltTime = 0.f;
	bool bMelted = false;
};

/**
 * 纸片死亡表现：把怪物当前帧的纸片沿随机 Voronoi 裂缝撕成若干块，碎片带速度翻滚飞散，
 * 落地后缩没（融化）并在地面摊开成一滩颜料。
 *
 * 用法：怪物死亡瞬间调用 SpawnPaperDeathEffect，传入它的 UPaperFlipbookComponent。
 * 该 Actor 自带根节点，所有碎片与颜料滩都是它下面的 UProceduralMeshComponent。
 */
UCLASS(Blueprintable)
class PIXELBLITZ_API APaperDeathEffect : public AActor
{
	GENERATED_BODY()

public:
	APaperDeathEffect();

	/**
	 * 在纸片角色死亡位置生成撕裂效果。
	 * @param SpriteComp  角色的 Flipbook 组件（取当前帧 Sprite 的贴图与几何）
	 * @param BurstDir    击退/击打方向（世界系，只取 X 分量参与飞散）
	 * @param PaintColor  融化后的颜料颜色
	 * @param EffectClass 可选的子类覆盖
	 * @return 生成成功返回实例，失败返回 nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "FX|PaperDeath", meta = (WorldContext = "WorldContextObject"))
	static APaperDeathEffect* SpawnPaperDeathEffect(UObject* WorldContextObject, UPaperFlipbookComponent* SpriteComp, const FVector& BurstDir, const FLinearColor& PaintColor, TSubclassOf<APaperDeathEffect> EffectClass = nullptr);

	virtual void Tick(float DeltaSeconds) override;

protected:
	/** 碎片数量（Voronoi 区域数） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	int32 ShardCount = 8;

	/** 水平飞散初速 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float BurstSpeed = 220.f;

	/** 向上抛起初速 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float BurstUpSpeed = 300.f;

	/** 重力倍率（按 980cm/s^2 基准） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float GravityScale = 1.4f;

	/** 空气阻力（水平衰减系数） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float AirDrag = 1.5f;

	/** 碎片落地后缩没所需时间 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float MeltDuration = 0.55f;

	/** 融化时向地面下沉速度 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float SinkSpeed = 30.f;

	/** 颜料滩摊开时长 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float PuddleGrowDuration = 0.5f;

	/** 颜料滩保留时长（秒），<= 0 表示永久保留 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float PuddleLifetime = 15.f;

	/** 颜料滩半径相对纸片半宽的比例 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float PuddleRadiusScale = 1.f;

	/** 颜料滩纵向压扁比例（横版视角下形成趴在地上的椭圆） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	float PuddleFlatten = 0.32f;

	/** 碎片材质（默认 M_PaperShard，缺失则回退到纸片默认材质） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	TSoftObjectPtr<UMaterialInterface> ShardMaterial;

	/** 颜料滩材质（默认 M_PaintSplat，缺失则回退到纸片默认材质） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	TSoftObjectPtr<UMaterialInterface> PuddleMaterial;

	/** 0 表示每次随机 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX|PaperDeath")
	int32 RandomSeed = 0;

private:
	bool InitializeFromSprite(UPaperFlipbookComponent* SpriteComp, const FVector& BurstDir, const FLinearColor& InPaintColor);
	void BuildShardMesh(const TArray<FVector2D>& PolyST, const FVector2D& RectCenter, UTexture2D* Texture, UMaterialInterface* ShardParent, const FVector& BurstDir);
	void BuildPuddleMesh(UTexture2D* Texture, UMaterialInterface* PuddleParent);

	FVector2D BilinearQuad(const FVector2D Quad[4], const FVector2D& ST) const;

	/** 从指定世界位置向下探地面，命中返回 true 并给出地面高度 */
	bool TraceGroundZ(const FVector& FromWorld, float& OutGroundZ) const;

	UPROPERTY()
	FLinearColor PaintColor = FLinearColor(0.55f, 0.1f, 0.12f, 1.f);

	UPROPERTY()
	TArray<FPaperShardState> Shards;

	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> PuddleMesh = nullptr;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PuddleMID = nullptr;

	FVector2D QuadPos[4];
	FVector2D QuadUV[4];
	FVector2D QuadMin = FVector2D::ZeroVector;
	FVector2D QuadMax = FVector2D::ZeroVector;

	FRandomStream RandomStream;

	float PuddleGroundZ = 0.f;
	bool bHasPuddleGround = true;
	float PuddleRadius = 60.f;
	float PuddleAge = 0.f;
	float TotalAge = 0.f;
	int32 LandedCount = 0;
	bool bInitialized = false;
};
