// Copyright 2025 kirzo

#pragma once

#include "Geometry/KzShape.h"
#include "KzCylinder.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Cylinder"))
struct KZLIB_API FKzCylinder : public FKzShape
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Kz", meta = (ClampMin = 0))
	float Radius = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Kz", meta = (ClampMin = 0))
	float HalfHeight = 100.0f;

	FKzCylinder() = default;

	explicit FKzCylinder(float InRadius, float InHalfHeight)
		: Radius(InRadius), HalfHeight(InHalfHeight)
	{
		Sanitize();
	}

	virtual bool IsZeroExtent() const override
	{
		return Radius <= 0.0f || HalfHeight <= 0.0f;
	}

	virtual void Sanitize() override
	{
		Radius = FMath::Max(0.0f, Radius);
		HalfHeight = FMath::Max(0.0f, HalfHeight);
	}

	virtual FBox GetAABB(const FVector& Position, const FQuat& Orientation) const override
	{
		// Axis of the cylinder in world space
		const FVector Axis = Orientation.GetAxisZ();

		// Offset to top/bottom faces
		const FVector HalfSegment = Axis * HalfHeight;

		// Compute the projected world-space extents
		const FVector AbsAxisZ(FMath::Abs(Axis.X), FMath::Abs(Axis.Y), FMath::Abs(Axis.Z));
		const FVector RadialExtent = FVector(Radius) * FVector(1.0f - AbsAxisZ.X, 1.0f - AbsAxisZ.Y, 1.0f - AbsAxisZ.Z);

		// Combine caps and radius
		const FVector Top = Position + HalfSegment;
		const FVector Bottom = Position - HalfSegment;

		const FVector Min = FVector::Min(Top, Bottom) - FVector(Radius);
		const FVector Max = FVector::Max(Top, Bottom) + FVector(Radius);

		return FBox(Min, Max);
	}

	virtual FCollisionShape ToCollisionShape(float Inflation) const override
	{
		return FCollisionShape::MakeBox(FVector(Radius, Radius, HalfHeight) + Inflation);
	}

	virtual FVector GetClosestPoint(const FVector& Position, const FQuat& Orientation, const FVector& Point) const override
	{
		const FVector LocalPoint = Orientation.UnrotateVector(Point - Position);

		FVector ClosestPoint = LocalPoint.GetClampedToMaxSize2D(Radius);
		if (FMath::Abs(LocalPoint.Z) > HalfHeight)
		{
			ClosestPoint.Z = FMath::Clamp(LocalPoint.Z, -HalfHeight, HalfHeight);
		}

		return Position + Orientation.RotateVector(ClosestPoint);
	}

	virtual bool IntersectsPoint(const FVector& Position, const FQuat& Orientation, const FVector& Point) const override
	{
		const FVector LocalPoint = Orientation.UnrotateVector(Point - Position);
		return FMath::Abs(LocalPoint.Z) <= HalfHeight && LocalPoint.SizeSquared2D() <= FMath::Square(Radius);
	}

	virtual bool IntersectsSphere(const FVector& Position, const FQuat& Orientation, const FVector& SphereCenter, float SphereRadius) const override
	{
		const FVector LocalCenter = Orientation.UnrotateVector(SphereCenter - Position);

		FVector ClosestPoint = LocalCenter.GetClampedToMaxSize2D(Radius);
		if (FMath::Abs(LocalCenter.Z) > HalfHeight)
		{
			ClosestPoint.Z = FMath::Clamp(LocalCenter.Z, -HalfHeight, HalfHeight);
		}

		const float DistSq = FVector::DistSquared(LocalCenter, ClosestPoint);
		return DistSq <= FMath::Square(SphereRadius);
	}

	FORCEINLINE FKzCylinder operator+(float Inflation) const
	{
		return FKzCylinder(Radius + Inflation, HalfHeight + Inflation);
	}

	FORCEINLINE FKzCylinder& operator+=(float Inflation)
	{
		this->Inflate(Inflation);
		return *this;
	}

	FORCEINLINE FKzCylinder operator+(const FVector& Inflation) const
	{
		return FKzCylinder(Radius + Inflation.X, HalfHeight + Inflation.Z);
	}

	FORCEINLINE FKzCylinder& operator+=(const FVector& Inflation)
	{
		this->Inflate(Inflation);
		return *this;
	}

	FORCEINLINE FKzCylinder operator*(float Scale) const
	{
		return FKzCylinder(Radius * FMath::Abs(Scale), HalfHeight * FMath::Abs(Scale));
	}

	FORCEINLINE FKzCylinder& operator*=(float Scale)
	{
		this->Scale(Scale);
		return *this;
	}

	FORCEINLINE FKzCylinder operator*(const FVector& Scale) const
	{
		return FKzCylinder(Radius * FMath::Min(FMath::Abs(Scale.X), FMath::Abs(Scale.Y)), HalfHeight * FMath::Abs(Scale.Z));
	}

	FORCEINLINE FKzCylinder& operator*=(const FVector& Scale)
	{
		this->Scale(Scale);
		return *this;
	}

	virtual void Inflate(float Inflation) override
	{
		Radius += Inflation;
		HalfHeight += Inflation;
		Sanitize();
	}

	virtual void Inflate(const FVector& Inflation) override
	{
		Radius += Inflation.X;
		HalfHeight += Inflation.Z;
		Sanitize();
	}

	virtual void Scale(float Scale) override
	{
		Radius *= FMath::Abs(Scale);
		HalfHeight *= FMath::Abs(Scale);
	}

	virtual void Scale(const FVector& Scale) override
	{
		Radius *= FMath::Max(FMath::Abs(Scale.X), FMath::Abs(Scale.Y));
		HalfHeight *= FMath::Abs(Scale.Z);
	}

	virtual void DrawDebug(const UWorld* InWorld, FVector const& Position, const FQuat& Orientation, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) const override;

	virtual void DrawSceneProxy(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FLinearColor& Color, bool bDrawSolid, float Thickness, int32 ViewIndex, FMeshElementCollector& Collector) const override;
};

FORCEINLINE FKzCylinder operator+(float Inflation, const FKzCylinder& Shape)
{
	return Shape.operator+(Inflation);
}

FORCEINLINE FKzCylinder operator+(const FVector& Inflation, const FKzCylinder& Shape)
{
	return Shape.operator+(Inflation);
}

FORCEINLINE FKzCylinder operator*(float Scale, const FKzCylinder& Shape)
{
	return Shape.operator*(Scale);
}

FORCEINLINE FKzCylinder operator*(const FVector& Scale, const FKzCylinder& Shape)
{
	return Shape.operator*(Scale);
}

#undef SHAPE_OPERATORS