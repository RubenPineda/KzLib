// Copyright 2025 kirzo

#include "Geometry/Shapes/KzBox.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialRenderProxy.h"

void FKzBox::DrawDebug(const UWorld* InWorld, FVector const& Position, const FQuat& Orientation, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness) const
{
	DrawDebugBox(InWorld, Position, HalfSize, Orientation, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
}

void FKzBox::DrawSceneProxy(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FLinearColor& Color, bool bDrawSolid, float Thickness, int32 ViewIndex, FMeshElementCollector& Collector) const
{
	const FVector Origin = LocalToWorld.GetOrigin();
	const FVector UnitX = LocalToWorld.GetUnitAxis(EAxis::X);
	const FVector UnitY = LocalToWorld.GetUnitAxis(EAxis::Y);
	const FVector UnitZ = LocalToWorld.GetUnitAxis(EAxis::Z);

	DrawOrientedWireBox(PDI, Origin, UnitX, UnitY, UnitZ, HalfSize, Color, SDPG_World, Thickness);

	if (bDrawSolid)
	{
		const FLinearColor SolidColor = FLinearColor(Color.R, Color.G, Color.B, 0.2f);
		FMaterialRenderProxy* const MaterialRenderProxy = new FColoredMaterialRenderProxy(GEngine->DebugMeshMaterial->GetRenderProxy(), SolidColor);
		GetBoxMesh(LocalToWorld.GetMatrixWithoutScale(), HalfSize, MaterialRenderProxy, SDPG_World, ViewIndex, Collector);
	}
}