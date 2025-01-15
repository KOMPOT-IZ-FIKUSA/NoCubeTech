// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelSimplifications.generated.h"

USTRUCT()
struct NOCUBETECH_API FVoxelSimplification3D
{
	GENERATED_BODY()
private:

	static const int q = 10;

	UPROPERTY()
	FVector minPoint = FVector::ZeroVector;
	UPROPERTY()
	FVector maxPoint = FVector::ZeroVector;
	UPROPERTY()
	FVector delta = FVector::ZeroVector;
	UPROPERTY()
	TArray<float> values = {};
	UPROPERTY()
	TArray<FVector> normalizedVertices = {};
	UPROPERTY()
	TArray<int> triangles = {};


public:


	float GetSignedDistanceToFigure(FVector point) {

		if (normalizedVertices.Num() == 0) {
			return 1;
		}

		point = (point - minPoint) / delta;

		if (
			point.X < 0 || point.X > 1 ||
			point.Y < 0 || point.Y > 1 ||
			point.Z < 0 || point.Z > 1
			)
		{
			return 1;
		}

		

		int x0 = fmin(q - 2, point.X * (q - 1));
		int y0 = fmin(q - 2, point.Y * (q - 1));
		int z0 = fmin(q - 2, point.Z * (q - 1));

		float dx = point.X * (q - 1) - x0;
		float dy = point.Y * (q - 1) - y0;
		float dz = point.Z * (q - 1) - z0;
		float xInv = 1 - dx;
		float yInv = 1 - dy;
		float zInv = 1 - dz;

		return
			GetValueOrCalculateIfNotFound(x0, y0, z0) * xInv * yInv * zInv +
			GetValueOrCalculateIfNotFound(x0, y0 + 1, z0) * xInv * dy * zInv +
			GetValueOrCalculateIfNotFound(x0 + 1, y0, z0) * dx * yInv * zInv +
			GetValueOrCalculateIfNotFound(x0 + 1, y0 + 1, z0) * dx * dy * zInv +
			GetValueOrCalculateIfNotFound(x0, y0, z0 + 1) * xInv * yInv * dz +
			GetValueOrCalculateIfNotFound(x0, y0 + 1, z0 + 1) * xInv * dy * dz +
			GetValueOrCalculateIfNotFound(x0 + 1, y0, z0 + 1) * dx * yInv * dz +
			GetValueOrCalculateIfNotFound(x0 + 1, y0 + 1, z0 + 1) * dx * dy * dz;
	}

	void SetUp(TArray<FVector>& vertices, TArray<int>& triangles_) {

		minPoint = vertices[0];
		maxPoint = vertices[0];
		values = {};

		values.Init(INFINITY, q * q * q);

		for (FVector vertex : vertices) {
			minPoint = FVector::Min(vertex, minPoint);
			maxPoint = FVector::Max(vertex, maxPoint);
		}
		delta = maxPoint - minPoint;

		normalizedVertices = {};
		for (FVector vertex : vertices) {
			normalizedVertices.Add((vertex - minPoint) / delta);
		}
		triangles = triangles_;

	}


private:

	int calculateIndex(int x, int y, int z) {
		return x * q * q + y * q + z;
	}

	float GetValueOrCalculateIfNotFound(int x, int y, int z)
	{
		int index = calculateIndex(x, y, z);
		float val = values[index];
		if (val == INFINITY) {
			val = getSignedDistanceViaRaycast(FVector(float(x) / (q - 1), float(y) / (q - 1), float(z) / (q - 1)));
			values[index] = val;
		}
		return val;
	}



	float getSignedDistanceViaRaycast(FVector normalizedPoint) {
		int raycastIntersections = 0;
		float minDistance = INFINITY;
		for (int i = 0; i < triangles.Num(); i += 3) {
			FVector& a = normalizedVertices[triangles[i]];
			FVector& b = normalizedVertices[triangles[i + 1]];
			FVector& c = normalizedVertices[triangles[i + 2]];

			float d = getDistanceToTriangle(a, b, c, normalizedPoint);
			if (d < minDistance) {
				minDistance = d;
			}
			FVector rayDirection = FVector(1, 0, 0);
			if (RaycastTriangle(a, b, c, rayDirection, normalizedPoint)) {
				raycastIntersections += 1;
			}
		}

		if (raycastIntersections % 2 == 0) {
			return minDistance;
		}
		else {
			return -minDistance;
		}
	}

	static float getDistanceToTriangle(FVector a, FVector b, FVector c, FVector p) {
		FVector ab = b - a;
		FVector ac = c - a;
		FVector ap = p - a;

		// Compute normal of the triangle
		FVector n = FVector::CrossProduct(ab, ac);
		n.Normalize();

		// Compute perpendicular distance from point to plane of the triangle
		float distToPlane = FVector::DotProduct(ap, n);

		// Project point p onto the plane
		FVector projection = p - distToPlane * n;

		// Barycentric coordinates to determine if the projection lies inside the triangle
		FVector v0 = b - a;
		FVector v1 = c - a;
		FVector v2 = projection - a;

		float d00 = FVector::DotProduct(v0, v0);
		float d01 = FVector::DotProduct(v0, v1);
		float d11 = FVector::DotProduct(v1, v1);
		float d20 = FVector::DotProduct(v2, v0);
		float d21 = FVector::DotProduct(v2, v1);

		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;

		// Check if the projection lies inside the triangle
		if (u >= 0 && v >= 0 && w >= 0) {
			return FMath::Abs(distToPlane);
		}
		else {
			// Distance to the closest point on the triangle's perimeter (edge or vertex)
			float distAB = FVector::Dist(p, FMath::ClosestPointOnLine(a, b, p));
			float distBC = FVector::Dist(p, FMath::ClosestPointOnLine(b, c, p));
			float distCA = FVector::Dist(p, FMath::ClosestPointOnLine(c, a, p));

			return FMath::Min(FMath::Min(distAB, distBC), distCA);
		}
	}

	static bool RaycastTriangle(const FVector& a, const FVector& b, const FVector& c, const FVector& rayDirection, const FVector& rayOrigin) {
		const float EPSILON = 1e-8;
		FVector ab = b - a;
		FVector ac = c - a;
		FVector pvec = FVector::CrossProduct(rayDirection, ac);
		float det = FVector::DotProduct(ab, pvec);

		if (fabs(det) < EPSILON) {
			return false;  // Ray is parallel to the triangle
		}

		float invDet = 1.0 / det;
		FVector tvec = rayOrigin - a;
		float u = FVector::DotProduct(tvec, pvec) * invDet;

		if (u < 0.0 || u > 1.0) {
			return false;
		}

		FVector qvec = FVector::CrossProduct(tvec, ab);
		float v = FVector::DotProduct(rayDirection, qvec) * invDet;

		if (v < 0.0 || u + v > 1.0) {
			return false;
		}

		float t = FVector::DotProduct(ac, qvec) * invDet;

		if (t > EPSILON) {
			return true;  // Intersection at distance t from the ray origin
		}
		else {
			return false;  // Intersection behind the ray origin
		}
	}
};
