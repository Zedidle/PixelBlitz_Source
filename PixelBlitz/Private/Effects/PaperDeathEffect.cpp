// Fill out your copyright notice in the Description page of Project Settings.

#include "Effects/PaperDeathEffect.h"

#include "ProceduralMeshComponent.h"
#include "CollisionQueryParams.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
	/** 用半平面 dot(P - A, B - A) <= 0 裁剪多边形（Sutherland-Hodgman） */
	static TArray<FVector2D> ClipPolyByHalfPlane(const TArray<FVector2D>& Poly, const FVector2D& A, const FVector2D& B)
	{
		TArray<FVector2D> Out;
		if (Poly.Num() < 3)
		{
			return Out;
		}

		const FVector2D Dir = B - A;
		if (Dir.IsNearlyZero())
		{
			return Poly;
		}

		Out.Reserve(Poly.Num() + 2);
		for (int32 Index = 0; Index < Poly.Num(); ++Index)
		{
			const FVector2D& Cur = Poly[Index];
			const FVector2D& Next = Poly[(Index + 1) % Poly.Num()];

			const double CurDot = FVector2D::DotProduct(Cur - A, Dir);
			const double NextDot = FVector2D::DotProduct(Next - A, Dir);
			const bool bCurInside = CurDot <= 0.0;
			const bool bNextInside = NextDot <= 0.0;

			if (bCurInside)
			{
				Out.Add(Cur);
			}

			if (bCurInside != bNextInside)
			{
				const double Denom = NextDot - CurDot;
				if (!FMath::IsNearlyZero(Denom))
				{
					const double T = FMath::Clamp(-CurDot / Denom, 0.0, 1.0);
					Out.Add(Cur + (Next - Cur) * T);
				}
			}
		}

		return Out;
	}
}

APaperDeathEffect::APaperDeathEffect()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Movable);
	RootComponent = Root;

	ShardMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Assets/Materials/M_PaperShard.M_PaperShard")));
	PuddleMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Assets/Materials/M_PaintSplat.M_PaintSplat")));
}

APaperDeathEffect* APaperDeathEffect::SpawnPaperDeathEffect(UObject* WorldContextObject, UPaperFlipbookComponent* SpriteComp, const FVector& BurstDir, const FLinearColor& PaintColor, TSubclassOf<APaperDeathEffect> EffectClass)
{
	if (!SpriteComp)
	{
		return nullptr;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World)
	{
		return nullptr;
	}

	if (!EffectClass)
	{
		EffectClass = APaperDeathEffect::StaticClass();
	}

	const FTransform SpawnTransform = SpriteComp->GetComponentTransform();

	APaperDeathEffect* FX = World->SpawnActorDeferred<APaperDeathEffect>(EffectClass, SpawnTransform, SpriteComp->GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!FX)
	{
		return nullptr;
	}

	if (!FX->InitializeFromSprite(SpriteComp, BurstDir, PaintColor))
	{
		FX->Destroy();
		return nullptr;
	}

	FX->FinishSpawning(SpawnTransform);
	return FX;
}

FVector2D APaperDeathEffect::BilinearQuad(const FVector2D Quad[4], const FVector2D& ST) const
{
	const double S = ST.X;
	const double T = ST.Y;
	return Quad[0] * ((1.f - S) * (1.f - T))
		+ Quad[1] * (S * (1.f - T))
		+ Quad[2] * (S * T)
		+ Quad[3] * ((1.f - S) * T);
}

bool APaperDeathEffect::TraceGroundZ(const FVector& FromWorld, float& OutGroundZ) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FCollisionQueryParams QueryParams(FName(TEXT("PaperDeathGround")), false, this);
	if (AActor* OwnerActor = GetOwner())
	{
		QueryParams.AddIgnoredActor(OwnerActor);
	}

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FHitResult Hit;
	const FVector Start = FromWorld + FVector(0.f, 0.f, 80.f);
	const FVector End = FromWorld - FVector(0.f, 0.f, 2000.f);
	if (World->LineTraceSingleByObjectType(Hit, Start, End, ObjectParams, QueryParams))
	{
		OutGroundZ = (float)Hit.ImpactPoint.Z;
		return true;
	}

	return false;
}

bool APaperDeathEffect::InitializeFromSprite(UPaperFlipbookComponent* SpriteComp, const FVector& BurstDir, const FLinearColor& InPaintColor)
{
	// GetSpriteAtCachedIndex 在引擎内是 protected，改用公开 API：当前播放时间 -> Flipbook 取 Sprite
	UPaperFlipbook* Flipbook = SpriteComp->GetFlipbook();
	if (!Flipbook)
	{
		return false;
	}

	UPaperSprite* Sprite = Flipbook->GetSpriteAtTime(SpriteComp->GetPlaybackPosition(), true);
	if (!Sprite || Sprite->BakedRenderData.Num() < 3)
	{
		return false;
	}

	UTexture2D* Texture = Sprite->GetBakedTexture();
	if (!Texture)
	{
		return false;
	}

	PaintColor = InPaintColor;
	RandomStream.Initialize(RandomSeed != 0 ? RandomSeed : FMath::Rand());
	ShardCount = FMath::Clamp(ShardCount, 2, 24);

	// 1. 取出纸片矩形（枢轴空间，单位 cm）与四个角在图集里的 UV
	const TArray<FVector4>& Baked = Sprite->BakedRenderData;
	FVector2D MinPos(FLT_MAX, FLT_MAX);
	FVector2D MaxPos(-FLT_MAX, -FLT_MAX);
	for (const FVector4& Vert : Baked)
	{
		MinPos.X = FMath::Min(MinPos.X, Vert.X);
		MinPos.Y = FMath::Min(MinPos.Y, Vert.Y);
		MaxPos.X = FMath::Max(MaxPos.X, Vert.X);
		MaxPos.Y = FMath::Max(MaxPos.Y, Vert.Y);
	}
	QuadMin = MinPos;
	QuadMax = MaxPos;

	const FVector2D Size = MaxPos - MinPos;
	if (Size.X < KINDA_SMALL_NUMBER || Size.Y < KINDA_SMALL_NUMBER)
	{
		return false;
	}

	const double Tolerance = FMath::Max(Size.X, Size.Y) * 0.02;
	auto FindCornerUV = [&Baked, Tolerance](const FVector2D& CornerPos) -> FVector2D
	{
		for (const FVector4& Vert : Baked)
		{
			if (FMath::Abs(Vert.X - CornerPos.X) <= Tolerance && FMath::Abs(Vert.Y - CornerPos.Y) <= Tolerance)
			{
				return FVector2D(Vert.Z, Vert.W);
			}
		}
		return FVector2D::ZeroVector;
	};

	QuadPos[0] = FVector2D(MinPos.X, MinPos.Y); // 左下
	QuadPos[1] = FVector2D(MaxPos.X, MinPos.Y); // 右下
	QuadPos[2] = FVector2D(MaxPos.X, MaxPos.Y); // 右上
	QuadPos[3] = FVector2D(MinPos.X, MaxPos.Y); // 左上
	for (int32 Corner = 0; Corner < 4; ++Corner)
	{
		QuadUV[Corner] = FindCornerUV(QuadPos[Corner]);
	}

	// 2. 地面高度：颜料滩落点
	bHasPuddleGround = TraceGroundZ(GetActorLocation(), PuddleGroundZ);
	if (!bHasPuddleGround)
	{
		PuddleGroundZ = GetActorLocation().Z - 30.f;
	}

	// 3. 材质：优先用专用材质，缺失时回退到纸片自身默认材质（顶点色仍可着色）
	UMaterialInterface* ShardParent = ShardMaterial.LoadSynchronous();
	if (!ShardParent)
	{
		ShardParent = Sprite->GetDefaultMaterial();
	}
	UMaterialInterface* PuddleParent = PuddleMaterial.LoadSynchronous();
	if (!PuddleParent)
	{
		PuddleParent = ShardParent;
	}
	if (!ShardParent)
	{
		return false;
	}

	// 4. Voronoi 切分：每块碎片 = 自身种子与其它种子中垂线围出的区域
	TArray<FVector2D> Seeds;
	Seeds.Reserve(ShardCount);
	for (int32 Index = 0; Index < ShardCount; ++Index)
	{
		Seeds.Emplace(RandomStream.FRandRange(0.06f, 0.94f), RandomStream.FRandRange(0.06f, 0.94f));
	}

	static const FVector2D SquareCorners[4] = {
		FVector2D(0.f, 0.f), FVector2D(1.f, 0.f), FVector2D(1.f, 1.f), FVector2D(0.f, 1.f)
	};

	const FVector2D RectCenter = (QuadMin + QuadMax) * 0.5f;
	Shards.Reset();

	for (int32 Index = 0; Index < Seeds.Num(); ++Index)
	{
		TArray<FVector2D> Poly;
		Poly.Add(SquareCorners[0]);
		Poly.Add(SquareCorners[1]);
		Poly.Add(SquareCorners[2]);
		Poly.Add(SquareCorners[3]);

		for (int32 Other = 0; Other < Seeds.Num() && Poly.Num() >= 3; ++Other)
		{
			if (Other == Index)
			{
				continue;
			}
			Poly = ClipPolyByHalfPlane(Poly, Seeds[Index], Seeds[Other]);
		}

		if (Poly.Num() >= 3)
		{
			BuildShardMesh(Poly, RectCenter, Texture, ShardParent, BurstDir);
		}
	}

	// 5. 颜料滩
	BuildPuddleMesh(Texture, PuddleParent);

	if (Shards.Num() == 0)
	{
		return false;
	}

	bInitialized = true;
	return true;
}

void APaperDeathEffect::BuildShardMesh(const TArray<FVector2D>& PolyST, const FVector2D& RectCenter, UTexture2D* Texture, UMaterialInterface* ShardParent, const FVector& BurstDir)
{
	TArray<FVector> Verts;
	TArray<FVector2D> UVs;
	Verts.Reserve(PolyST.Num());
	UVs.Reserve(PolyST.Num());

	for (const FVector2D& ST : PolyST)
	{
		const FVector2D Pos = BilinearQuad(QuadPos, ST);
		Verts.Add(FVector(Pos.X, 0.f, Pos.Y));
		UVs.Add(BilinearQuad(QuadUV, ST));
	}

	if (Verts.Num() < 3)
	{
		return;
	}

	// 以质心为本地原点，方便每块独立位移与旋转
	FVector Centroid = FVector::ZeroVector;
	for (const FVector& Vert : Verts)
	{
		Centroid += Vert;
	}
	Centroid /= (float)Verts.Num();

	for (FVector& Vert : Verts)
	{
		Vert -= Centroid;
	}

	TArray<int32> Tris;
	Tris.Reserve((Verts.Num() - 2) * 3);
	for (int32 Index = 1; Index + 1 < Verts.Num(); ++Index)
	{
		Tris.Add(0);
		Tris.Add(Index);
		Tris.Add(Index + 1);
	}

	TArray<FVector> Normals;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;
	Normals.Init(FVector(0.f, -1.f, 0.f), Verts.Num());
	Colors.Init(FLinearColor::White, Verts.Num());
	Tangents.Init(FProcMeshTangent(FVector(1.f, 0.f, 0.f), false), Verts.Num());

	UProceduralMeshComponent* MeshComp = NewObject<UProceduralMeshComponent>(this);
	MeshComp->SetupAttachment(GetRootComponent());
	MeshComp->SetRelativeLocation(FVector(Centroid.X, 0.f, Centroid.Y));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCastShadow(false);
	MeshComp->SetMobility(EComponentMobility::Movable);
	MeshComp->RegisterComponent();
	MeshComp->CreateMeshSection_LinearColor(0, Verts, Tris, Normals, UVs, Colors, Tangents, false);

	UMaterialInstanceDynamic* ShardMID = UMaterialInstanceDynamic::Create(ShardParent, this);
	if (ShardMID)
	{
		ShardMID->SetTextureParameterValue(TEXT("PaperTex"), Texture);
		ShardMID->SetTextureParameterValue(TEXT("SpriteTexture"), Texture);
		ShardMID->SetScalarParameterValue(TEXT("Fade"), 1.f);
		ShardMID->SetVectorParameterValue(TEXT("PaintTint"), FLinearColor::White);
		MeshComp->SetMaterial(0, ShardMID);
	}

	// 初始飞散：从纸片中心向外炸开 + 击打方向 + 上抛，纸片带翻滚
	FPaperShardState State;
	State.MeshComp = MeshComp;

	// 每块碎片单独探地：飞出平台的碎片不落地，直接坠落消失
	const FVector ShardWorld = GetTransform().TransformPosition(FVector(Centroid.X, 0.f, Centroid.Y));
	State.bHasGround = TraceGroundZ(ShardWorld, State.GroundZ);

	FVector RadialDir(Centroid.X - RectCenter.X, 0.f, Centroid.Y - RectCenter.Y);
	if (RadialDir.SizeSquared() < 1.f)
	{
		const float RandAngle = RandomStream.FRandRange(0.f, 2.f * PI);
		RadialDir = FVector(FMath::Cos(RandAngle), 0.f, FMath::Sin(RandAngle));
	}
	RadialDir = RadialDir.GetSafeNormal();

	const float SpeedScale = RandomStream.FRandRange(0.65f, 1.35f);
	State.Velocity = RadialDir * BurstSpeed * SpeedScale;
	State.Velocity.X += FMath::Clamp((float)BurstDir.X, -1.f, 1.f) * BurstSpeed * 0.6f;
	State.Velocity.Z += BurstUpSpeed * RandomStream.FRandRange(0.7f, 1.3f);

	State.AngularVelocity = FRotator(
		RandomStream.FRandRange(-160.f, 160.f),
		RandomStream.FRandRange(-80.f, 80.f),
		RandomStream.FRandRange(-140.f, 140.f));

	Shards.Add(State);
}

void APaperDeathEffect::BuildPuddleMesh(UTexture2D* Texture, UMaterialInterface* PuddleParent)
{
	const int32 Segments = 28;
	PuddleRadius = FMath::Max(12.f, (float)((QuadMax.X - QuadMin.X) * 0.5 * PuddleRadiusScale));
	const float FlatRadius = PuddleRadius * PuddleFlatten;

	TArray<FVector> Verts;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<int32> Tris;

	// 中心点 + 一圈椭圆（XZ 平面内，纵向压扁 → 横版视角下像趴在地上的颜料）
	Verts.Add(FVector::ZeroVector);
	UVs.Add(FVector2D(0.5f, 0.5f));
	Colors.Add(PaintColor);

	for (int32 Index = 0; Index <= Segments; ++Index)
	{
		const float Angle = (float)Index / (float)Segments * 2.f * PI;
		const float Cos = FMath::Cos(Angle);
		const float Sin = FMath::Sin(Angle);
		Verts.Add(FVector(Cos * PuddleRadius, 0.f, Sin * FlatRadius));
		UVs.Add(FVector2D(0.5f + 0.5f * Cos, 0.5f + 0.5f * Sin));
		Colors.Add(PaintColor);
	}

	for (int32 Index = 1; Index <= Segments; ++Index)
	{
		Tris.Add(0);
		Tris.Add(Index + 1);
		Tris.Add(Index);
	}

	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	Normals.Init(FVector(0.f, -1.f, 0.f), Verts.Num());
	Tangents.Init(FProcMeshTangent(FVector(1.f, 0.f, 0.f), false), Verts.Num());

	PuddleMesh = NewObject<UProceduralMeshComponent>(this);
	PuddleMesh->SetupAttachment(GetRootComponent());
	PuddleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PuddleMesh->SetCastShadow(false);
	PuddleMesh->SetMobility(EComponentMobility::Movable);

	const FVector RelGround = GetTransform().InverseTransformPosition(FVector(GetActorLocation().X, GetActorLocation().Y, PuddleGroundZ));
	PuddleMesh->SetRelativeLocation(FVector(0.f, 0.f, RelGround.Z));
	PuddleMesh->RegisterComponent();
	PuddleMesh->CreateMeshSection_LinearColor(0, Verts, Tris, Normals, UVs, Colors, Tangents, false);

	PuddleMID = UMaterialInstanceDynamic::Create(PuddleParent, this);
	if (PuddleMID)
	{
		PuddleMID->SetTextureParameterValue(TEXT("PaperTex"), Texture);
		PuddleMID->SetTextureParameterValue(TEXT("SpriteTexture"), Texture);
		PuddleMID->SetVectorParameterValue(TEXT("PaintColor"), PaintColor);
		PuddleMID->SetScalarParameterValue(TEXT("Progress"), 0.f);
		PuddleMID->SetScalarParameterValue(TEXT("Fade"), 1.f);
		PuddleMesh->SetMaterial(0, PuddleMID);
	}

	PuddleMesh->SetVisibility(false);
}

void APaperDeathEffect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bInitialized)
	{
		return;
	}

	TotalAge += DeltaSeconds;
	PuddleAge += DeltaSeconds;

	bool bAllMelted = true;

	for (FPaperShardState& Shard : Shards)
	{
		if (Shard.bMelted || !Shard.MeshComp)
		{
			continue;
		}
		bAllMelted = false;

		if (!Shard.bLanded)
		{
			// 自由飞行：重力 + 水平阻力 + 翻滚
			Shard.Velocity.Z -= 980.f * GravityScale * DeltaSeconds;
			const float DragFactor = FMath::Exp(-AirDrag * DeltaSeconds);
			Shard.Velocity.X *= DragFactor;
			Shard.Velocity.Y *= DragFactor;

			const FVector RelLoc = Shard.MeshComp->GetRelativeLocation() + Shard.Velocity * DeltaSeconds;
			Shard.MeshComp->SetRelativeLocation(RelLoc);
			Shard.MeshComp->AddRelativeRotation(Shard.AngularVelocity * DeltaSeconds);

			const double WorldZ = GetTransform().TransformPosition(RelLoc).Z;

			// 飞出平台：不落地，掉出视野后直接消失
			if (!Shard.bHasGround)
			{
				if (WorldZ < GetActorLocation().Z - 2500.0)
				{
					Shard.bMelted = true;
					Shard.MeshComp->SetVisibility(false);
				}
				continue;
			}

			if (WorldZ <= Shard.GroundZ + 1.f)
			{
				Shard.bLanded = true;
				++LandedCount;
				// 落地贴合纸面：把翻滚角度压平，避免碎片插进地面
				Shard.MeshComp->SetRelativeRotation(FRotator(
					FMath::FRandRange(-6.f, 6.f),
					FMath::FRandRange(-12.f, 12.f),
					FMath::FRandRange(-10.f, 10.f)));
			}
		}
		else
		{
			// 落地后缩没、下沉，融入颜料滩
			Shard.MeltTime += DeltaSeconds;
			const float Alpha = FMath::Clamp(Shard.MeltTime / FMath::Max(0.01f, MeltDuration), 0.f, 1.f);
			const float Shrink = 1.f - Alpha;

			const FVector RelLoc = Shard.MeshComp->GetRelativeLocation() - FVector(0.f, 0.f, SinkSpeed * DeltaSeconds);
			Shard.MeshComp->SetRelativeLocation(RelLoc);
			Shard.MeshComp->SetRelativeScale3D(FVector(Shrink, Shrink, Shrink));

			if (UMaterialInstanceDynamic* ShardMID = Cast<UMaterialInstanceDynamic>(Shard.MeshComp->GetMaterial(0)))
			{
				ShardMID->SetScalarParameterValue(TEXT("Fade"), Shrink);
			}

			if (Alpha >= 1.f)
			{
				Shard.bMelted = true;
				Shard.MeshComp->SetVisibility(false);
			}
		}
	}

	// 颜料滩随碎片落地摊开，之后按寿命淡出
	bool bPuddleGone = false;
	if (PuddleMesh && PuddleMID)
	{
		const float LandedProgress = (float)LandedCount / (float)FMath::Max(1, Shards.Num());
		const float GrowAlpha = FMath::Clamp(PuddleAge / FMath::Max(0.01f, PuddleGrowDuration), 0.f, 1.f);
		const float GrowScale = FMath::Lerp(0.35f, 1.f, GrowAlpha);

		PuddleMesh->SetVisibility(LandedCount > 0 && bHasPuddleGround);
		PuddleMesh->SetRelativeScale3D(FVector(GrowScale, GrowScale, GrowScale));

		float FadeAlpha = 1.f;
		if (PuddleLifetime > 0.f)
		{
			FadeAlpha = 1.f - FMath::Clamp((PuddleAge - PuddleLifetime) / 1.f, 0.f, 1.f);
			bPuddleGone = FadeAlpha <= 0.f;
		}

		if (bPuddleGone)
		{
			PuddleMesh->SetVisibility(false);
		}

		PuddleMID->SetScalarParameterValue(TEXT("Progress"), FMath::Max(LandedProgress, GrowAlpha));
		PuddleMID->SetScalarParameterValue(TEXT("Fade"), FadeAlpha);
	}

	// 结束条件：碎片全部熔完，且颜料滩已淡出（或永久保留时只降频 Tick）
	if (bAllMelted)
	{
		if (PuddleLifetime > 0.f)
		{
			if (bPuddleGone || TotalAge > PuddleLifetime + 6.f)
			{
				Destroy();
			}
		}
		else if (PuddleAge > PuddleGrowDuration + 1.f)
		{
			SetActorTickInterval(0.5f);
		}
	}

	// 兜底：碎片长时间没落地（掉出关卡）则收尾
	if (TotalAge > 12.f && LandedCount == 0)
	{
		Destroy();
	}
}
