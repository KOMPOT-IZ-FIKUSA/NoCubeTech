// Fill out your copyright notice in the Description page of Project Settings.


#include "DataStructures.h"
#include "ProceduralMeshComponent.h"
#include "GeometryMathUtils.h"


void FSimpleMeshGeometry::Export(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents) {
	for (FGeometryVertex& vertex : vertices) {
		Vertices.Add(FVector(vertex.x, vertex.y, vertex.z));
		Normals.Add(FVector(0, 0, 1));
		UV0.Add(FVector2D(vertex.u, vertex.v));
		VertexColors.Add(FColor::White);
		Tangents.Add(FProcMeshTangent());
	}
	for (FGeometryTriangle& triangle : faces) {
		Triangles.Add(triangle.index1.i);
		Triangles.Add(triangle.index2.i);
		Triangles.Add(triangle.index3.i);
		if (false) {
			Triangles.Add(triangle.index1.i);
			Triangles.Add(triangle.index3.i);
			Triangles.Add(triangle.index2.i);
		}
	}
}
void FComplexMeshGeometry::Export(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents) {
	for (FGeometryVertexI& vertex : vertices) {
		Vertices.Add(FVector(vertex.x, vertex.y, vertex.z));
		Normals.Add(FVector(0, 0, 1));
		UV0.Add(FVector2D(vertex.u, vertex.v));
		VertexColors.Add(vertex.color);
		Tangents.Add(FProcMeshTangent());
	}
	for (int i = 0; i < faces.Num(); i++) {
		if (!validFaces[i]) {
			continue;
		}
		FGeometryTriangle& triangle = faces[i];
		Triangles.Add(triangle.index1.i);
		Triangles.Add(triangle.index2.i);
		Triangles.Add(triangle.index3.i);
		if (false) {
			Triangles.Add(triangle.index1.i);
			Triangles.Add(triangle.index3.i);
			Triangles.Add(triangle.index2.i);
		}
	}
}
void FColoredMeshGeometry::Export(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents) {
	FixSparsity();
	int textureSize = CalculateTextureSize(faces.Num());
	for (int faceIndex = 0; faceIndex < faces.Num(); faceIndex++) {
		FGeometryTriangle& face = faces[faceIndex];
		Vertices.Add(vertices[face.index1.i]);
		Vertices.Add(vertices[face.index2.i]);
		Vertices.Add(vertices[face.index3.i]);
		Normals.Add(FVector(0, 0, 1));
		Normals.Add(FVector(0, 0, 1));
		Normals.Add(FVector(0, 0, 1));
		FVector2D uv = CalculateUVForFace(faceIndex, textureSize);
		Triangles.Add(Vertices.Num() - 1);
		Triangles.Add(Vertices.Num() - 2);
		Triangles.Add(Vertices.Num() - 3);
		UV0.Add(uv);
		UV0.Add(uv);
		UV0.Add(uv);
		Tangents.Add(FProcMeshTangent());
		Tangents.Add(FProcMeshTangent());
		Tangents.Add(FProcMeshTangent());
		VertexColors.Add(FColor::White);
		VertexColors.Add(FColor::White);
		VertexColors.Add(FColor::White);
	}

}

void FSimpleMeshGeometry::UnwrapUV(float scale) {
	GeometryMathUtils::UnwrapUV(*this, scale);

}