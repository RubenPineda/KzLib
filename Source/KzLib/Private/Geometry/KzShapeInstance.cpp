// Copyright 2025 kirzo

#pragma once

#include "Geometry/KzShapeInstance.h"
#include "Geometry/Shapes/KzSphere.h"

FKzShapeInstance::FKzShapeInstance()
{
	Shape.InitializeAs<FKzSphere>();
}