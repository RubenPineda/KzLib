// Copyright 2025 kirzo

#include "KzLib.h"

#define LOCTEXT_NAMESPACE "FKzLibModule"

void FKzLibModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FKzLibModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FKzLibModule, KzLib)