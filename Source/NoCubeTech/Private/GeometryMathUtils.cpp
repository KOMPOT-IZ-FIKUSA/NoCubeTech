// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryMathUtils.h"
#include "DataStructures.h"

GeometryMathUtils::GeometryMathUtils()
{
}

GeometryMathUtils::~GeometryMathUtils()
{
}

void GeometryMathUtils::ExtractMapOfVertices(TArray<FGeometryVertex>& vertices, TArray<FGeometryTriangle>& faces, TArray<TSet<FVertexIndex>>& out) {
	out = {};
	for (int i = 0; i < vertices.Num(); i++) {
		out.Add(TSet<FVertexIndex>());
	}
	for (FGeometryTriangle& face : faces) {
		out[face.index1.i].Add(face.index2);
		out[face.index1.i].Add(face.index3);
		out[face.index2.i].Add(face.index1);
		out[face.index2.i].Add(face.index3);
		out[face.index3.i].Add(face.index1);
		out[face.index3.i].Add(face.index2);
	}
	for (int i = 0; i < vertices.Num(); i++) {
		out[i].Remove(FVertexIndex(i));
	}
}

void GeometryMathUtils::ExtractMapOfTriangles(TArray<FGeometryVertex>& vertices, TArray<FGeometryTriangle>& faces, TArray<TSet<FTriangleIndex>>& out) {
	out = {};
	for (int i = 0; i < vertices.Num(); i++) {
		out.Add(TSet<FTriangleIndex>());
	}
	for (int i = 0; i < faces.Num(); i++) {
		FGeometryTriangle& face = faces[i];
		out[face.index1.i].Add(FTriangleIndex(i));
		out[face.index2.i].Add(FTriangleIndex(i));
		out[face.index3.i].Add(FTriangleIndex(i));
	}
}

TArray<FVertexIndex> GeometryMathUtils::GetConnectedUnwrappedValidVertices(TSet<FVertexIndex>& connected, TArray<bool>& unwrapped, TArray<bool>& errors) {
	TSet<FVertexIndex> result = {};
	for (FVertexIndex connected1 : connected) {
		if (unwrapped[connected1.i] && !errors[connected1.i]) {
			result.Add(connected1);
		}
	}
	return result.Array();
}

bool GeometryMathUtils::UnwrapVertex(FGeometryVertex& mutual1, FGeometryVertex& mutual2, FGeometryVertex& unique_opposite, FGeometryVertex& target, float scale) {
	double resultX1 = 0;
	double resultY1 = 0;
	double resultX2 = 0;
	double resultY2 = 0;

	double r1 = target.Dist(mutual1) * scale;
	double r2 = target.Dist(mutual2) * scale;


	bool success = IntersectCircles(mutual1.u, mutual1.v, mutual2.u, mutual2.v, r1, r2, resultX1, resultY1, resultX2, resultY2);
	if (!success) {
		return false;
	}
	if (
		(resultX1 - unique_opposite.u) * (resultX1 - unique_opposite.u) + (resultY1 - unique_opposite.v) * (resultY1 - unique_opposite.v) >
		(resultX2 - unique_opposite.u) * (resultX2 - unique_opposite.u) + (resultY2 - unique_opposite.v) * (resultY2 - unique_opposite.v)) {
		target.u = resultX1;
		target.v = resultY1;
	}
	else {
		target.u = resultX2;
		target.v = resultY2;
	}
	return true;
}

TArray<FGeometryTriangle> GeometryMathUtils::GetTrianglesByIndicesAndOrientForVertex(TArray<FTriangleIndex>& indices, TArray<FGeometryTriangle>& allTriangles, int targetVertexIndex) {
	TArray<FGeometryTriangle> result = {};
	for (FTriangleIndex i : indices) {
		FGeometryTriangle triangle = allTriangles[i.i];
		if (triangle.index1 == targetVertexIndex) {
			result.Add(triangle);
		}
		else if (triangle.index2 == targetVertexIndex) {
			result.Add(FGeometryTriangle(triangle.index2, triangle.index3, triangle.index1));
		}
		else {
			result.Add(FGeometryTriangle(triangle.index3, triangle.index1, triangle.index2));
		}
	}
	return result;
}

void GeometryMathUtils::HandleIndicesOnFaceRemoved(TArray<FTriangleIndex>& facesIndicesCollection, int removedIndex) {
	for (int i = 0; i < facesIndicesCollection.Num(); i++) {
		int index = facesIndicesCollection[i].i;
		if (index == removedIndex) {
			facesIndicesCollection.RemoveAt(i);
			i -= 1;
		}
		else if (index > removedIndex) {
			facesIndicesCollection[i].i -= 1;
		}
	}
}

bool GeometryMathUtils::UnwrapVertexConsideringConnectionsOrCreateNewIfNotPossible(
	TArray<FGeometryVertex>& vertices,
	TArray<FGeometryTriangle>& faces,
	TArray<TSet<FVertexIndex>>& connectedVerticesMap,
	TArray<TSet<FTriangleIndex>>& connectedTrianglesMap,
	TArray<bool>& unwrappedVertices,
	TArray<bool>& errorVertices,
	int vertexToProcessIndex,
	float scale
) {
	FGeometryVertex& vertexToProcess = vertices[vertexToProcessIndex];
	TArray<FVertexIndex> unwrappedValidNeighbours = GetConnectedUnwrappedValidVertices(connectedVerticesMap[vertexToProcessIndex], unwrappedVertices, errorVertices);
	float eps = 1e-4;
	if (unwrappedValidNeighbours.Num() <= 1) {

	}
	else if (unwrappedValidNeighbours.Num() == 2) {
		FVertexIndex v0i = unwrappedValidNeighbours[0];
		FVertexIndex v1i = unwrappedValidNeighbours[1];
		FGeometryVertex& v0 = vertices[v0i.i];
		FGeometryVertex& v1 = vertices[v1i.i];
		if (abs(v0.Dist(v1) * scale - v0.DistUV(v1)) < eps) {
			TSet<FVertexIndex> mutual_vertices_of_v1_and_v2 = connectedVerticesMap[v1i.i].Intersect(connectedVerticesMap[v1i.i]);
			FVertexIndex unique_v3 = FVertexIndex(-1);
			bool found_unique_v3 = false;
			for (FVertexIndex v3 : mutual_vertices_of_v1_and_v2) {
				if (unwrappedVertices[v3.i]) {
					found_unique_v3 = true;
					unique_v3 = v3;
					break;
				}
			}
			if (!found_unique_v3) {
				unique_v3 = v0i;
			}
			UnwrapVertex(v0, v1, vertices[unique_v3.i], vertexToProcess, scale); 
		}
		else {
			// v0 and v1 are at the wrong distance in uv
		}

	}
	else {
		// More than two connected unwrapped vertices
		// 
		// Get triangles this vertex is connected to
		TArray<FTriangleIndex> trianglesConnectedToTarget_i = connectedTrianglesMap[vertexToProcessIndex].Array();
		TArray<FGeometryTriangle> trianglesConnectedToTarget = GetTrianglesByIndicesAndOrientForVertex(trianglesConnectedToTarget_i, faces, vertexToProcessIndex);
		TArray<FVertexIndex> newVertices = {};

		// Create copies of this vertex for every triangles excepting one
		for (int i = 0; i < trianglesConnectedToTarget.Num() - 1; i++) {
			// Destroy existing triangle
			int indexToRemove = trianglesConnectedToTarget_i[i].i;
			faces.RemoveAt(indexToRemove);
			HandleIndicesOnFaceRemoved(trianglesConnectedToTarget_i, indexToRemove);

			// Add new vertex
			newVertices.Add(vertices.Num());
			vertices.Add(vertexToProcess);
			errorVertices.Add(false);
			unwrappedVertices.Add(false);

			// TODO: delete the whole thing or finish it

		}
	}
	return false;
}

void GeometryMathUtils::UnwrapUV(FSimpleMeshGeometry& g, float scale) {
	TArray<FGeometryVertex>& vertices = g.vertices;
	TArray<FGeometryTriangle>& faces = g.faces;

	TArray<TSet<FVertexIndex>> connectedVerticesMap = {};
	ExtractMapOfVertices(vertices, faces, connectedVerticesMap);

	TArray<TSet<FTriangleIndex>> connectedTrianglesMap = {};
	ExtractMapOfTriangles(vertices, faces, connectedTrianglesMap);


	TArray<bool> unwrappedVertices = {};
	TArray<bool> errorVertices = {};
	for (int j = 0; j < vertices.Num(); j++) {
		errorVertices.Add(connectedVerticesMap[j].Num() <= 1);
		unwrappedVertices.Add(false);
	}

	while (true) {
		bool unwrappedAtLeast1 = false;
		for (int i = 0; i < vertices.Num(); i++) {
			if (unwrappedVertices[i] || errorVertices[i]) {
				continue;
			}
			UnwrapVertexConsideringConnectionsOrCreateNewIfNotPossible(vertices, faces, connectedVerticesMap, connectedTrianglesMap, unwrappedVertices, errorVertices, i, scale);
			unwrappedVertices[i] = true;
			unwrappedAtLeast1 = true;

		}
		if (!unwrappedAtLeast1) {
			bool unwrapped_one_more = false;
			for (int i = 0; i < vertices.Num(); i++) {
				if (!unwrappedVertices[i]) {
					unwrapped_one_more = true;
					FGeometryVertex& vertex1 = vertices[i];
					FVertexIndex vertex2Index = FVertexIndex(0);
					for (FVertexIndex vertex2 : connectedVerticesMap[i]) {
						vertex2Index = vertex2;
					}
					FGeometryVertex& vertex2 = vertices[vertex2Index.i];
					vertex1.u = 0;
					vertex1.v = 0;
					vertex2.u = 0;
					vertex2.v = vertex1.Dist(vertex2) * scale;
					unwrappedVertices[i] = true;
					unwrappedVertices[vertex2Index.i] = true;
					break;
				}
			}
			if (!unwrapped_one_more) {
				return;
			}
		}
	}
}
