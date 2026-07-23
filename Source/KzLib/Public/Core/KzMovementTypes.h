// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "KzMovementTypes.generated.h"

/** Defines how to handle the Z-coordinate (height) of a target location. */
UENUM(BlueprintType)
enum class EKzTargetVerticalAlignment : uint8
{
	/** Uses the target location exactly as provided (including Z). */
	UseTargetZ          UMETA(DisplayName = "Use Target Z"),

	/** Treats the target location as the floor level and adds the character's capsule half-height. */
	AlignFeetToTarget   UMETA(DisplayName = "Align Feet To Target (Add Capsule Height)"),

	/** Traces down from the target location to find the actual floor, then adds the capsule half-height. */
	AlignFeetToFloor    UMETA(DisplayName = "Align Feet To Floor (Trace & Add Capsule Height)"),

	/** Ignores the target's Z value and maintains the actor's current Z height. */
	KeepStartZ          UMETA(DisplayName = "Maintain Start Z")
};

class AActor;

namespace Kz
{
	/**
	 * Resolves a target location's height according to the given vertical alignment: raw Z,
	 * capsule-offset, downward floor trace, or keep-current-Z. Returns the raw location when
	 * Avatar is null (capsule/collision data is unavailable).
	 */
	KZLIB_API FVector ResolveVerticalAlignedLocation(const AActor* Avatar, const FVector& TargetLocation, EKzTargetVerticalAlignment Alignment);
}