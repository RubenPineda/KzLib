// Copyright 2025 kirzo

#include "Kismet/KzGeomLibrary.h"
#include "Collision/KzHitResult.h"
#include "Collision/KzRaycast.h"
#include "Geometry/KzShapeInstance.h"
#include "Geometry/Shapes/CommonShapes.h"

#include "KismetTraceUtils.h"

#include "KzDrawDebugHelpers.h"

// === Ray intersection functions ===

bool UKzGeomLibrary::RayIntersectsShape(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector RayStart, const FVector RayDir, float MaxDistance, const FKzShapeInstance& Shape, const FVector Position, const FRotator Rotation, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	const bool bHit = Kz::GJK::Raycast(OutHit, RayStart, RayDir, MaxDistance, Shape , Position, Rotation.Quaternion());

#if ENABLE_DRAW_DEBUG
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	DrawDebugLineTraceSingle(World, OutHit.TraceStart, OutHit.TraceEnd, DrawDebugType, bHit, OutHit.ToHitResult(), TraceColor, TraceHitColor, DrawTime);
	::DrawDebugShape(World, Position, Rotation.Quaternion(), Shape, bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
#endif

	return bHit;
}

bool UKzGeomLibrary::RayIntersectsSphere(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector RayStart, const FVector RayDir, float MaxDistance, const FVector Center, float Radius, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	const bool bHit = Kz::Raycast::Sphere(OutHit, Center, Radius, RayStart, RayDir, MaxDistance);

#if ENABLE_DRAW_DEBUG
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	DrawDebugLineTraceSingle(World, OutHit.TraceStart, OutHit.TraceEnd, DrawDebugType, bHit, OutHit.ToHitResult(), TraceColor, TraceHitColor, DrawTime);
	DrawDebugSphere(World, Center, Radius, 12, bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
#endif

	return bHit;
}

bool UKzGeomLibrary::LineIntersectsSphere(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector Start, const FVector End, const FVector Center, float Radius, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Dir; float Len;
	(End - Start).ToDirectionAndLength(Dir, Len);

	return RayIntersectsSphere(WorldContextObject, OutHit, Start, Dir, Len, Center, Radius, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
}

bool UKzGeomLibrary::RayIntersectsBox(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector RayStart, const FVector RayDir, float MaxDistance, const FVector Center, const FVector HalfSize, const FRotator Rotation, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	const bool bHit = Kz::Raycast::Box(OutHit, Center, Rotation.Quaternion(), HalfSize, RayStart, RayDir, MaxDistance);

#if ENABLE_DRAW_DEBUG
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	DrawDebugLineTraceSingle(World, OutHit.TraceStart, OutHit.TraceEnd, DrawDebugType, bHit, OutHit.ToHitResult(), TraceColor, TraceHitColor, DrawTime);
	DrawDebugBox(World, Center, HalfSize, Rotation.Quaternion(), bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
#endif

	return bHit;
}

bool UKzGeomLibrary::LineIntersectsBox(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector Start, const FVector End, const FVector Center, const FVector HalfSize, const FRotator Rotation, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Dir; float Len;
	(End - Start).ToDirectionAndLength(Dir, Len);

	return RayIntersectsBox(WorldContextObject, OutHit, Start, Dir, Len, Center, HalfSize, Rotation, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
}

bool UKzGeomLibrary::RayIntersectsCapsule(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector RayStart, const FVector RayDir, float MaxDistance, const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	const bool bHit = Kz::Raycast::Capsule(OutHit, Center, Rotation.Quaternion(), Radius, HalfHeight, RayStart, RayDir, MaxDistance);

#if ENABLE_DRAW_DEBUG
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	DrawDebugLineTraceSingle(World, OutHit.TraceStart, OutHit.TraceEnd, DrawDebugType, bHit, OutHit.ToHitResult(), TraceColor, TraceHitColor, DrawTime);
	DrawDebugCapsule(World, Center, HalfHeight, Radius, Rotation.Quaternion(), bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
#endif

	return bHit;
}

bool UKzGeomLibrary::LineIntersectsCapsule(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector Start, const FVector End, const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Dir; float Len;
	(End - Start).ToDirectionAndLength(Dir, Len);

	return RayIntersectsCapsule(WorldContextObject, OutHit, Start, Dir, Len, Center, Radius, HalfHeight, Rotation, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
}

bool UKzGeomLibrary::RayIntersectsCylinder(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector RayStart, const FVector RayDir, float MaxDistance, const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	const bool bHit = Kz::Raycast::Cylinder(OutHit, Center, Rotation.Quaternion(), Radius, HalfHeight, RayStart, RayDir, MaxDistance);

#if ENABLE_DRAW_DEBUG
	bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
	float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	DrawDebugLineTraceSingle(World, OutHit.TraceStart, OutHit.TraceEnd, DrawDebugType, bHit, OutHit.ToHitResult(), TraceColor, TraceHitColor, DrawTime);
	DrawDebugCapsule(World, Center, HalfHeight, Radius, Rotation.Quaternion(), bHit ? TraceHitColor.ToFColor(true) : TraceColor.ToFColor(true), bPersistent, LifeTime);
#endif

	return bHit;
}

bool UKzGeomLibrary::LineIntersectsCylinder(const UObject* WorldContextObject, FKzHitResult& OutHit, const FVector Start, const FVector End, const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Dir; float Len;
	(End - Start).ToDirectionAndLength(Dir, Len);

	return RayIntersectsCylinder(WorldContextObject, OutHit, Start, Dir, Len, Center, Radius, HalfHeight, Rotation, DrawDebugType, TraceColor, TraceHitColor, DrawTime);
}

// === Geometry functions ===

void UKzGeomLibrary::DrawDebugShape(const UObject* WorldContextObject, const FVector Position, const FRotator Rotation, const FKzShapeInstance& Shape, FLinearColor Color, float LifeTime, float Thickness)
{
#if ENABLE_DRAW_DEBUG
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		::DrawDebugShape(World, Position, Rotation.Quaternion(), Shape, Color.ToFColor(true), false, LifeTime, SDPG_World, Thickness);
	}
#endif
}

FBox UKzGeomLibrary::GetShapeAABB(const FVector& Position, const FQuat& Rotation, const FKzShapeInstance& Shape)
{
	return Shape.GetBoundingBox(Position, Rotation);
}

FBox UKzGeomLibrary::K2_GetShapeAABB(const FVector Position, const FRotator Rotation, const FKzShapeInstance& Shape)
{
	return GetShapeAABB(Position, Rotation.Quaternion(), Shape);
}

FVector UKzGeomLibrary::ClosestPointOnShape(const FVector& Position, const FQuat& Rotation, const FKzShapeInstance& Shape, const FVector& Point)
{
	return Shape.GetClosestPoint(Position, Rotation, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnShape(const FVector Position, const FRotator Rotation, const FKzShapeInstance& Shape, const FVector Point)
{
	return ClosestPointOnShape(Position, Rotation.Quaternion(), Shape, Point);
}

bool UKzGeomLibrary::ShapeIntersectsPoint(const FVector& Position, const FQuat& Rotation, const FKzShapeInstance& Shape, const FVector& Point)
{
	return Shape.IntersectsPoint(Position, Rotation, Point);
}

bool UKzGeomLibrary::K2_ShapeIntersectsPoint(const FVector Position, const FRotator Rotation, const FKzShapeInstance& Shape, const FVector Point)
{
	return ShapeIntersectsPoint(Position, Rotation.Quaternion(), Shape, Point);
}

bool UKzGeomLibrary::ShapeIntersectsSphere(const FVector& Position, const FQuat& Rotation, const FKzShapeInstance& Shape, const FVector& SphereCenter, float SphereRadius)
{
	return Shape.IntersectsSphere(Position, Rotation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_ShapeIntersectsSphere(const FVector Position, const FRotator Rotation, const FKzShapeInstance& Shape, const FVector SphereCenter, float SphereRadius)
{
	return ShapeIntersectsSphere(Position, Rotation.Quaternion(), Shape, SphereCenter, SphereRadius);
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

FBox UKzGeomLibrary::GetBoxAABB(const FVector& Center, const FVector& HalfSize, const FQuat& Rotation)
{
	return FKzBox(HalfSize).GetBoundingBox(Center, Rotation);
}

FBox UKzGeomLibrary::K2_GetBoxAABB(const FVector Center, const FVector HalfSize, const FRotator Rotation)
{
	return GetBoxAABB(Center, HalfSize, Rotation.Quaternion());
}

FVector UKzGeomLibrary::ClosestPointOnBox(const FVector& Center, const FVector& HalfSize, const FQuat& Rotation, const FVector& Point)
{
	return FKzBox(HalfSize).GetClosestPoint(Center, FQuat::Identity, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnBox(const FVector Center, const FVector HalfSize, const FRotator Rotation, const FVector Point)
{
	return ClosestPointOnBox(Center, HalfSize, Rotation.Quaternion(), Point);
}

bool UKzGeomLibrary::BoxIntersectsPoint(const FVector& Center, const FVector& HalfSize, const FQuat& Rotation, const FVector& Point)
{
	return FKzBox(HalfSize).IntersectsPoint(Center, Rotation, Point);
}

bool UKzGeomLibrary::K2_BoxIntersectsPoint(const FVector Center, const FVector HalfSize, const FRotator Rotation, const FVector Point)
{
	return BoxIntersectsPoint(Center, HalfSize, Rotation.Quaternion(), Point);
}

bool UKzGeomLibrary::BoxIntersectsSphere(const FVector& Center, const FVector& HalfSize, const FQuat& Rotation, const FVector& SphereCenter, float SphereRadius)
{
	return FKzBox(HalfSize).IntersectsSphere(Center, Rotation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_BoxIntersectsSphere(const FVector Center, const FVector HalfSize, const FRotator Rotation, const FVector SphereCenter, float SphereRadius)
{
	return BoxIntersectsSphere(Center, HalfSize, Rotation.Quaternion(), SphereCenter, SphereRadius);
}

FKzShapeInstance UKzGeomLibrary::MakeCapsule(const float Radius, const float HalfHeight)
{
	return FKzShapeInstance::Make<FKzCapsule>(Radius, HalfHeight);
}

FBox UKzGeomLibrary::GetCapsuleAABB(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation)
{
	return FKzCapsule(Radius, HalfHeight).GetBoundingBox(Center, Rotation);
}

FBox UKzGeomLibrary::K2_GetCapsuleAABB(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation)
{
	return GetCapsuleAABB(Center, Radius, HalfHeight, Rotation.Quaternion());
}

FVector UKzGeomLibrary::ClosestPointOnCapsule(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation, const FVector& Point)
{
	return FKzCapsule(Radius, HalfHeight).GetClosestPoint(Center, FQuat::Identity, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnCapsule(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, const FVector Point)
{
	return ClosestPointOnCapsule(Center, Radius, HalfHeight, Rotation.Quaternion(), Point);
}

bool UKzGeomLibrary::CapsuleIntersectsPoint(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation, const FVector& Point)
{
	return FKzCapsule(Radius, HalfHeight).IntersectsPoint(Center, Rotation, Point);
}

bool UKzGeomLibrary::K2_CapsuleIntersectsPoint(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, const FVector Point)
{
	return CapsuleIntersectsPoint(Center, Radius, HalfHeight, Rotation.Quaternion(), Point);
}

bool UKzGeomLibrary::CapsuleIntersectsSphere(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation, const FVector& SphereCenter, float SphereRadius)
{
	return FKzCapsule(Radius, HalfHeight).IntersectsSphere(Center, Rotation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_CapsuleIntersectsSphere(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, const FVector SphereCenter, float SphereRadius)
{
	return CapsuleIntersectsSphere(Center, Radius, HalfHeight, Rotation.Quaternion(), SphereCenter, SphereRadius);
}

FKzShapeInstance UKzGeomLibrary::MakeCylinder(const float Radius, const float HalfHeight)
{
	return FKzShapeInstance::Make<FKzCylinder>(Radius, HalfHeight);
}

FBox UKzGeomLibrary::GetCylinderAABB(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation)
{
	return FKzCylinder(Radius, HalfHeight).GetBoundingBox(Center, Rotation);
}

FBox UKzGeomLibrary::K2_GetCylinderAABB(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation)
{
	return GetCylinderAABB(Center, Radius, HalfHeight, Rotation.Quaternion());
}

FVector UKzGeomLibrary::ClosestPointOnCylinder(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation, const FVector& Point)
{
	return FKzCylinder(Radius, HalfHeight).GetClosestPoint(Center, FQuat::Identity, Point);
}

FVector UKzGeomLibrary::K2_ClosestPointOnCylinder(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, const FVector Point)
{
	return ClosestPointOnCylinder(Center, Radius, HalfHeight, Rotation.Quaternion(), Point);
}

bool UKzGeomLibrary::CylinderIntersectsPoint(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation, const FVector& Point)
{
	return FKzCylinder(Radius, HalfHeight).IntersectsPoint(Center, Rotation, Point);
}

bool UKzGeomLibrary::K2_CylinderIntersectsPoint(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, FVector Point)
{
	return CylinderIntersectsPoint(Center, Radius, HalfHeight, Rotation.Quaternion(), Point);
}

bool UKzGeomLibrary::CylinderIntersectsSphere(const FVector& Center, float Radius, float HalfHeight, const FQuat& Rotation, const FVector& SphereCenter, float SphereRadius)
{
	return FKzCylinder(Radius, HalfHeight).IntersectsSphere(Center, Rotation, SphereCenter, SphereRadius);
}

bool UKzGeomLibrary::K2_CylinderIntersectsSphere(const FVector Center, float Radius, float HalfHeight, const FRotator Rotation, const FVector SphereCenter, float SphereRadius)
{
	return CylinderIntersectsSphere(Center, Radius, HalfHeight, Rotation.Quaternion(), SphereCenter, SphereRadius);
}