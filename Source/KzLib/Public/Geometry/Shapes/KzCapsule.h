// Copyright 2025 kirzo

#pragma once

#include "Geometry/KzShape.h"
#include "KzCapsule.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Capsule"))
struct KZLIB_API FKzCapsule : public FKzShape
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Kz", meta = (ClampMin = 0))
	float Radius = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Kz", meta = (ClampMin = 0))
	float HalfHeight = 100.0f;

	FKzCapsule() = default;

	explicit FKzCapsule(float InRadius, float InHalfHeight)
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
		HalfHeight = FMath::Max(0.0f, HalfHeight);
		Radius = FMath::Clamp(Radius, 0.0f, HalfHeight);
	}

	virtual FBox GetBoundingBox(const FVector& Position, const FQuat& Orientation) const override
	{
		// Vector from center to one cap center (local +Z axis)
		const FVector Axis = Orientation.GetAxisZ();
		const FVector HalfSegment = Axis * (HalfHeight - Radius);

		// Endpoints of the capsule spine
		const FVector CapA = Position - HalfSegment;
		const FVector CapB = Position + HalfSegment;

		// Compute min/max bounds including the radius
		const FVector Min = FVector::Min(CapA, CapB) - FVector(Radius);
		const FVector Max = FVector::Max(CapA, CapB) + FVector(Radius);

		return FBox(Min, Max);
	}

	virtual FCollisionShape ToCollisionShape(float Inflation) const override
	{
		return FCollisionShape::MakeCapsule(Radius + Inflation, HalfHeight + Inflation);
	}

	virtual FVector GetClosestPoint(const FVector& Position, const FQuat& Orientation, const FVector& Point) const override
	{
		const FVector LocalPoint = Orientation.UnrotateVector(Point - Position);
		if (FMath::Abs(LocalPoint.Z) <= HalfHeight - Radius)
		{
			return Position + Orientation.RotateVector(LocalPoint.GetClampedToMaxSize2D(Radius));
		}
		else
		{
			const FVector Offset = FVector::UpVector * FMath::Sign(LocalPoint.Z) * (HalfHeight - Radius);
			return Position + Orientation.RotateVector((LocalPoint - Offset).GetClampedToMaxSize(Radius) + Offset);
		}
	}

	virtual bool IntersectsPoint(const FVector& Position, const FQuat& Orientation, const FVector& Point) const override
	{
		const FVector LocalPoint = Orientation.UnrotateVector(Point - Position);

		return
			(FMath::Abs(LocalPoint.Z) <= HalfHeight - Radius &&
			 LocalPoint.SizeSquared2D() <= FMath::Square(Radius)) ||
			FVector::DistSquared(FVector::UpVector * (HalfHeight - Radius) * FMath::Sign(LocalPoint.Z), LocalPoint) <= FMath::Square(Radius);
	}

	virtual bool IntersectsSphere(const FVector& Position, const FQuat& Orientation, const FVector& SphereCenter, float SphereRadius) const override
	{
		const FVector LocalCenter = Orientation.UnrotateVector(SphereCenter - Position);

		FVector ClosestPoint;
		if (FMath::Abs(LocalCenter.Z) <= HalfHeight - Radius)
		{
			ClosestPoint = LocalCenter.GetClampedToMaxSize2D(Radius);
		}
		else
		{
			const FVector Offset = FVector::UpVector * FMath::Sign(LocalCenter.Z) * (HalfHeight - Radius);
			ClosestPoint = (LocalCenter - Offset).GetClampedToMaxSize(Radius) + Offset;
		}

		const float DistSq = FVector::DistSquared(LocalCenter, ClosestPoint);
		return DistSq <= FMath::Square(SphereRadius);
	}

	FORCEINLINE FKzCapsule operator+(float Inflation) const
	{
		return FKzCapsule(Radius + Inflation, HalfHeight + Inflation);
	}

	FORCEINLINE FKzCapsule& operator+=(float Inflation)
	{
		this->Inflate(Inflation);
		return *this;
	}

	FORCEINLINE FKzCapsule operator+(const FVector& Inflation) const
	{
		return FKzCapsule(Radius + Inflation.X, HalfHeight + Inflation.Z);
	}

	FORCEINLINE FKzCapsule& operator+=(const FVector& Inflation)
	{
		this->Inflate(Inflation);
		return *this;
	}

	FORCEINLINE FKzCapsule operator*(float Scale) const
	{
		return FKzCapsule(Radius * FMath::Abs(Scale), HalfHeight * FMath::Abs(Scale));
	}

	FORCEINLINE FKzCapsule& operator*=(float Scale)
	{
		this->Scale(Scale);
		return *this;
	}

	FORCEINLINE FKzCapsule operator*(const FVector& Scale) const
	{
		return FKzCapsule(Radius * FMath::Min(FMath::Abs(Scale.X), FMath::Abs(Scale.Y)), HalfHeight * FMath::Abs(Scale.Z));
	}

	FORCEINLINE FKzCapsule& operator*=(const FVector& Scale)
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
		Radius = +Inflation.X;
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
		Radius *= FMath::Min(FMath::Abs(Scale.X), FMath::Abs(Scale.Y));
		HalfHeight *= FMath::Abs(Scale.Z);
	}

	virtual void DrawDebug(const UWorld* InWorld, FVector const& Position, const FQuat& Orientation, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) const override;

	virtual void DrawSceneProxy(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FLinearColor& Color, bool bDrawSolid, float Thickness, int32 ViewIndex, FMeshElementCollector& Collector) const override;
};

FORCEINLINE FKzCapsule operator+(float Inflation, const FKzCapsule& Shape)
{
	return Shape.operator+(Inflation);
}

FORCEINLINE FKzCapsule operator+(const FVector& Inflation, const FKzCapsule& Shape)
{
	return Shape.operator+(Inflation);
}

FORCEINLINE FKzCapsule operator*(float Scale, const FKzCapsule& Shape)
{
	return Shape.operator*(Scale);
}

FORCEINLINE FKzCapsule operator*(const FVector& Scale, const FKzCapsule& Shape)
{
	return Shape.operator*(Scale);
}