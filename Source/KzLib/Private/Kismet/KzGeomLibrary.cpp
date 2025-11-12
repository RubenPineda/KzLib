// Copyright 2025 kirzo

#include "Kismet/KzGeomLibrary.h"
#include "Geometry/KzShapeInstance.h"
#include "Geometry/Shapes/CommonShapes.h"

#include "KismetTraceUtils.h"

#include "KzDrawDebugHelpers.h"

bool UKzGeomLibrary::RayIntersectsSphere(const FVector RayOrigin, const FVector RayDir, float RayLength, const FVector SphereCenter, float SphereRadius, bool& bStartInside, float& OutDistance, FVector& OutHitPoint)
{
	const FVector OC = RayOrigin - SphereCenter;
	const float a = FVector::DotProduct(RayDir, RayDir); // should be 1.0 if normalized
	const float b = 2.0f * FVector::DotProduct(OC, RayDir);
	const float c = FVector::DotProduct(OC, OC) - (SphereRadius * SphereRadius);
	const float Discriminant = b * b - 4.0f * a * c;

	if (Discriminant < 0.0f)
	{
		return false; // no intersection
	}

	// Calculate the two potential intersection distances.
	const float SqrtDisc = FMath::Sqrt(Discriminant);
	const float Inv2A = 0.5f / a;
	const float t1 = (-b - SqrtDisc) * Inv2A;
	const float t2 = (-b + SqrtDisc) * Inv2A;

	bStartInside = false;
	float HitDist = t1;

	if (t1 < 0.0f)
	{
		HitDist = t2;
		bStartInside = true;
		if (t2 < 0.0f)
		{
			OutHitPoint = RayOrigin;
			return false; // both negative
		}
	}

	if (RayLength > 0.0f && HitDist > RayLength)
	{
		return false; // beyond ray length
	}

	OutDistance = HitDist;
	OutHitPoint = RayOrigin + RayDir * HitDist;
	return true;
}

bool UKzGeomLibrary::RayIntersectsBox(const FVector RayOrigin, const FVector RayDir, float RayLength, const FVector Center, const FVector BoxHalfSize, const FQuat Orientation, bool& bStartInside, float& OutDistance, FVector& OutHitPoint)
{
	FVector RayOrigin_Local = RayOrigin - Center;
	FVector RayDir_Local = RayDir;

	const bool bIsIdentity = Orientation.IsIdentity();
	if (!bIsIdentity)
	{
		RayOrigin_Local = Orientation.UnrotateVector(RayOrigin_Local);
		RayDir_Local = Orientation.UnrotateVector(RayDir_Local);
	}

	const FVector Min = -BoxHalfSize;
	const FVector Max =  BoxHalfSize;

	// Check if ray starts inside the box
	bStartInside = (RayOrigin_Local.X >= Min.X && RayOrigin_Local.X <= Max.X) && (RayOrigin_Local.Y >= Min.Y && RayOrigin_Local.Y <= Max.Y) && (RayOrigin_Local.Z >= Min.Z && RayOrigin_Local.Z <= Max.Z);

	if (bStartInside)
	{
		OutDistance = 0.0f;
		OutHitPoint = RayOrigin;
		return true;
	}

	float tmin = 0.f;
	float tmax = (RayLength <= 0.f) ? UE_BIG_NUMBER : RayLength;

	for (int32 k = 0; k < 3; ++k)
	{
		const float D = RayDir_Local[k];
		const float O = RayOrigin_Local[k];

		if (FMath::IsNearlyZero(D, UE_KINDA_SMALL_NUMBER))
		{
			// Ray is parallel to the slab planes on this axis
			if (O < Min[k] || O > Max[k])
			{
				return false; // Outside the box in this axis
			}
			// Otherwise, the ray stays inside slab range; no change to tmin/tmax
			continue;
		}

		const float InvD = 1.0f / D;
		float t0 = (Min[k] - O) * InvD;
		float t1 = (Max[k] - O) * InvD;

		if (InvD < 0.0f)
		{
			Swap(t0, t1);
		}

		tmin = FMath::Max(tmin, t0);
		tmax = FMath::Min(tmax, t1);

		if (tmax < tmin)
		{
			return false;
		}
	}

	if (tmin > RayLength)
	{
		return false; // Beyond max ray distance
	}

	OutDistance = FMath::Max(tmin, 0.0f);

	const FVector LocalHit = RayOrigin_Local + RayDir_Local * OutDistance;
	OutHitPoint = Center + (bIsIdentity ? LocalHit : Orientation.RotateVector(LocalHit));

	return true;
}

bool UKzGeomLibrary::LineIntersectsSphere(const UObject* WorldContextObject, const FVector Start, const FVector End, const FVector SphereCenter, float SphereRadius, bool& bStartInside, float& OutDistance, FVector& OutHitPoint, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Dir; float Len;
	(End - Start).ToDirectionAndLength(Dir, Len);

	const bool bHit = RayIntersectsSphere(Start, Dir, Len, SphereCenter, SphereRadius, bStartInside, OutDistance, OutHitPoint);

#if ENABLE_DRAW_DEBUG
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	FHitResult Hit;
	Hit.bBlockingHit = bHit;
	Hit.ImpactPoint = OutHitPoint;
	DrawDebugLineTraceSingle(World, Start, End, DrawDebugType, bHit, Hit, TraceColor, TraceHitColor, DrawTime);
	DrawDebugSphere(World, SphereCenter, SphereRadius, 16, bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
#endif

	return bHit;
}

bool UKzGeomLibrary::LineIntersectsBox(const UObject* WorldContextObject, const FVector Start, const FVector End, const FVector Center, const FVector HalfSize, const FRotator Orientation, bool& bStartInside, float& OutDistance, FVector& OutHitPoint, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Dir; float Len;
	(End - Start).ToDirectionAndLength(Dir, Len);

	const bool bHit = RayIntersectsBox(Start, Dir, Len, Center, HalfSize, Orientation.Quaternion(), bStartInside, OutDistance, OutHitPoint);

#if ENABLE_DRAW_DEBUG
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	FHitResult Hit;
	Hit.bBlockingHit = bHit;
	Hit.ImpactPoint = OutHitPoint;
	DrawDebugLineTraceSingle(World, Start, End, DrawDebugType, bHit, Hit, TraceColor, TraceHitColor, DrawTime);
	DrawDebugBox(World, Center, HalfSize, Orientation.Quaternion(), bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
#endif

	return bHit;
}

void UKzGeomLibrary::DrawDebugShape(const UObject* WorldContextObject, const FVector Position, const FRotator Orientation, const FKzShapeInstance& Shape, FLinearColor Color, float LifeTime, float Thickness)
{
#if ENABLE_DRAW_DEBUG
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		::DrawDebugShape(World, Position, Orientation.Quaternion(), Shape, Color.ToFColor(true), false, LifeTime, SDPG_World, Thickness);
	}
#endif
}

FBox UKzGeomLibrary::GetShapeAABB(const FVector& Position, const FQuat& Orientation, const FKzShapeInstance& Shape)
{
	return Shape.GetBoundingBox(Position, Orientation);
}

FBox UKzGeomLibrary::K2_GetShapeAABB(const FVector Position, const FRotator Orientation, const FKzShapeInstance& Shape)
{
	return GetShapeAABB(Position, Orientation.Quaternion(), Shape);
}

FVector UKzGeomLibrary::ClosestPointOnShape(const FVector& Position, const FQuat& Orientation, const FKzShapeInstance& Shape, const FVector& Point)
{
	return Shape.GetClosestPoint(Position, Orientation, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnShape(const FVector Position, const FRotator Orientation, const FKzShapeInstance& Shape, const FVector Point)
{
	return ClosestPointOnShape(Position, Orientation.Quaternion(), Shape, Point);
}

bool UKzGeomLibrary::ShapeIntersectsPoint(const FVector& Position, const FQuat& Orientation, const FKzShapeInstance& Shape, const FVector& Point)
{
	return Shape.IntersectsPoint(Position, Orientation, Point);
}

bool UKzGeomLibrary::K2_ShapeIntersectsPoint(const FVector Position, const FRotator Orientation, const FKzShapeInstance& Shape, const FVector Point)
{
	return ShapeIntersectsPoint(Position, Orientation.Quaternion(), Shape, Point);
}

bool UKzGeomLibrary::ShapeIntersectsSphere(const FVector& Position, const FQuat& Orientation, const FKzShapeInstance& Shape, const FVector& SphereCenter, float SphereRadius)
{
	return Shape.IntersectsSphere(Position, Orientation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_ShapeIntersectsSphere(const FVector Position, const FRotator Orientation, const FKzShapeInstance& Shape, const FVector SphereCenter, float SphereRadius)
{
	return ShapeIntersectsSphere(Position, Orientation.Quaternion(), Shape, SphereCenter, SphereRadius);
}

FKzShapeInstance UKzGeomLibrary::MakeSphere(const float Radius)
{
	return FKzShapeInstance::Make<FKzSphere>(Radius);
}

FBox UKzGeomLibrary::GetSphereAABB(const FVector Center, float Radius)
{
	return FKzSphere(Radius).GetBoundingBox(Center, FQuat::Identity);
}

FVector UKzGeomLibrary::ClosestPointOnSphere(const FVector Center, float Radius, FVector Point)
{
	return FKzSphere(Radius).GetClosestPoint(Center, FQuat::Identity, Point);
}

bool UKzGeomLibrary::SphereIntersectsPoint(const FVector Center, float Radius, const FVector Point)
{
	return FKzSphere(Radius).IntersectsPoint(Center, FQuat::Identity, Point);
}

bool UKzGeomLibrary::SphereIntersectsSphere(const FVector CenterA, float RadiusA, const FVector CenterB, float RadiusB)
{
	return FKzSphere(RadiusA).IntersectsSphere(CenterA, FQuat::Identity, CenterB, RadiusB);
}

FKzShapeInstance UKzGeomLibrary::MakeBox(const FVector HalfSize)
{
	return FKzShapeInstance::Make<FKzBox>(HalfSize);
}

FBox UKzGeomLibrary::GetBoxAABB(const FVector& Center, const FVector& HalfSize, const FQuat& Orientation)
{
	return FKzBox(HalfSize).GetBoundingBox(Center, Orientation);
}

FBox UKzGeomLibrary::K2_GetBoxAABB(const FVector Center, const FVector HalfSize, const FRotator Orientation)
{
	return GetBoxAABB(Center, HalfSize, Orientation.Quaternion());
}

FVector UKzGeomLibrary::ClosestPointOnBox(const FVector& Center, const FVector& HalfSize, const FQuat& Orientation, const FVector& Point)
{
	return FKzBox(HalfSize).GetClosestPoint(Center, FQuat::Identity, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnBox(const FVector Center, const FVector HalfSize, const FRotator Orientation, const FVector Point)
{
	return ClosestPointOnBox(Center, HalfSize, Orientation.Quaternion(), Point);
}

bool UKzGeomLibrary::BoxIntersectsPoint(const FVector& Center, const FVector& HalfSize, const FQuat& Orientation, const FVector& Point)
{
	return FKzBox(HalfSize).IntersectsPoint(Center, Orientation, Point);
}

bool UKzGeomLibrary::K2_BoxIntersectsPoint(const FVector Center, const FVector HalfSize, const FRotator Orientation, const FVector Point)
{
	return BoxIntersectsPoint(Center, HalfSize, Orientation.Quaternion(), Point);
}

bool UKzGeomLibrary::BoxIntersectsSphere(const FVector& Center, const FVector& HalfSize, const FQuat& Orientation, const FVector& SphereCenter, float SphereRadius)
{
	return FKzBox(HalfSize).IntersectsSphere(Center, Orientation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_BoxIntersectsSphere(const FVector Center, const FVector HalfSize, const FRotator Orientation, const FVector SphereCenter, float SphereRadius)
{
	return BoxIntersectsSphere(Center, HalfSize, Orientation.Quaternion(), SphereCenter, SphereRadius);
}

FKzShapeInstance UKzGeomLibrary::MakeCapsule(const float Radius, const float HalfHeight)
{
	return FKzShapeInstance::Make<FKzCapsule>(Radius, HalfHeight);
}

FBox UKzGeomLibrary::GetCapsuleAABB(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation)
{
	return FKzCapsule(Radius, HalfHeight).GetBoundingBox(Center, Orientation);
}

FBox UKzGeomLibrary::K2_GetCapsuleAABB(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation)
{
	return GetCapsuleAABB(Center, Radius, HalfHeight, Orientation.Quaternion());
}

FVector UKzGeomLibrary::ClosestPointOnCapsule(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation, const FVector& Point)
{
	return FKzCapsule(Radius, HalfHeight).GetClosestPoint(Center, FQuat::Identity, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnCapsule(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation, const FVector Point)
{
	return ClosestPointOnCapsule(Center, Radius, HalfHeight, Orientation.Quaternion(), Point);
}

bool UKzGeomLibrary::CapsuleIntersectsPoint(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation, const FVector& Point)
{
	return FKzCapsule(Radius, HalfHeight).IntersectsPoint(Center, Orientation, Point);
}

bool UKzGeomLibrary::K2_CapsuleIntersectsPoint(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation, const FVector Point)
{
	return CapsuleIntersectsPoint(Center, Radius, HalfHeight, Orientation.Quaternion(), Point);
}

bool UKzGeomLibrary::CapsuleIntersectsSphere(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation, const FVector& SphereCenter, float SphereRadius)
{
	return FKzCapsule(Radius, HalfHeight).IntersectsSphere(Center, Orientation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_CapsuleIntersectsSphere(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation, const FVector SphereCenter, float SphereRadius)
{
	return CapsuleIntersectsSphere(Center, Radius, HalfHeight, Orientation.Quaternion(), SphereCenter, SphereRadius);
}

FKzShapeInstance UKzGeomLibrary::MakeCylinder(const float Radius, const float HalfHeight)
{
	return FKzShapeInstance::Make<FKzCylinder>(Radius, HalfHeight);
}

FBox UKzGeomLibrary::GetCylinderAABB(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation)
{
	return FKzCylinder(Radius, HalfHeight).GetBoundingBox(Center, Orientation);
}

FBox UKzGeomLibrary::K2_GetCylinderAABB(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation)
{
	return GetCylinderAABB(Center, Radius, HalfHeight, Orientation.Quaternion());
}

FVector UKzGeomLibrary::ClosestPointOnCylinder(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation, const FVector& Point)
{
	return FKzCylinder(Radius, HalfHeight).GetClosestPoint(Center, FQuat::Identity, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnCylinder(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation, const FVector Point)
{
	return ClosestPointOnCylinder(Center, Radius, HalfHeight, Orientation.Quaternion(), Point);
}

bool UKzGeomLibrary::CylinderIntersectsPoint(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation, const FVector& Point)
{
	return FKzCylinder(Radius, HalfHeight).IntersectsPoint(Center, Orientation, Point);
}

bool UKzGeomLibrary::K2_CylinderIntersectsPoint(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation, FVector Point)
{
	return CylinderIntersectsPoint(Center, Radius, HalfHeight, Orientation.Quaternion(), Point);
}

bool UKzGeomLibrary::CylinderIntersectsSphere(const FVector& Center, float Radius, float HalfHeight, const FQuat& Orientation, const FVector& SphereCenter, float SphereRadius)
{
	return FKzCylinder(Radius, HalfHeight).IntersectsSphere(Center, Orientation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_CylinderIntersectsSphere(const FVector Center, float Radius, float HalfHeight, const FRotator Orientation, const FVector SphereCenter, float SphereRadius)
{
	return CylinderIntersectsSphere(Center, Radius, HalfHeight, Orientation.Quaternion(), SphereCenter, SphereRadius);
}