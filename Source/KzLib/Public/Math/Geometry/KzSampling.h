// Copyright 2025 kirzo

#pragma once

#include "Math/Box.h"
#include "PhysicsEngine/BodySetup.h"

namespace Kz::Geom::Sample
{
	FORCEINLINE void BoxVertices(const FBox& B, TArray<FVector>& Out)
	{
		Out.Reset(8);
		const FVector Min = B.Min;
		const FVector Max = B.Max;

		const FVector C[8] =
		{
			{Min.X, Min.Y, Min.Z},
			{Max.X, Min.Y, Min.Z},
			{Min.X, Max.Y, Min.Z},
			{Max.X, Max.Y, Min.Z},
			{Min.X, Min.Y, Max.Z},
			{Max.X, Min.Y, Max.Z},
			{Min.X, Max.Y, Max.Z},
			{Max.X, Max.Y, Max.Z},
		};

		Out.Append(C, 8);
	}

	FORCEINLINE void BoxVertices(const FKBoxElem& Box, TArray<FVector>& Out)
	{
		Out.Reset(8);

		const FQuat   Q = Box.Rotation.Quaternion();
		const FVector HalfExtents = FVector(Box.X * 0.5f, Box.Y * 0.5f, Box.Z * 0.5f);

		const FVector V[8] =
		{
			{-1.f,-1.f,-1.f}, {+1.f,-1.f,-1.f},
			{-1.f,+1.f,-1.f}, {+1.f,+1.f,-1.f},
			{-1.f,-1.f,+1.f}, {+1.f,-1.f,+1.f},
			{-1.f,+1.f,+1.f}, {+1.f,+1.f,+1.f},
		};

		for (int32 i = 0; i < 8; i++)
		{
			const FVector P = V[i] * HalfExtents;
			Out.Add(Q.RotateVector(P) + Box.Center);
		}
	}

	FORCEINLINE void SphereVertices(const FKSphereElem& S, TArray<FVector>& Out)
	{
		const float R = S.Radius;

		Out.Reset(6);
		Out.Add(FVector(R, 0, 0) + S.Center);
		Out.Add(FVector(-R, 0, 0) + S.Center);
		Out.Add(FVector(0, R, 0) + S.Center);
		Out.Add(FVector(0, -R, 0) + S.Center);
		Out.Add(FVector(0, 0, R) + S.Center);
		Out.Add(FVector(0, 0, -R) + S.Center);
	}

	FORCEINLINE void SphylVertices(const FKSphylElem& S, TArray<FVector>& Out)
	{
		const float R = S.Radius;
		const float H = S.Length * 0.5f;
		const FQuat Q = S.Rotation.Quaternion();

		Out.Reset(8);

		FVector P[8] =
		{
			{+R, 0.f, +H}, {-R, 0.f, +H}, {0.f, +R, +H}, {0.f, -R, +H},
			{+R, 0.f, -H}, {-R, 0.f, -H}, {0.f, +R, -H}, {0.f, -R, -H},
		};

		for (FVector& X : P)
		{
			Out.Add(Q.RotateVector(X) + S.Center);
		}
	}
}