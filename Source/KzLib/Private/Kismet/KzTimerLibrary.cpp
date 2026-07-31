// Copyright 2026 kirzo

#include "Kismet/KzTimerLibrary.h"
#include "Core/KzTimerSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "UObject/Stack.h"

/** Resolve the pause-proof manager from any world context object (delegate target, self, or the hidden WorldContext pin). */
static FTimerManager* GetKzTimerManager(const UObject* ContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UKzTimerSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UKzTimerSubsystem>() : nullptr;
	return Subsystem ? &Subsystem->GetTimerManager() : nullptr;
}

/** Bind Object.FunctionName as a timer delegate, rejecting functions that take parameters (they would fail at execution). */
static bool BindTimerDelegate(UObject* Object, const FString& FunctionName, FTimerDynamicDelegate& OutDelegate)
{
	const FName FunctionFName(*FunctionName);
	if (Object)
	{
		const UFunction* Func = Object->FindFunction(FunctionFName);
		if (Func && Func->ParmsSize > 0)
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("SetKzTimer passed a function (%s) that expects parameters."), *FunctionName), ELogVerbosity::Warning);
			return false;
		}
	}

	OutDelegate.BindUFunction(Object, FunctionFName);
	return true;
}

FTimerHandle UKzTimerLibrary::K2_SetTimerDelegate(FTimerDynamicDelegate Delegate, float Time, bool bLooping, bool bMaxOncePerFrame, float InitialStartDelay, float InitialStartDelayVariance)
{
	FTimerHandle Handle;
	if (!Delegate.IsBound())
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("SetKzTimer passed a bad function (%s) or object (%s)."), *Delegate.GetFunctionName().ToString(), *GetNameSafe(Delegate.GetUObject())), ELogVerbosity::Warning);
		return Handle;
	}

	if (FTimerManager* Manager = GetKzTimerManager(Delegate.GetUObject()))
	{
		InitialStartDelay += FMath::RandRange(-InitialStartDelayVariance, InitialStartDelayVariance);
		if (Time <= 0.f || (Time + InitialStartDelay) < 0.f)
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s %s SetKzTimer passed a negative or zero time. The associated timer may fail to be created/fire! If using InitialStartDelayVariance, be sure it is smaller than (Time + InitialStartDelay)."), *GetNameSafe(Delegate.GetUObject()), *Delegate.GetFunctionName().ToString()), ELogVerbosity::Warning);
		}

		Handle = Manager->K2_FindDynamicTimerHandle(Delegate);
		Manager->SetTimer(Handle, Delegate, Time, FTimerManagerTimerParameters { .bLoop = bLooping, .bMaxOncePerFrame = bMaxOncePerFrame, .FirstDelay = Time + InitialStartDelay });
	}
	return Handle;
}

FTimerHandle UKzTimerLibrary::K2_SetTimer(UObject* Object, FString FunctionName, float Time, bool bLooping, bool bMaxOncePerFrame, float InitialStartDelay, float InitialStartDelayVariance)
{
	FTimerDynamicDelegate Delegate;
	if (!BindTimerDelegate(Object, FunctionName, Delegate))
	{
		return FTimerHandle();
	}
	return K2_SetTimerDelegate(Delegate, Time, bLooping, bMaxOncePerFrame, InitialStartDelay, InitialStartDelayVariance);
}

FTimerHandle UKzTimerLibrary::K2_SetTimerForNextTickDelegate(FTimerDynamicDelegate Delegate)
{
	FTimerHandle Handle;
	if (!Delegate.IsBound())
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("SetKzTimerForNextTick passed a bad function (%s) or object (%s)."), *Delegate.GetFunctionName().ToString(), *GetNameSafe(Delegate.GetUObject())), ELogVerbosity::Warning);
		return Handle;
	}

	if (FTimerManager* Manager = GetKzTimerManager(Delegate.GetUObject()))
	{
		Handle = Manager->SetTimerForNextTick(Delegate);
	}
	return Handle;
}

FTimerHandle UKzTimerLibrary::K2_SetTimerForNextTick(UObject* Object, FString FunctionName)
{
	FTimerDynamicDelegate Delegate;
	if (!BindTimerDelegate(Object, FunctionName, Delegate))
	{
		return FTimerHandle();
	}
	return K2_SetTimerForNextTickDelegate(Delegate);
}

void UKzTimerLibrary::K2_ClearAndInvalidateTimerHandle(const UObject* WorldContextObject, FTimerHandle& Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			Manager->ClearTimer(Handle);
		}
	}
}

void UKzTimerLibrary::K2_PauseTimerHandle(const UObject* WorldContextObject, FTimerHandle Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			Manager->PauseTimer(Handle);
		}
	}
}

void UKzTimerLibrary::K2_UnPauseTimerHandle(const UObject* WorldContextObject, FTimerHandle Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			Manager->UnPauseTimer(Handle);
		}
	}
}

bool UKzTimerLibrary::K2_IsTimerActiveHandle(const UObject* WorldContextObject, FTimerHandle Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			return Manager->IsTimerActive(Handle);
		}
	}
	return false;
}

bool UKzTimerLibrary::K2_IsTimerPausedHandle(const UObject* WorldContextObject, FTimerHandle Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			return Manager->IsTimerPaused(Handle);
		}
	}
	return false;
}

bool UKzTimerLibrary::K2_TimerExistsHandle(const UObject* WorldContextObject, FTimerHandle Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			return Manager->TimerExists(Handle);
		}
	}
	return false;
}

float UKzTimerLibrary::K2_GetTimerElapsedTimeHandle(const UObject* WorldContextObject, FTimerHandle Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			return Manager->GetTimerElapsed(Handle);
		}
	}
	return -1.f;
}

float UKzTimerLibrary::K2_GetTimerRemainingTimeHandle(const UObject* WorldContextObject, FTimerHandle Handle)
{
	if (Handle.IsValid())
	{
		if (FTimerManager* Manager = GetKzTimerManager(WorldContextObject))
		{
			return Manager->GetTimerRemaining(Handle);
		}
	}
	return -1.f;
}

/** Find the handle registered for Object.FunctionName on the Kz manager - !IsValid() if none. */
static FTimerHandle FindKzDynamicTimerHandle(FTimerManager*& OutManager, UObject* Object, const FString& FunctionName)
{
	FTimerDynamicDelegate Delegate;
	Delegate.BindUFunction(Object, FName(*FunctionName));

	OutManager = GetKzTimerManager(Object);
	return OutManager ? OutManager->K2_FindDynamicTimerHandle(Delegate) : FTimerHandle();
}

void UKzTimerLibrary::K2_ClearTimer(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	if (Manager)
	{
		Manager->ClearTimer(Handle);
	}
}

void UKzTimerLibrary::K2_PauseTimer(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	const FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	if (Manager)
	{
		Manager->PauseTimer(Handle);
	}
}

void UKzTimerLibrary::K2_UnPauseTimer(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	const FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	if (Manager)
	{
		Manager->UnPauseTimer(Handle);
	}
}

bool UKzTimerLibrary::K2_IsTimerActive(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	const FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	return Manager && Handle.IsValid() ? Manager->IsTimerActive(Handle) : false;
}

bool UKzTimerLibrary::K2_IsTimerPaused(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	const FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	return Manager && Handle.IsValid() ? Manager->IsTimerPaused(Handle) : false;
}

bool UKzTimerLibrary::K2_TimerExists(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	const FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	return Manager && Handle.IsValid() ? Manager->TimerExists(Handle) : false;
}

float UKzTimerLibrary::K2_GetTimerElapsedTime(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	const FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	return Manager && Handle.IsValid() ? Manager->GetTimerElapsed(Handle) : -1.f;
}

float UKzTimerLibrary::K2_GetTimerRemainingTime(UObject* Object, FString FunctionName)
{
	FTimerManager* Manager = nullptr;
	const FTimerHandle Handle = FindKzDynamicTimerHandle(Manager, Object, FunctionName);
	return Manager && Handle.IsValid() ? Manager->GetTimerRemaining(Handle) : -1.f;
}
