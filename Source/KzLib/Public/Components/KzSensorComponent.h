// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Components/KzShapeComponent.h"
#include "KzSensorComponent.generated.h"

class UKzRegistrySubsystem;

/** Struct to pair the logical object with the specific shape that triggered detection */
USTRUCT(BlueprintType)
struct FKzOverlapResult
{
	GENERATED_BODY()

	/** The high-level object (Actor, Component, or custom UObject) */
	UPROPERTY(BlueprintReadOnly, Category = "Kz Sensor")
	TWeakObjectPtr<UObject> LogicObject;

	/** The specific shape component that caused the overlap */
	UPROPERTY(BlueprintReadOnly, Category = "Kz Sensor")
	TWeakObjectPtr<UKzShapeComponent> PhysicalShape;

	bool operator==(const UObject* Other) const { return LogicObject.Get() == Other; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKzSensorObjectEvent, const FKzOverlapResult&, OverlapInfo);

/**
 * A sensor component that detects objects using KzLib's GJK intersection.
 * Optimized to maintain the link between the Physical Shape and the Logical Object
 * to avoid ambiguous resolution steps.
 */
UCLASS(ClassGroup = "KzLib", meta = (BlueprintSpawnableComponent))
class KZLIB_API UKzSensorComponent : public UKzShapeComponent
{
	GENERATED_BODY()

public:
	UKzSensorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- Configuration ---

	/** The class type to look for in the KzRegistrySubsystem.*/
	UPROPERTY(EditAnywhere, Category = "Kz Sensor")
	TSubclassOf<UObject> AutoRegisterCategory;

	/** How often to perform the geometric scan (in seconds). */
	UPROPERTY(EditAnywhere, Category = "Kz Sensor", meta = (ClampMin = 0, Units = "Seconds"))
	float ScanInterval = 0.1f;

	// --- Events ---

	/** Fired when an object enters the sensor. */
	UPROPERTY(BlueprintAssignable, Category = "Kz Sensor")
	FKzSensorObjectEvent OnObjectBeginOverlap;

	/** Fired when an object exits the sensor. */
	UPROPERTY(BlueprintAssignable, Category = "Kz Sensor")
	FKzSensorObjectEvent OnObjectEndOverlap;

	// --- Public API ---

	/**
	 * Returns the list of currently overlapped logic objects cast to T.
	 * Convenience wrapper if you don't care about the physical shape.
	 */
	template <typename T>
	TArray<T*> GetOverlappingObjects() const
	{
		TArray<T*> Result;
		Result.Reserve(CachedOverlaps.Num());

		for (const FKzOverlapResult& Overlap : CachedOverlaps)
		{
			if (UObject* Obj = Overlap.LogicObject.Get())
			{
				if (T* Casted = Cast<T>(Obj))
				{
					Result.Add(Casted);
				}
			}
		}
		return Result;
	}

	/** Returns the full overlap info (Logic Object + Physical Shape). */
	UFUNCTION(BlueprintCallable, Category = "Kz Sensor")
	const TArray<FKzOverlapResult>& GetOverlapInfos() const { return CachedOverlaps; }

	/** Helper to get the physical shape associated with a specific logical object. */
	UFUNCTION(BlueprintCallable, Category = "Kz Sensor")
	UKzShapeComponent* GetShapeForObject(UObject* LogicObject) const;

private:
	void PerformScan();

	/** The cache of resolved logical objects and shapes. */
	TArray<FKzOverlapResult> CachedOverlaps;

	float TimeSinceLastScan = 0.0f;
};