// Copyright 2025 kirzo

#include "Geometry/Shapes/KzCapsule.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialRenderProxy.h"

void FKzCapsule::DrawDebug(const UWorld* InWorld, FVector const& Position, const FQuat& Orientation, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness) const
{
	DrawDebugCapsule(InWorld, Position, HalfHeight, Radius, Orientation, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
}

void FKzCapsule::DrawSceneProxy(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FLinearColor& Color, bool bDrawSolid, float Thickness, int32 ViewIndex, FMeshElementCollector& Collector) const
{
	const FVector Origin = LocalToWorld.GetOrigin();
	const FVector UnitX = LocalToWorld.GetUnitAxis(EAxis::X);
	const FVector UnitY = LocalToWorld.GetUnitAxis(EAxis::Y);
	const FVector UnitZ = LocalToWorld.GetUnitAxis(EAxis::Z);

	const int32 CapsuleSides = FMath::Clamp<int32>(Radius / 4.f, 16, 64);
	DrawWireCapsule(PDI, Origin, UnitX, UnitY, UnitZ, Color, Radius, HalfHeight, CapsuleSides, SDPG_World, Thickness);

	if (bDrawSolid)
	{
		const FVector Bottom = Origin - UnitZ * HalfHeight;

		const FLinearColor SolidColor = FLinearColor(Color.R, Color.G, Color.B, 0.2f);
		FMaterialRenderProxy* const MaterialRenderProxy = new FColoredMaterialRenderProxy(GEngine->DebugMeshMaterial->GetRenderProxy(), SolidColor);
		GetCapsuleMesh(Bottom, UnitX, UnitY, UnitZ, SolidColor, Radius, HalfHeight, CapsuleSides, MaterialRenderProxy, SDPG_World, false, ViewIndex, Collector);
	}
}