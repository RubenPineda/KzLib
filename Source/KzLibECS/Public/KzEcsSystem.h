// Copyright 2025 kirzo

#pragma once

#include "CoreMinimal.h"
#include "KzEcsRegistry.h"

namespace Kz::ECS
{
	/**
	 * Interface for a system that processes entities.
	 */
	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		/**
		 * Executed every frame/tick.
		 * @param DeltaTime  Time elapsed since last update.
		 * @param Registry   The ECS registry capable of querying entities.
		 */
		virtual void Update(float DeltaTime, Registry& Registry) = 0;
	};

}