// Copyright 2025 kirzo

#pragma once

#include "CoreMinimal.h"

struct FKzHitResult;

namespace Kz::Raycast
{
	// Fast path: Sphere
	bool Sphere(FKzHitResult& OutHit, const FVector& Center, float Radius, const FVector& RayStart, const FVector& RayDir, float MaxDistance);

	// Fast path: Box (AABB)
	bool Box(FKzHitResult& OutHit, const FVector& Center, const FVector& Extents, const FVector& RayStart, const FVector& RayDir, float MaxDistance);

	// Fast path: Box (OBB)
	bool Box(FKzHitResult& OutHit, const FVector& Center, const FQuat& Rotation, const FVector& Extents, const FVector& RayStart, const FVector& RayDir, float MaxDistance);

	// Fast path: Capsule
	bool Capsule(FKzHitResult& OutHit, const FVector& Center, const FQuat& Rotation, float Radius, float HalfHeight, const FVector& RayStart, const FVector& RayDir, float MaxDistance);

	// Fast path: Cylinder (aligned in local Z)
	bool Cylinder(FKzHitResult& OutHit, const FVector& Center, const FQuat& Rotation, float Radius, float HalfHeight, const FVector& RayStart, const FVector& RayDir, float MaxDistance);
}