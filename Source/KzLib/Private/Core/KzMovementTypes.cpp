// Copyright 2026 kirzo

#include "Core/KzMovementTypes.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

namespace Kz
{
	FVector ResolveVerticalAlignedLocation(const AActor* Avatar, const FVector& TargetLocation, EKzTargetVerticalAlignment Alignment)
	{
		if (!Avatar)
		{
			return TargetLocation;
		}

		FVector FinalTargetLocation = TargetLocation;
		const float HalfHeight = Avatar->GetSimpleCollisionHalfHeight();
		const FVector UpDir = Avatar->GetActorUpVector();

		switch (Alignment)
		{
		case EKzTargetVerticalAlignment::KeepStartZ:
			FinalTargetLocation.Z = Avatar->GetActorLocation().Z;
			break;

		case EKzTargetVerticalAlignment::AlignFeetToTarget:
			FinalTargetLocation += (UpDir * HalfHeight);
			break;

		case EKzTargetVerticalAlignment::AlignFeetToFloor:
			if (const UWorld* World = Avatar->GetWorld())
			{
				FHitResult HitResult;
				const FVector TraceStart = TargetLocation + (UpDir * 10.0f);
				const FVector TraceEnd = TargetLocation - (UpDir * HalfHeight * 2.0f);

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(Avatar);

				ECollisionChannel ObjectType = ECC_Visibility;
				FCollisionResponseParams ResponseParams;
				if (const UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Avatar->GetRootComponent()))
				{
					ObjectType = Primitive->GetCollisionObjectType();
					ResponseParams = FCollisionResponseParams(Primitive->GetCollisionResponseToChannels());
				}

				// Fall back to AlignFeetToTarget when no floor is found in range.
				if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ObjectType, QueryParams, ResponseParams))
				{
					FinalTargetLocation = HitResult.ImpactPoint + (UpDir * HalfHeight);
				}
				else
				{
					FinalTargetLocation += (UpDir * HalfHeight);
				}
			}
			break;

		case EKzTargetVerticalAlignment::UseTargetZ:
		default:
			break;
		}

		return FinalTargetLocation;
	}
}