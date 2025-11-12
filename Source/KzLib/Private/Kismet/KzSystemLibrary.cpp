// Copyright 2025 kirzo

#include "Kismet/KzSystemLibrary.h"
#include "Collision/KzHitResult.h"

void UKzSystemLibrary::BreakHitResult(const FKzHitResult& Hit, bool& bBlockingHit, bool& bInitialOverlap, float& Time, float& Distance, FVector& Location, FVector& Normal, FVector& TraceStart, FVector& TraceEnd)
{
	bBlockingHit = Hit.bBlockingHit;
	bInitialOverlap = Hit.bStartPenetrating;
	Time = Hit.Time;
	Distance = Hit.Distance;
	Location = Hit.Location;
	Normal = Hit.Normal;
	TraceStart = Hit.TraceStart;
	TraceEnd = Hit.TraceEnd;
}