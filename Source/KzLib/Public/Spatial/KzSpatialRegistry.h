// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Spatial/KzSpatialHashGrid.h"

namespace Kz
{
	/**
	 * Dual (static + dynamic) spatial registry built on TSpatialHashGrid. Static elements pay
	 * zero per-frame cost; dynamic elements are re-inserted automatically when their bounds
	 * change, provided TickDynamics() is called once per frame.
	 *
	 * TSemantics must satisfy TSpatialHashGrid's contract plus one extra static method:
	 *   static bool IsDynamic(const TElement&);
	 */
	template<typename TElement, typename TSemantics>
	class TSpatialRegistry
	{
	public:
		void SetCellSize(float InCellSize)
		{
			StaticGrid.SetCellSize(InCellSize);
			DynamicGrid.SetCellSize(InCellSize);
		}

		/**
		 * Minimum bounds movement (in units) before a dynamic element is re-indexed.
		 * Default FBox::Equals tolerance is microscopic: simulating or animated
		 * primitives jitter past it every frame and would churn Remove+Insert for
		 * elements that have not meaningfully moved. Cell placement tolerates a
		 * few units of staleness (queries narrowphase against real positions).
		 */
		void SetReindexThreshold(float InThreshold)
		{
			ReindexThreshold = FMath::Max(0.0f, InThreshold);
		}

		void Reset()
		{
			StaticGrid.Reset();
			DynamicGrid.Reset();
			Registered.Reset();
			DynamicTracks.Reset();
		}

		void Register(const TElement& Element)
		{
			if (!TSemantics::IsValid(Element) || Registered.Contains(Element))
			{
				return;
			}

			Registered.Add(Element);

			if (TSemantics::IsDynamic(Element))
			{
				DynamicTracks.Add(FDynamicTrack{ Element, TSemantics::GetBoundingBox(Element) });
				DynamicGrid.Insert(Element);
			}
			else
			{
				StaticGrid.Insert(Element);
			}
		}

		void Unregister(const TElement& Element)
		{
			if (!Registered.Contains(Element))
			{
				return;
			}

			Registered.Remove(Element);

			if (TSemantics::IsDynamic(Element))
			{
				const int32 Index = DynamicTracks.IndexOfByPredicate([&Element](const FDynamicTrack& Track) { return Track.Element == Element; });
				if (Index != INDEX_NONE)
				{
					DynamicGrid.Remove(Element, DynamicTracks[Index].LastBounds);
					DynamicTracks.RemoveAtSwap(Index);
				}
			}
			else
			{
				StaticGrid.Remove(Element, TSemantics::GetBoundingBox(Element));
			}
		}

		void Query(TArray<typename TSemantics::ElementIdType>& OutResults, const FKzShapeInstance& Shape, const FVector& Position, const FQuat& Rotation) const
		{
			StaticGrid.Query(OutResults, Shape, Position, Rotation);
			DynamicGrid.Query(OutResults, Shape, Position, Rotation);
		}

		void DebugDraw(const class UWorld* World, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) const
		{
			StaticGrid.DebugDraw(World, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
			DynamicGrid.DebugDraw(World, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
		}

		void TickDynamics()
		{
			for (int32 i = DynamicTracks.Num() - 1; i >= 0; --i)
			{
				FDynamicTrack& Track = DynamicTracks[i];

				if (!TSemantics::IsValid(Track.Element))
				{
					DynamicTracks.RemoveAtSwap(i);
					continue;
				}

				const FBox CurrentBounds = TSemantics::GetBoundingBox(Track.Element);
				if (!CurrentBounds.Equals(Track.LastBounds, ReindexThreshold))
				{
					DynamicGrid.Remove(Track.Element, Track.LastBounds);
					Track.LastBounds = CurrentBounds;
					DynamicGrid.Insert(Track.Element);
				}
			}
		}

		const TSet<TElement>& GetRegistered() const { return Registered; }

	private:
		struct FDynamicTrack
		{
			TElement Element;
			FBox LastBounds = FBox(EForceInit::ForceInit);
		};

		TSpatialHashGrid<TElement, TSemantics> StaticGrid;
		TSpatialHashGrid<TElement, TSemantics> DynamicGrid;
		TSet<TElement> Registered;
		TArray<FDynamicTrack> DynamicTracks;
		float ReindexThreshold = 10.0f;
	};
}
