// Copyright 2025 kirzo

#pragma once

#include "CoreMinimal.h"
#include "KzEcsSystem.h"

namespace Kz::ECS
{
	/**
	 * Manages a collection of systems and executes them in order.
	 */
	class SystemGroup
	{
	public:
		SystemGroup(Registry& InRegistry)
			: R(InRegistry)
		{
		}

		/**
		 * Adds a new system of type T to the group.
		 * Arguments are forwarded to T's constructor.
		 */
		template<typename T, typename... Args>
		T* AddSystem(Args&&... Arguments)
		{
			TUniquePtr<T> NewSystem = MakeUnique<T>(Forward<Args>(Arguments)...);
			T* SystemPtr = NewSystem.Get();
			Systems.Add(MoveTemp(NewSystem));
			return SystemPtr;
		}

		/**
		 * Updates all systems in the order they were added.
		 */
		void Update(float DeltaTime)
		{
			for (const auto& System : Systems)
			{
				System->Update(DeltaTime, R);
			}
		}

	private:
		Registry& R;
		TArray<TUniquePtr<ISystem>> Systems;
	};
}