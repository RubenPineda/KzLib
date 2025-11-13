// Copyright 2025 kirzo

#pragma once

#include "Math/Geometry/KzShapeInstance.h"
#include "Math/Geometry/Shapes/KzSphere.h"

FKzShapeInstance::FKzShapeInstance()
{
	Shape.InitializeAs<FKzSphere>();
}