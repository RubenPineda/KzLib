// Copyright 2025 kirzo

#pragma once

#include "CoreMinimal.h"

struct FKzShapeInstance;
struct FKzHitResult;

namespace Kz::GJK
{
	/** Performs a raycast against a convex shape. */
	bool Raycast(FKzHitResult& OutHit,
							 const FVector& RayOrigin, const FVector& RayDir, float MaxDistance,
							 const FKzShapeInstance& Shape, const FVector& ShapePosition, const FQuat& ShapeRotation);

	/** Performs a GJK intersection test between two convex shapes. */
	bool Intersect(const FKzShapeInstance& ShapeA, const FVector& PositionA, const FQuat& RotationA,
								 const FKzShapeInstance& ShapeB, const FVector& PositionB, const FQuat& RotationB,
								 int32 MaxIterations = 20);
}