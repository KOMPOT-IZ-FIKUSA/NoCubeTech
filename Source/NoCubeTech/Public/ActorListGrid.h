// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


//non-serializable
struct NOCUBETECH_API StorableObjectReference {
	friend struct ActorListGridMultiple;
private:
	// Reference to uobject. Might be invalid.
	TWeakObjectPtr<UObject> reference;

	// Maximum object horizontal dimension. (axis aligned width1 or width2)
	float sizeX, sizeY;

	float size() {
		return sizeX > sizeY ? sizeX : sizeY;
	}

	// Object center position
	float x, y;

	// -1 if global objects (no grid), otherwize valid index
	int boundGridIndex;

	// nullptr of global objects (no grid), otherwize valid ptr
	struct ObjectCluster* boundClusterPtr;

	StorableObjectReference(const TWeakObjectPtr<UObject>& reference, float sizeX, float sizeY, float boundX, float boundY, int boundGridIndex, ObjectCluster* boundClusterPtr)
		: reference(reference), sizeX(sizeX), sizeY(sizeY), x(boundX), y(boundY), boundGridIndex(boundGridIndex), boundClusterPtr(boundClusterPtr)
	{
	}

public:
	bool operator==(const StorableObjectReference& other) const {
		return reference.Get() == other.reference.Get();
	}

	UObject* GetObjPtr() {
		return reference.Get();
	}


	StorableObjectReference() : reference(nullptr), sizeX(0), sizeY(0), x(0), y(0), boundGridIndex(-2), boundClusterPtr(nullptr) {
		
	}

	bool IntersectsBox(float x0, float y0, float x1, float y1) {
		return x + sizeX / 2 >= x0 && x - sizeX / 2 <= x1 && y + sizeY / 2 >= y0 && y - sizeY / 2 <= y1;
	}

};


//non-serializable
struct NOCUBETECH_API ObjectCluster {
private:
	TArray<TArray<StorableObjectReference>> data;
	int sideSize;
	int numberOfActors;


public:
	ObjectCluster* left = nullptr;
	ObjectCluster* right = nullptr;
	ObjectCluster* top = nullptr;
	ObjectCluster* bottom = nullptr;

	ObjectCluster(int sideSize) : data({}), sideSize(sideSize), numberOfActors(0) {
		data.Init({}, sideSize * sideSize);
	}

	void Add(int cellX, int cellY, StorableObjectReference object) {
		data[cellY * sideSize + cellX].Add(object);
		numberOfActors += 1;
	}

	void Remove(int cellX, int cellY, StorableObjectReference object) {
		data[cellY * sideSize + cellX].RemoveSingle(object);
		numberOfActors -= 1;
	}


	bool IsEmpty() {
		return numberOfActors == 0;
	}

	TArray<StorableObjectReference>& GetCell(int cellX, int cellY) {
		return data[cellY * sideSize + cellX];
	}

};

// Non-serializable
struct NOCUBETECH_API ObjectListGridSingle {
private:
	TMap<FInt32Vector2, ObjectCluster> clusters;
	int cellsInClusterSide;
	float clusterSize;
	float cellSize;

	ObjectCluster* CreateClusterAndBindToNeighbors(FInt32Vector2 index) {
		ObjectCluster& cluster = clusters.Add(index, ObjectCluster(cellsInClusterSide));
		ObjectCluster* left = clusters.Find(FInt32Vector2(index.X - 1, index.Y));
		ObjectCluster* top = clusters.Find(FInt32Vector2(index.X, index.Y + 1));
		ObjectCluster* right = clusters.Find(FInt32Vector2(index.X + 1, index.Y));
		ObjectCluster* bottom = clusters.Find(FInt32Vector2(index.X, index.Y - 1));


		if (left != nullptr) {
			cluster.left = left;
			left->right = &cluster;
		}

		if (top != nullptr) {
			cluster.top = top;
			top->bottom = &cluster;
		}

		if (right != nullptr) {
			cluster.right = right;
			right->left = &cluster;
		}

		if (bottom != nullptr) {
			cluster.bottom = bottom;
			bottom->top = &cluster;
		}
		return &cluster;
	}

public:


	inline FInt32Vector2 positionToClusterIndex(float x, float y) {
		return FInt32Vector2(floor(x / clusterSize), floor(y / clusterSize));
	}

	inline FInt32Vector2 cellIndexToCellInClusterIndex(int x, int y) {
		return FInt32Vector2(
			((x % cellsInClusterSide) + cellsInClusterSide) % cellsInClusterSide,
			((y % cellsInClusterSide) + cellsInClusterSide) % cellsInClusterSide
		);
	}

	inline FInt32Vector2 cellAbsoluteIndexToClusterIndex(int x, int y) {
		return FInt32Vector2(floor((float)x / cellsInClusterSide), floor((float)y / cellsInClusterSide));
	}

	inline FInt32Vector2 positionToCellAbsoluteIndex(float x, float y) {
		return FInt32Vector2(floor(x / cellSize), floor(y / cellSize));
	}

	inline FInt32Vector2 positionToCellIndexInCluster(float x, float y) {
		FInt32Vector2 cellIndex = positionToCellAbsoluteIndex(x, y);
		return cellIndexToCellInClusterIndex(cellIndex.X, cellIndex.Y);
	}

	float GetCellSize() {
		return cellSize;
	}

	ObjectListGridSingle(int cellsInCluster, float cellSize) : clusters({}), cellsInClusterSide(cellsInCluster), clusterSize(cellsInCluster* cellSize), cellSize(cellSize) {

	}

	ObjectCluster* GetCluster(FInt32Vector2 index, bool createIfNotExists) {
		ObjectCluster* cluster = clusters.Find(index);
		if (cluster == nullptr && createIfNotExists) {
			return CreateClusterAndBindToNeighbors(index);
		}
		return cluster;
	}

	static inline int maxI(int a, int b) {
		return a > b ? a : b;
	}

	static inline int minI(int a, int b) {
		return a < b ? a : b;
	}

	void GetObjectsInRect(float x0, float y0, float x1, float y1, TArray<TWeakObjectPtr<UObject>>& result) {

		// Search area cells
		int cellMinX = floor(x0 / cellSize);
		int cellMaxX = ceil(x1 / cellSize) - 1;
		int cellMinY = floor(y0 / cellSize);
		int cellMaxY = ceil(y1 / cellSize) - 1;

		// Increase the searching widow to consider objects that take more than one cell
		cellMinX -= 1;
		cellMaxX += 1;
		cellMinY -= 1;
		cellMaxY += 1;

		FInt32Vector2 minClusterIndex = cellAbsoluteIndexToClusterIndex(cellMinX, cellMinY);
		FInt32Vector2 maxClusterIndex = cellAbsoluteIndexToClusterIndex(cellMaxX, cellMaxY);


		for (int clusterY = minClusterIndex.Y; clusterY <= maxClusterIndex.Y; clusterY++) {
			for (int clusterX = minClusterIndex.X; clusterX <= maxClusterIndex.X; clusterX++) {

				ObjectCluster* cluster = clusters.Find(FInt32Vector2(clusterX, clusterY));
				if (cluster == nullptr) {
					continue;
				}
				// Cluster cells boundings
				int clusterCellsMinX = clusterX * cellsInClusterSide;
				int clusterCellsMaxX = clusterCellsMinX + cellsInClusterSide - 1;
				int clusterCellsMinY = clusterY * cellsInClusterSide;
				int clusterCellsMaxY = clusterCellsMinY + cellsInClusterSide - 1;


				// Intersection between cluster cells and search area cells
				int interLeft = maxI(cellMinX, clusterCellsMinX);
				int interRight = minI(cellMaxX, clusterCellsMaxX);
				int interBottom = maxI(cellMinY, clusterCellsMinY);
				int interTop = minI(cellMaxY, clusterCellsMaxY);

				for (int cellAbsoluteX = interLeft; cellAbsoluteX <= interRight; cellAbsoluteX++) {
					for (int cellAbsoluteY = interBottom; cellAbsoluteY <= interTop; cellAbsoluteY++) {
						FInt32Vector2 indexInCluster = cellIndexToCellInClusterIndex(cellAbsoluteX, cellAbsoluteY);
						for (StorableObjectReference& object : cluster->GetCell(indexInCluster.X, indexInCluster.Y)) {
							UObject* object1 = object.GetObjPtr();
							if (object1 != nullptr && object.IntersectsBox(x0, y0, x1, y1)) {
								result.Add(object1);
							}
						}
					}
				}
			}
		}
	}

};

// Non-serializable
struct NOCUBETECH_API ActorListGridMultiple
{
private:
	TArray<ObjectListGridSingle> grids;
	TArray<StorableObjectReference> global;

	const int cellsInClusterSide = 32;

	int getGridIndex(float size) {
		for (int i = 0; i < grids.Num(); i++) {
			if (grids[i].GetCellSize() >= size) {
				return i;
			}
		}
		return -1;
	}

public:

	ActorListGridMultiple(float cellSizeMin, float cellSizeMax) : grids({}), global({}) {
		for (float cellSize = cellSizeMin; cellSize <= cellSizeMax; cellSize *= 2) {
			grids.Add(ObjectListGridSingle(cellsInClusterSide, cellSize));
		}
	}

	StorableObjectReference RegisterObject(UObject* obj, float x0, float y0, float x1, float y1) {
		check(x1 >= x0);
		check(y1 >= y0);
		check(obj != nullptr);

		GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Green, TEXT("Register object"));

		float x = (x0 + x1) / 2;
		float y = (y0 + y1) / 2;
		float sizeX = x1 - x0;
		float sizeY = y1 - y0;
		float sizeMax = fmaxf(sizeX, sizeY);

		int gridIndex = getGridIndex(sizeMax);

		if (gridIndex == -1) {
			StorableObjectReference result = StorableObjectReference(obj, sizeX, sizeY, x, y, -1, nullptr);
			global.Add(result);
			return result;
		}
		else {

			ObjectListGridSingle& grid = grids[gridIndex];
			FInt32Vector2 clusterIndexInGrid = grid.positionToClusterIndex(x, y);
			FInt32Vector2 cellIndexInCluster = grid.positionToCellIndexInCluster(x, y);

			ObjectCluster* cluster = grid.GetCluster(clusterIndexInGrid, true);
			StorableObjectReference result = StorableObjectReference(obj, sizeX, sizeY, x, y, gridIndex, cluster);
			cluster->Add(cellIndexInCluster.X, cellIndexInCluster.Y, result);
			return result;
		}


	}

	StorableObjectReference UpdateObject(StorableObjectReference obj, float x0, float y0, float x1, float y1) {
		check(x1 >= x0);
		check(y1 >= y0);
		check(obj.reference.Get() != nullptr);

		float x = (x0 + x1) / 2;
		float y = (y0 + y1) / 2;
		float sizeX = x1 - x0;
		float sizeY = y1 - y0;
		float sizeMax = fmaxf(sizeX, sizeY);

		int newGridIndex = getGridIndex(sizeMax);

		if (newGridIndex != obj.boundGridIndex) {
			// Remove from old grid
			if (obj.boundGridIndex == -1) {
				global.RemoveSingle(obj);
			}
			else {
				ObjectListGridSingle& grid = grids[obj.boundGridIndex];
				FInt32Vector2 cellIndexInCluster = grid.positionToCellIndexInCluster(x, y);
				obj.boundClusterPtr->Remove(cellIndexInCluster.X, cellIndexInCluster.Y, obj);
			}
			// Add to new grid
			return RegisterObject(obj.reference.Get(), x0, y0, x1, y1);
		}
		else {
			// same grid
			if (obj.boundGridIndex == -1) {
				for (StorableObjectReference& obj1 : global) {
					if (obj1.GetObjPtr() == obj.GetObjPtr()) {
						obj1.x = obj.x;
						obj1.y = obj.y;
						obj1.sizeX = obj.sizeX;
						obj1.sizeY = obj.sizeY;
						return obj1;
					}
				}
				check(false);
				return StorableObjectReference(); // return invalid
			}
			else {
				// Different grid
				ObjectListGridSingle& grid = grids[obj.boundGridIndex];
				FInt32Vector2 oldClusterIndex = grid.positionToClusterIndex(obj.x, obj.y);
				FInt32Vector2 oldCellIndexInCluster = grid.positionToCellIndexInCluster(obj.x, obj.y);
				FInt32Vector2 newClusterIndex = grid.positionToClusterIndex(y, x);
				FInt32Vector2 newCellIndexInCluster = grid.positionToCellIndexInCluster(x, y);
				if (oldClusterIndex == newClusterIndex) {
					// same cluster, same cell
					if (oldCellIndexInCluster == newCellIndexInCluster) {
						for (StorableObjectReference& obj1 : obj.boundClusterPtr->GetCell(oldCellIndexInCluster.X, oldCellIndexInCluster.Y)) {
							if (obj.reference.Get() == obj1.reference.Get()) {
								obj1.x = x;
								obj1.y = y;
								obj1.sizeX = sizeX;
								obj1.sizeY = sizeY;
								return obj1;
							}
						}
						check(false);
						return StorableObjectReference(); // return invalid
					}
					else {
						// same cluster, different cell
						obj.boundClusterPtr->GetCell(oldCellIndexInCluster.X, oldCellIndexInCluster.Y).RemoveSingle(obj);
						obj.x = x;
						obj.y = y;
						obj.sizeX = sizeX;
						obj.sizeY = sizeY;
						obj.boundClusterPtr->Add(oldCellIndexInCluster.X, oldCellIndexInCluster.Y, obj);
						return obj;
					}
				}
				else {
					// different cluster
					FInt32Vector2 deltaCluster = newClusterIndex - oldClusterIndex;
					ObjectCluster* newCluster = nullptr;
					if (deltaCluster == FInt32Vector2(-1, 0)) {
						newCluster = obj.boundClusterPtr->left;
					}
					else if (deltaCluster == FInt32Vector2(1, 0)) {
						newCluster = obj.boundClusterPtr->right;
					}
					else if (deltaCluster == FInt32Vector2(0, 1)) {
						newCluster = obj.boundClusterPtr->top;
					}
					else if (deltaCluster == FInt32Vector2(0, -1)) {
						newCluster = obj.boundClusterPtr->bottom;
					}
					if (newCluster == nullptr) {
						newCluster = grid.GetCluster(newClusterIndex, true);
					}
					obj.x = x;
					obj.y = y;
					obj.sizeX = sizeX;
					obj.sizeY = sizeY;
					obj.boundClusterPtr = newCluster;
					newCluster->Add(oldCellIndexInCluster.X, oldCellIndexInCluster.Y, obj);
					return obj;
				}
			}

		}
	}

	void RemoveObject(StorableObjectReference object) {
		if (object.boundGridIndex == -1) {
			global.RemoveSingle(object);
		}
		else {
			FInt32Vector2 cellIndexInCluster = grids[object.boundGridIndex].positionToCellIndexInCluster(object.x, object.y);
			object.boundClusterPtr->Remove(cellIndexInCluster.X, cellIndexInCluster.Y, object);
		}
	}

	void GetObjectsInRect(float x0, float y0, float x1, float y1, TArray<TWeakObjectPtr<UObject>>& result) {



		for (StorableObjectReference& storable : global) {
			if (storable.IntersectsBox(x0, y0, x1, y1)) {
				result.Add(storable.reference);
			}
		}
		for (ObjectListGridSingle& grid : grids) {
			grid.GetObjectsInRect(x0, y0, x1, y1, result);
			
		}
	}


};
