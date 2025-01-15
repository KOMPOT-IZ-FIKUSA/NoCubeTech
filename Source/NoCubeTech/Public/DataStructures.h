// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataStructures.generated.h"


USTRUCT()
struct NOCUBETECH_API FIntegerPair {
	GENERATED_BODY()
	UPROPERTY()
	int32 x;
	UPROPERTY()
	int32 y;
public:
	FIntegerPair() : x(0), y(0) {

	}

	FIntegerPair(int32 x, int32 y) : x(x), y(y) {

	}

	friend uint32 GetTypeHash(const FIntegerPair& struc) {
		return ((uint32)(struc.x)) ^ ((uint32)(struc.y));
	}

	bool operator==(const FIntegerPair& Other) const
	{
		return x == Other.x && y == Other.y;
	}

};

USTRUCT()
struct NOCUBETECH_API FActorGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FIntegerPair, TWeakObjectPtr<AActor>> data;

public:

	FActorGrid() {
		data = TMap<FIntegerPair, TWeakObjectPtr<AActor>>();
	};

	void Put(int32 x, int32 y, AActor* actor) {
		if (actor != nullptr) {
			data.Add(FIntegerPair(x, y), TWeakObjectPtr<AActor>(actor));
		}
	}

	TWeakObjectPtr<AActor> Get(int32 x, int32 y) {
		TWeakObjectPtr<AActor>* result = data.Find(FIntegerPair(x, y));
		if (result == nullptr) {
			return nullptr;
		}
		else {
			return *result;
		}
	}

	bool Contains(int32 x, int32 y) {
		FIntegerPair key = FIntegerPair(x, y);
		TWeakObjectPtr<AActor>* result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		else {
			if (result->IsValid()) {
				return true;
			}
			else {
				data.Remove(key);
				return false;
			}
		}
	};

	void CheckAndRemove(int32 x, int32 y) {
		data.Remove(FIntegerPair(x, y));
	}

	auto CreateConstIterator() {
		return data.CreateConstIterator();
	}
};


USTRUCT()
struct NOCUBETECH_API FBoolGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FIntegerPair, bool> data;

public:

	FBoolGrid() {

		data = TMap<FIntegerPair, bool>();
	};

	void Put(int32 x, int32 y, bool value) {
		data.Add(FIntegerPair(x, y), value);
	}

	bool Get(int32 x, int32 y) {
		return data[FIntegerPair(x, y)];
	}

	bool Contains(int32 x, int32 y) {
		FIntegerPair key = FIntegerPair(x, y);
		bool* result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		else {
			return *result;
		}
	};
};

USTRUCT()
struct NOCUBETECH_API FObjectGrid
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FIntegerPair, UObject*> data;

public:

	FObjectGrid() {

		data = TMap<FIntegerPair, UObject*>();
	};

	void Put(int32 x, int32 y, UObject* obj) {
		if (obj == nullptr) {
			data.Remove(FIntegerPair(x, y));
		}
		else {
			data.Add(FIntegerPair(x, y), obj);
		}
	}

	UObject* Get(int32 x, int32 y) {
		UObject** result = data.Find(FIntegerPair(x, y));
		if (result == nullptr) {
			return nullptr;
		}
		else {
			return *result;
		}
	}

	bool ContainsNonNull(int32 x, int32 y) {
		FIntegerPair key = FIntegerPair(x, y);
		UObject** result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		if (*result == nullptr) {
			return false;
		}
		return true;
	};

	void CheckAndRemove(int32 x, int32 y) {
		data.Remove(FIntegerPair(x, y));
	}

	void CheckAndRemove(const FIntegerPair& xyPair) {
		data.Remove(xyPair);
	}

	auto CreateConstIterator() {
		return data.CreateConstIterator();
	}

};

USTRUCT()
struct NOCUBETECH_API FInt64BoolMap {
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<int64, bool> data;
public:

	FInt64BoolMap() {
		data = TMap<int64, bool>();
	};

	void Put(int64 key, bool value) {
		bool* ptr = data.Find(key);
		bool nul = ptr == nullptr;
		if (value) {
			if (nul) {
				data.Add(key, true);
			}
			else {
				*ptr = value;
			}
		}
		else {
			if (nul) {
				// nothing
			}
			else {
				data.Remove(key);
			}
		}
	}

	bool Get(int64 key) {
		bool* result = data.Find(key);
		if (result == nullptr) {
			return false;
		}
		else {
			return *result;
		}
	}

};


USTRUCT()
struct NOCUBETECH_API FVertexIndex {
	GENERATED_BODY()
public:
	UPROPERTY()
	int32 i;

	friend uint32 GetTypeHash(const FVertexIndex& TypeVar) {
		return *((uint32*)&TypeVar.i);
	}

	FVertexIndex(const int32& i)
		: i(i)
	{
	}

	bool operator==(const FVertexIndex& other) const {
		return other.i == i;
	}

	FVertexIndex() = default;
};

USTRUCT()
struct NOCUBETECH_API FGeometryTriangle {
	GENERATED_BODY()
public:
	UPROPERTY()
	FVertexIndex index1;
	UPROPERTY()
	FVertexIndex index2;
	UPROPERTY()
	FVertexIndex index3;

	FGeometryTriangle(const int32& index1, const int32& index2, const int32& index3)
		: index1(index1), index2(index2), index3(index3)
	{
	}

	FGeometryTriangle(const FVertexIndex& index1, const FVertexIndex& index2, const FVertexIndex& index3)
		: index1(index1), index2(index2), index3(index3)
	{
	}

	bool ContainsIndex(FVertexIndex index) {
		return index.i == index1.i || index.i == index2.i || index.i == index3.i;
	}

	// Finds index different from i1 and i2. Returns whether both i1 and i2 are found in the triangle.
	bool GetRemainingIndex(int32 i1, int32 i2, int32& result) {
		if (index1 == i1 && index2 == i2) {
			result = index3.i;
			return true;
		}
		if (index1 == i2 && index2 == i1) {
			result = index3.i;
			return true;
		}
		if (index2 == i2 && index3 == i1) {
			result = index1.i;
			return true;
		}
		if (index2 == i1 && index3 == i2) {
			result = index1.i;
			return true;
		}
		if (index1 == i1 && index3 == i2) {
			result = index2.i;
			return true;
		}
		if (index1 == i2 && index3 == i1) {
			result = index2.i;
			return true;
		}
		return false;
	}

	FGeometryTriangle() = default;

	/*
	* Checks if at least two out of 3 indices for this and the other one are equal
	*/
	bool HasMutualSideWithTriangle(const FGeometryTriangle& other) const {
		int matchedCount = 0;
		if (index1 == other.index1 || index1 == other.index2 || index1 == other.index3) {
			matchedCount += 1;
		}
		if (index2 == other.index1 || index2 == other.index2 || index2 == other.index3) {
			matchedCount += 1;
		}
		if (index3 == other.index1 || index3 == other.index2 || index3 == other.index3) {
			matchedCount += 1;
		}
		return matchedCount >= 2;
	}

private:
	static inline void sortTwoIntegers(int32& a, int32& b) {
		if (a > b) {
			int t;
			t = a;
			a = b;
			b = t;
		}
	}

	static void sortThreeIntegers(int32& a, int32& b, int32& c) {
		sortTwoIntegers(a, b);
		sortTwoIntegers(b, c);
		sortTwoIntegers(a, b);
	}
};

USTRUCT()
struct NOCUBETECH_API FGeometryVertex {
	GENERATED_BODY()
	UPROPERTY()
	double x;
	UPROPERTY()
	double y;
	UPROPERTY()
	double z;
	UPROPERTY()
	double u;
	UPROPERTY()
	double v;

	FGeometryVertex(double x, double y, double z, double u, double v)
		: x(x), y(y), z(z), u(u), v(v)
	{
	}

	FGeometryVertex() = default;

	float Dist(const FGeometryVertex& other) {
		return sqrtf((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
	}

	float DistUV(const FGeometryVertex& other) {
		return sqrtf((u - other.u) * (u - other.u) + (v - other.v) * (v - other.v));
	}


};


USTRUCT()
struct NOCUBETECH_API FGeometryVertexI {
	GENERATED_BODY()
	UPROPERTY()
	double x;
	UPROPERTY()
	double y;
	UPROPERTY()
	double z;
	UPROPERTY()
	double u;
	UPROPERTY()
	double v;
	UPROPERTY()
	FLinearColor color;



	FGeometryVertexI() = default;

	double Dist(const FGeometryVertexI& other) {
		return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
	}

	double DistUV(const FGeometryVertexI& other) {
		return sqrt((u - other.u) * (u - other.u) + (v - other.v) * (v - other.v));
	}


};


struct FProcMeshTangent;


USTRUCT()
struct NOCUBETECH_API FTriangleIndex {
	GENERATED_BODY()
	UPROPERTY()
	int32 i;

	friend uint32 GetTypeHash(const FTriangleIndex& TypeVar) {
		return *((uint32*)&TypeVar.i);
	}

	FTriangleIndex(const int32& i)
		: i(i)
	{
	}

	bool operator==(const FTriangleIndex& other) const {
		return other.i == i;
	}

	FTriangleIndex() = default;
};

USTRUCT()
struct NOCUBETECH_API FSimpleMeshGeometry {
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FGeometryVertex> vertices;
	UPROPERTY()
	TArray<FGeometryTriangle> faces;

	FSimpleMeshGeometry() {
		vertices = TArray<FGeometryVertex>();
		faces = TArray<FGeometryTriangle>();
	}

	void Export(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents);


	// Returns all the faces that belong to connected cluster
	TSet<int32> GetFacesCluster(int32 begginingFaceIndex) {
		// Start with face with index begginingFaceIndex
		TSet<int32> validatedFacesIndices = { begginingFaceIndex };
		// Faces that are not only considered to be a part of the connected set, but also were analyzed in terms of neighbour faces
		TSet<int32> extendedFacesIndices = {};
		while (true) {
			// Set of faces that will be found as parts of connected set
			TSet<int32> newValidFacesIndices = {};
			for (int32 faceIndex : validatedFacesIndices) {
				// If it was already processed, skip it
				if (extendedFacesIndices.Contains(faceIndex)) {
					continue;
				}
				// Take one face that is already a part of connected set
				FGeometryTriangle validFace = faces[faceIndex];
				/*
				Then find faces that have mutual edge with this
				*/
				for (int anyFaceIndex = 0; anyFaceIndex < faces.Num(); anyFaceIndex++) {
					if (faces[anyFaceIndex].HasMutualSideWithTriangle(validFace)) {
						newValidFacesIndices.Add(anyFaceIndex);
					}
				}
				extendedFacesIndices.Add(faceIndex);
			}
			/*
			* Remember the number of elements
			* Copy from newValidFacesIndices to validatedFaceIndices
			* If no new elements, end the loop
			* If new elements added, continue loop execution
			*/
			int prevNum = validatedFacesIndices.Num();
			validatedFacesIndices.Append(newValidFacesIndices);
			if (prevNum == validatedFacesIndices.Num()) {
				return validatedFacesIndices;
			}
		}

	}


	void UnwrapUV(float scale);
private:

	static inline float distanceSqr(float x1, float y1, float x2, float y2) {
		return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);

	}


	static void printTest(double x1, double y1, double x2, double y2, double r1, double r2, FString prefix) {
		FString msg = prefix;
		msg.Appendf(TEXT(" | %f, %f, %f, %f, %f, %f"), x1, y1, x2, y2, r1, r2);
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);
	}


};


USTRUCT()
struct NOCUBETECH_API FArrayOfIndices {
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<int32> indices;

	FArrayOfIndices() {
		indices = {};
	}

};



USTRUCT()
struct NOCUBETECH_API FComplexMeshGeometry {
	GENERATED_BODY()
public:

	FComplexMeshGeometry() {
		vertices = {};
		faces = {};
		validVertices = {};
		validFaces = {};
	}

	FGeometryVertexI& AddVertex() {
		return AddVertex(0, 0, 0, 0, 0);
	}

	FGeometryVertexI& AddVertex(double x, double y, double z, double u = 0, double v = 0, FLinearColor color = FLinearColor::White) {
		FGeometryVertexI& vertex = vertices.Emplace_GetRef();
		vertex.x = x;
		vertex.y = y;
		vertex.z = z;
		vertex.u = u;
		vertex.v = v;
		vertex.color = color;
		validVertices.Add(true);
		connectedFacesMap.Add({});
		return vertex;
	}

	void RemoveVertex(int vertexIndex) {
		// delete vertex
		validVertices[vertexIndex] = false;

		// Delete faces
		for (int faceIndex : connectedFacesMap[vertexIndex].indices) {
			RemoveFace(faceIndex);
		}
	}

	FGeometryTriangle& AddFace(int vertex1, int vertex2, int vertex3) {
		FGeometryTriangle& newFace = faces.Add_GetRef(FGeometryTriangle(vertex1, vertex2, vertex3));
		connectedFacesMap[vertex1].indices.AddUnique(faces.Num() - 1);
		connectedFacesMap[vertex2].indices.AddUnique(faces.Num() - 1);
		connectedFacesMap[vertex3].indices.AddUnique(faces.Num() - 1);
		validFaces.Add(true);
		return newFace;
	}

	void RemoveFace(int faceIndex) {
		validFaces[faceIndex] = false;
		FGeometryTriangle face = faces[faceIndex];
		connectedFacesMap[face.index1.i].indices.Remove(faceIndex);
		connectedFacesMap[face.index2.i].indices.Remove(faceIndex);
		connectedFacesMap[face.index3.i].indices.Remove(faceIndex);
	}


	FGeometryVertexI& CopyVertex(FGeometryVertexI& vertex) {
		return AddVertex(vertex.x, vertex.y, vertex.z, vertex.u, vertex.v);
	}

	static void RollTriangleIndicesForTargetIndex(FGeometryTriangle& triangle, int target) {
		if (triangle.index2 == target) {
			triangle.index2 = triangle.index3;
			triangle.index3 = triangle.index1;
			triangle.index1 = target;
		}
		else if (triangle.index3 == target) {
			triangle.index3 = triangle.index2;
			triangle.index2 = triangle.index1;
			triangle.index1 = target;
		}
	}

	void ScaleUV(double m) {
		for (int i = 0; i < vertices.Num(); i++) {
			if (validVertices[i])
			{
				vertices[i].u *= m;
				vertices[i].v *= m;
			}
		}
	}

	void UnwrapUV() {
		FString msg;
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("dont forget to delete msg"));

		TArray<bool> unwrappedVertices = {};
		for (int j = 0; j < vertices.Num(); j++) {
			unwrappedVertices.Add(false);
		}
		while (true) {

			bool unwrappedAtLeast1 = false;
			for (int i = 0; i < vertices.Num(); i++) {
				if (unwrappedVertices[i] || !validVertices[i]) {
					continue;
				}

				// filter the connected triangles to get only those which need one vertex to be completely unwrapped
				TArray<int> trianglesWithOtherVerticesUnwrapped = {};
				for (int triangleIndex : connectedFacesMap[i].indices) {
					FGeometryTriangle& face = faces[triangleIndex];
					RollTriangleIndicesForTargetIndex(face, i);
					if (!unwrappedVertices[face.index2.i] || !unwrappedVertices[face.index3.i]) {
						continue;
					}
					trianglesWithOtherVerticesUnwrapped.Add(triangleIndex);
				}
				if (trianglesWithOtherVerticesUnwrapped.Num() == 0) {
					continue;
				}
				// Note: all the triangles in trianglesWithOtherVerticesUnwrapped are rolled to make target vertex the first index (triangle.index1 = i)
				// Take one of the almost-unwrapped triangles that contains target vertex
				int triangleToUnwrapVertexFor = trianglesWithOtherVerticesUnwrapped[0];

				FGeometryTriangle& triangleToUnwrap = faces[triangleToUnwrapVertexFor];
				FGeometryVertexI& unwrapped1 = vertices[triangleToUnwrap.index2.i];
				FGeometryVertexI& unwrapped2 = vertices[triangleToUnwrap.index3.i];
				FGeometryVertexI& target = vertices[i];

				if (!CheckTwoVerticesCorreclyUnwrapped(unwrapped1, unwrapped2)) {
					GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Incorrectly unwrapped vertices pair! Removing vertex..."));
					RemoveVertex(triangleToUnwrap.index2.i);


					//msg = TEXT("");
					//msg = msg.Appendf(TEXT("Removed vertex %d because it makes incorrect uv with %d in triangle %d"), triangleToUnwrap.index2.i, triangleToUnwrap.index3.i, triangleToUnwrapVertexFor);
					//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);

					continue;
				}

				// Look for the "opposite" vertex in the neighbour triangles
				// "opposite" definition: A vertex that is connected to two other vertices, that are connected to the target.
				// opposite should be different from the target
				// opposite should be unwrapped
				FGeometryVertexI& opposite = unwrapped1;
				for (int conectedTriangleIndex : connectedFacesMap[i].indices) {
					FGeometryTriangle& triangle = faces[conectedTriangleIndex];
					int potentialOppositeIndex = 0;
					bool oppositeFound = triangle.GetRemainingIndex(triangleToUnwrap.index2.i, triangleToUnwrap.index3.i, potentialOppositeIndex);
					if (oppositeFound && unwrappedVertices[potentialOppositeIndex] && potentialOppositeIndex != i) {
						opposite = vertices[potentialOppositeIndex];
						break;
					}
				}

				if (target.Dist(unwrapped1) < 1e-5) {
					target.u = unwrapped1.u;
					target.v = unwrapped1.v;
				}
				else if (target.Dist(unwrapped2) < 1e-5) {
					target.u = unwrapped2.u;
					target.v = unwrapped2.v;
				}
				else {
					bool success = UnwrapVertex(unwrapped1, unwrapped2, opposite, target);
					if (!success) {
						GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot unwrap vertex! Removing vertex..."));
						RemoveVertex(i);
						continue;
					}
				}


				if (!CheckTwoVerticesCorreclyUnwrapped(target, unwrapped2)) {

					msg = TEXT("");
					msg = msg.Appendf(TEXT("Error %f %f"), target.Dist(unwrapped2), target.DistUV(unwrapped2));
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);
					return;
				}

				unwrappedVertices[i] = true;
				unwrappedAtLeast1 = true;

				//msg = TEXT("");
				//msg = msg.Appendf(TEXT("Unwrapped vertex %d"), i);
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);

				// Iterate over connected triangles and check whether new uv is valid for them
				// All the "invalid" triangles should be re-connected to a copy of the target, which is set as not unwrapped (completely new vertex)
				// Validity criterion: UV distance must be equal 3D distance (difference less than epsilon)
				TArray<int> invalidTriangles = {};
				for (int connectedTriangleIndex : connectedFacesMap[i].indices) {
					FGeometryTriangle& faceToValidate = faces[connectedTriangleIndex];
					RollTriangleIndicesForTargetIndex(faceToValidate, i);
					FGeometryVertexI& vertex2 = vertices[faceToValidate.index2.i];
					FGeometryVertexI& vertex3 = vertices[faceToValidate.index3.i];
					if ( // if it is unwrapped and the distance is invalid
						(unwrappedVertices[faceToValidate.index2.i] && !CheckTwoVerticesCorreclyUnwrapped(target, vertex2)) ||
						(unwrappedVertices[faceToValidate.index3.i] && !CheckTwoVerticesCorreclyUnwrapped(target, vertex3)) 
						) {
						invalidTriangles.Add(connectedTriangleIndex);
						//msg = TEXT("");
						//msg = msg.Appendf(TEXT("Found invalid triangle %d (%d %d %d)"), connectedTriangleIndex, i, faceToValidate.index2.i, faceToValidate.index3.i);
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);
					}
				}


				// If there are invalid triangles, add a copy of this vertex and connect all of the invalid triangles to that copy
				if (invalidTriangles.Num() > 0) {
					CopyVertex(target);
					int copyIndex = vertices.Num() - 1;
					//msg = TEXT("");
					//msg = msg.Appendf(TEXT("Copied vertex %d %d"), i, copyIndex);
					//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);
					unwrappedVertices.Add(false);
					for (int invalidTriangleIndex : invalidTriangles) {
						// still rolled; still index1 =  i
						// 1) Reconnect face
						faces[invalidTriangleIndex].index1 = copyIndex;
						// 2) Adjust mapping
						connectedFacesMap[copyIndex].indices.AddUnique(invalidTriangleIndex);
						connectedFacesMap[i].indices.Remove(invalidTriangleIndex);

						//msg = TEXT("");
						//msg = msg.Appendf(TEXT("Connected triangle %d from %d to %d"), invalidTriangleIndex, i, copyIndex);
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);
					}
				}
			}

			if (!unwrappedAtLeast1) {
				bool unwrappedOneMore = false;
				for (int i = 0; i < vertices.Num(); i++) {
					if (unwrappedVertices[i] || !validVertices[i]) {
						continue;
					}
					if (connectedFacesMap[i].indices.Num() == 0) {
						continue;
					}

					unwrappedOneMore = true;
					FGeometryVertexI& vertex1 = vertices[i];
					FGeometryTriangle& faceToBeginUnwrap = faces[connectedFacesMap[i].indices[0]];

					RollTriangleIndicesForTargetIndex(faceToBeginUnwrap, i);
					FGeometryVertexI& vertex2 = vertices[faceToBeginUnwrap.index2.i];
					check(!unwrappedVertices[faceToBeginUnwrap.index2.i]);
					vertex1.u = 0;
					vertex1.v = 0;
					vertex2.u = 0;
					vertex2.v = vertex1.Dist(vertex2);
					unwrappedVertices[i] = true;
					unwrappedVertices[faceToBeginUnwrap.index2.i] = true;
					GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Unwrapped 2 vertices (2)"));

					break;
				}
				if (!unwrappedOneMore) {
					//msg = TEXT("");
					//int unwrappedCount = 0;
					//for (bool s : unwrappedVertices) {
					//	if (s) {
					//		unwrappedCount += 1;
					//	}
					//}
					//msg = msg.Appendf(TEXT("Unwrapped vertices: %d All vertices: %d"), unwrappedCount, vertices.Num());
					//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);
					return;
				}
			}
			/*
			if (true) {
				FString msg = TEXT("");
				msg = msg.Appendf(TEXT("Debug iteration: %d\nVertices: [ "), debugIteration);
				for (int i = 0; i < vertices.Num(); i++) {
					if (validVertices[i]) {
						FGeometryVertexI& v = vertices[i];
						msg = msg.Appendf(TEXT("%f %f %f %f %f |\n"), v.x, v.y, v.z, v.u, v.v);
					}
				}
				int unwrappedNum = 0;
				for (bool unw : unwrappedVertices) {
					if (unw) {
						unwrappedNum += 1;
					}
				}
				msg = msg.Appendf(TEXT("]\nUnwrapped vertices: %d"), unwrappedNum);
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);
			}
			debugIteration += 1;
			if (debugIteration >= 2) {
				return;
			}
			*/
		}
	}

	void Export(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents);



private:
	UPROPERTY()
	TArray<FGeometryVertexI> vertices;
	UPROPERTY()
	TArray<bool> validVertices;

	UPROPERTY()
	TArray<FGeometryTriangle> faces;
	UPROPERTY()
	TArray<bool> validFaces;

	UPROPERTY()
	TArray<FArrayOfIndices> connectedFacesMap;

	//UPROPERTY()
	//TArray<>


	static bool CheckTwoVerticesCorreclyUnwrapped(FGeometryVertexI& vertex1, FGeometryVertexI& vertex2) {
		double distance = vertex1.Dist(vertex2);
		double distanceUV = vertex1.DistUV(vertex2);
		const double eps = 1e-3;
		double min_ = fmin(distance, distanceUV);
		double max_ = fmax(distance, distanceUV);
		return (max_ - min_ < eps) || ((max_ - min_) / max_ < eps);
	}

	static bool UnwrapVertex(FGeometryVertexI& mutual1, FGeometryVertexI& mutual2, FGeometryVertexI& unique_opposite, FGeometryVertexI& target) {
		double resultX1 = 0;
		double resultY1 = 0;
		double resultX2 = 0;
		double resultY2 = 0;

		double r1 = target.Dist(mutual1);
		double r2 = target.Dist(mutual2);

		double eps = 1e-4;


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

USTRUCT()
struct NOCUBETECH_API FColoredMeshGeometry {
	GENERATED_BODY()
public:

	FColoredMeshGeometry() {
		vertices = {};
		faces = {};
		facesColors = {};
		validVertices = {};
		validFaces = {};
	}

	FVector3d& AddVertex() {
		return AddVertex(0, 0, 0);
	}

	FVector3d& AddVertex(double x, double y, double z) {
		FVector3d& vertex = vertices.Emplace_GetRef();
		vertex.X = x;
		vertex.Y = y;
		vertex.Z = z;
		validVertices.Add(true);
		connectedFacesMap.Add({});
		return vertex;
	}

	void RemoveVertex(int vertexIndex) {
		// delete vertex
		validVertices[vertexIndex] = false;

		// Delete faces
		for (int faceIndex : connectedFacesMap[vertexIndex].indices) {
			RemoveFace(faceIndex);
		}
	}

	void AddFace(int vertex1, int vertex2, int vertex3, float r, float g, float b) {
		FGeometryTriangle& newFace = faces.Add_GetRef(FGeometryTriangle(vertex1, vertex2, vertex3));
		connectedFacesMap[vertex1].indices.AddUnique(faces.Num() - 1);
		connectedFacesMap[vertex2].indices.AddUnique(faces.Num() - 1);
		connectedFacesMap[vertex3].indices.AddUnique(faces.Num() - 1);
		facesColors.Add(FLinearColor(r, g, b));
		validFaces.Add(true);
	}

	void RemoveFace(int faceIndex) {
		validFaces[faceIndex] = false;
		FGeometryTriangle face = faces[faceIndex];
		connectedFacesMap[face.index1.i].indices.Remove(faceIndex);
		connectedFacesMap[face.index2.i].indices.Remove(faceIndex);
		connectedFacesMap[face.index3.i].indices.Remove(faceIndex);
	}


	FVector& CopyVertex(FGeometryVertexI& vertex) {
		return AddVertex(vertex.x, vertex.y, vertex.z);
	}

	static void RollTriangleIndicesForTargetIndex(FGeometryTriangle& triangle, int target) {
		if (triangle.index2 == target) {
			triangle.index2 = triangle.index3;
			triangle.index3 = triangle.index1;
			triangle.index1 = target;
		}
		else if (triangle.index3 == target) {
			triangle.index3 = triangle.index2;
			triangle.index2 = triangle.index1;
			triangle.index1 = target;
		}
	}


	void Export(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents);

	void FixSparsity() {
		int index = 0;
		TArray<FVector> newVertices = {};
		TArray<FArrayOfIndices> newConnectedFacesMap = {};
		TArray<int> indicesMapping = {}; // mapping from every vertex from data structure to vertices in a non-sparse array
		for (int i = 0; i > vertices.Num(); i++) {
			if (validVertices[i]) {
				indicesMapping.Add(index);
				index++;
				newVertices.Add(vertices[i]);
				newConnectedFacesMap.Add(connectedFacesMap[i]);
			}
			else {
				indicesMapping.Add(-1);
			}
		}


		TArray<FGeometryTriangle> newFaces = {};
		TArray<FLinearColor> newFacesColors = {};
		for (int i = 0; i < faces.Num(); i++) {
			if (validFaces[i]) {
				newFaces.Add(faces[i]);
				newFacesColors.Add(facesColors[i]);
			}
		}
		
		faces = newFaces;
		facesColors = newFacesColors;
		vertices = newVertices;
		connectedFacesMap = newConnectedFacesMap;
		bool true_ = true;
		validVertices = TArray<bool>(&true_, vertices.Num());
		validFaces = TArray<bool>(&true_, faces.Num());
	}


private:
	UPROPERTY()
	TArray<FVector> vertices;
	UPROPERTY()
	TArray<bool> validVertices;

	UPROPERTY()
	TArray<FGeometryTriangle> faces;
	UPROPERTY()
	TArray<FLinearColor> facesColors;
	UPROPERTY()
	TArray<bool> validFaces;

	UPROPERTY()
	TArray<FArrayOfIndices> connectedFacesMap;

	static FVector2D CalculateUVForFace(int faceIndex, int textureSize) {
		float u = ((float)(faceIndex % textureSize)) / textureSize;
		float v = ((float)(faceIndex / textureSize)) / textureSize;
		return FVector2D(u, v);
	}

	static int CalculateTextureSize(int facesCount) {
		int size = 1;
		while (size * size < facesCount) {
			size *= 2;
		}
		return size;
	}

};

