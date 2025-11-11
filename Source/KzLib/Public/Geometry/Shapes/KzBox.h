// Copyright 2025 kirzo

#pragma once

#include "Geometry/KzShape.h"
#include "KzBox.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Box"))
struct KZLIB_API FKzBox : public FKzShape
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Kz")
	FVector HalfSize = FVector(50.0f);

	FKzBox() = default;

	explicit FKzBox(const FVector& InHalfSize)
		: HalfSize(InHalfSize)
	{
		Sanitize();
	}

	virtual bool IsZeroExtent() const override
	{
		return HalfSize.X <= 0.0f || HalfSize.Y <= 0.0f || HalfSize.Z <= 0.0f;
	}

	virtual void Sanitize() override
	{
		HalfSize.X = FMath::Max(0.0f, HalfSize.X);
		HalfSize.Y = FMath::Max(0.0f, HalfSize.Y);
		HalfSize.Z = FMath::Max(0.0f, HalfSize.Z);
	}

	virtual FBox GetAABB(const FVector& Position, const FQuat& Orientation) const override
	{
		const FVector AxisX = Orientation.GetAxisX();
		const FVector AxisY = Orientation.GetAxisY();
		const FVector AxisZ = Orientation.GetAxisZ();

		const FVector AbsX = AxisX.GetAbs();
		const FVector AbsY = AxisY.GetAbs();
		const FVector AbsZ = AxisZ.GetAbs();

		const FVector WorldHalfExtent(
			AbsX.X * HalfSize.X + AbsY.X * HalfSize.Y + AbsZ.X * HalfSize.Z,
			AbsX.Y * HalfSize.X + AbsY.Y * HalfSize.Y + AbsZ.Y * HalfSize.Z,
			AbsX.Z * HalfSize.X + AbsY.Z * HalfSize.Y + AbsZ.Z * HalfSize.Z
		);

		return FBox(Position - WorldHalfExtent, Position + WorldHalfExtent);
	}

	virtual FCollisionShape ToCollisionShape(float Inflation) const override
	{
		return FCollisionShape::MakeBox(HalfSize + Inflation);
	}

	virtual FVector GetClosestPoint(const FVector& Position, const FQuat& Orientation, const FVector& Point) const override
	{
		const FVector LocalPoint = Orientation.UnrotateVector(Point - Position);
		return Position + Orientation.RotateVector(LocalPoint.BoundToBox(-HalfSize, HalfSize));
	}

	virtual bool IntersectsPoint(const FVector& Position, const FQuat& Orientation, const FVector& Point) const override
	{
		const FVector LocalPoint = Orientation.UnrotateVector(Point - Position);
		return
			FMath::IsWithinInclusive(LocalPoint.X, -HalfSize.X, HalfSize.X) &&
			FMath::IsWithinInclusive(LocalPoint.Y, -HalfSize.Y, HalfSize.Y) &&
			FMath::IsWithinInclusive(LocalPoint.Z, -HalfSize.Z, HalfSize.Z);
	}

	virtual bool IntersectsSphere(const FVector& Position, const FQuat& Orientation, const FVector& SphereCenter, float SphereRadius) const override
	{
		const FVector LocalCenter = Orientation.UnrotateVector(SphereCenter - Position);
		const FVector ClosestPoint = LocalCenter.BoundToBox(-HalfSize, HalfSize);

		const float DistSq = FVector::DistSquared(LocalCenter, ClosestPoint);
		return DistSq <= FMath::Square(SphereRadius);
	}

	FORCEINLINE FKzBox operator+(float Inflation) const
	{
		return FKzBox(HalfSize + Inflation);
	}

	FORCEINLINE FKzBox& operator+=(float Inflation)
	{
		this->Inflate(Inflation);
		return *this;
	}

	FORCEINLINE FKzBox operator+(const FVector& Inflation) const
	{
		return FKzBox(HalfSize + Inflation);
	}

	FORCEINLINE FKzBox& operator+=(const FVector& Inflation)
	{
		this->Inflate(Inflation);
		return *this;
	}

	FORCEINLINE FKzBox operator*(float Scale) const
	{
		return FKzBox(HalfSize * FMath::Abs(Scale));
	}

	FORCEINLINE FKzBox& operator*=(float Scale)
	{
		this->Scale(Scale);
		return *this;
	}

	FORCEINLINE FKzBox operator*(const FVector& Scale) const
	{
		return FKzBox(HalfSize * Scale.GetAbs());
	}

	FORCEINLINE FKzBox& operator*=(const FVector& Scale)
	{
		this->Scale(Scale);
		return *this;
	}

	virtual void Inflate(float Inflation) override
	{
		HalfSize += FVector(Inflation);
		Sanitize();
	}

	virtual void Inflate(const FVector& Inflation) override
	{
		HalfSize += Inflation;
		Sanitize();
	}

	virtual void Scale(float Scale) override
	{
		HalfSize *= FMath::Abs(Scale);
	}

	virtual void Scale(const FVector& Scale) override
	{
		HalfSize *= Scale.GetAbs();
	}

	virtual void DrawDebug(const UWorld* InWorld, FVector const& Position, const FQuat& Orientation, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) const override;

	virtual void DrawSceneProxy(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FLinearColor& Color, bool bDrawSolid, float Thickness, int32 ViewIndex, FMeshElementCollector& Collector) const override;
};

FORCEINLINE FKzBox operator+(float Inflation, const FKzBox& Shape)
{
	return Shape.operator+(Inflation);
}

FORCEINLINE FKzBox operator+(const FVector& Inflation, const FKzBox& Shape)
{
	return Shape.operator+(Inflation);
}

FORCEINLINE FKzBox operator*(float Scale, const FKzBox& Shape)
{
	return Shape.operator*(Scale);
}

FORCEINLINE FKzBox operator*(const FVector& Scale, const FKzBox& Shape)
{
	return Shape.operator*(Scale);
}