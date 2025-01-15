// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


struct FSimpleMeshGeometry;
struct FGeometryVertex;
struct FGeometryTriangle;
struct FVertexIndex;
struct FTriangleIndex;

class NOCUBETECH_API GeometryMathUtils
{
public:
	GeometryMathUtils();
	~GeometryMathUtils();

	static void UnwrapUV(FSimpleMeshGeometry& g, float scale);
	static void ExtractMapOfVertices(TArray<FGeometryVertex>& vertices, TArray<FGeometryTriangle>& faces, TArray<TSet<FVertexIndex>>& out);
	static void ExtractMapOfTriangles (TArray<FGeometryVertex>& vertices, TArray<FGeometryTriangle>& faces, TArray<TSet<FTriangleIndex>>& out);

	static TArray<FVertexIndex> GetConnectedUnwrappedValidVertices(TSet<FVertexIndex>& connected, TArray<bool>& unwrapped, TArray<bool>& errors);
	
	/// <summary>
	/// Returns a list of triangles corresponding the provided indices. Each resulting triangle has index1=targetVertexIndex.
	/// </summary>
	static TArray<FGeometryTriangle> GetTrianglesByIndicesAndOrientForVertex(TArray<FTriangleIndex>& indices, TArray<FGeometryTriangle>& allTriangles, int targetVertexIndex);


	/// <summary>
	/// Function unwraps a vertex with given index.
	/// If the vertex has zero or one neighbours, function does nothing.
	/// If the vertex has more that one neighbour,
	///   function copies this vertex for each face it is a part of,
	///   Calculates UV for each copy.
	///   Merges (Deletes) copies with similar uv result
	/// </summary>
	/// <param name="connectedVerticesMap">A mapping between every vertex and vertices that are connected to it.</param>
	/// <param name="vertexToProcess">The index of vertex to be unwrapped.</param>
	static bool UnwrapVertexConsideringConnectionsOrCreateNewIfNotPossible(
		TArray<FGeometryVertex>& vertices,
		TArray<FGeometryTriangle>& faces,
		TArray<TSet<FVertexIndex>>& connectedVerticesMap,
		TArray<TSet<FTriangleIndex>>& connectedTrianglesMap,
		TArray<bool>& unwrappedVertices,
		TArray<bool>& errorVertices,
		int vertexToProcess,
		float scale
	);

	// Removes faces with index == removedIndex from the collection. Decrements index of faces with index > removedIndex.
	static void HandleIndicesOnFaceRemoved(TArray<FTriangleIndex>& facesIndicesCollection, int removedIndex);
	
	static bool UnwrapVertex(FGeometryVertex& mutual1, FGeometryVertex& mutual2, FGeometryVertex& unique_opposite, FGeometryVertex& target, float scale);

	static bool IntersectCircles(double x1, double y1, double x2, double y2, double r1, double r2, double& resultX1, double& resultY1, double& resultX2, double& resultY2) {

		const double eps = 1e-4;
		r1 = abs(r1);
		r2 = abs(r2);

		double multiplier = fabsf(x1 - x2);
		multiplier = fmaxf(multiplier, fabsf(y1 - y2));
		multiplier = fmaxf(multiplier, fabsf(r1));
		multiplier = fmaxf(multiplier, fabsf(r2));
		x1 /= multiplier;
		x2 /= multiplier;
		y1 /= multiplier;
		y2 /= multiplier;
		r1 /= multiplier;
		r2 /= multiplier;
		double m2 = y1 - y2;
		double m1 = (x1 - x2) * (x1 - x2) - r1 * r1 - r2 * r2;
		double a = 4 * m2 * m2 + 4 * m1 + 4 * r1 * r1 + 4 * r2 * r2;
		double b = 4 * m2 * m2 * m2 + 4 * m1 * m2 + 8 * r1 * r1 * m2;
		double c = m1 * m1 + m2 * m2 * m2 * m2 + 2 * m1 * m2 * m2 - 4 * r1 * r1 * r2 * r2 + 4 * r1 * r1 * m2 * m2;
		double D = b * b - 4 * a * c;

		if (D < 0) {
			if (D > -eps) {
				D = 0;
			}
			else {
				return false;
			}
		}
		double y_sol_1 = y1 + (-b + sqrtf(D)) / 2 / a;
		double y_sol_2 = y1 + (-b - sqrtf(D)) / 2 / a;

		double delta_x_1_sqr = r1 * r1 - (y1 - y_sol_1) * (y1 - y_sol_1);
		if (delta_x_1_sqr < 0) {
			if (delta_x_1_sqr > -eps) {
				delta_x_1_sqr = 0;
			}
			else {
				return false;
			}
		}

		double delta_x_2_sqr = r1 * r1 - (y1 - y_sol_2) * (y1 - y_sol_2);

		if (delta_x_2_sqr < 0) {
			if (delta_x_2_sqr > -eps) {
				delta_x_2_sqr = 0;
			}
			else {
				return false;
			}
		}

		double sol_1_x = x1 + sqrtf(delta_x_1_sqr);
		double sol_2_x = x1 - sqrtf(delta_x_1_sqr);
		double sol_3_x = x1 - sqrtf(delta_x_2_sqr);
		double sol_4_x = x1 + sqrtf(delta_x_2_sqr);
		double sol_final_1_x = 0;
		double sol_final_2_x = 0;
		if (abs((sol_1_x - x2) * (sol_1_x - x2) + (y_sol_1 - y2) * (y_sol_1 - y2) - r2 * r2) < eps) {
			sol_final_1_x = sol_1_x;
		}
		else {
			sol_final_1_x = sol_2_x;
		}

		if (fabs((sol_3_x - x2) * (sol_3_x - x2) + (y_sol_2 - y2) * (y_sol_2 - y2) - r2 * r2) < eps) {
			sol_final_2_x = sol_3_x;
		}
		else {
			sol_final_2_x = sol_4_x;
		}

		resultX1 = sol_final_1_x * multiplier;
		resultY1 = y_sol_1 * multiplier;
		resultX2 = sol_final_2_x * multiplier;
		resultY2 = y_sol_2 * multiplier;
		return true;
	}



};
