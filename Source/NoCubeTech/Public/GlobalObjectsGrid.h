// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorListGrid.h"
#include "RandomGenerator.h"
#include "GlobalObjectsGrid.generated.h"


UCLASS()
class NOCUBETECH_API URectTest : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
	float x0;
	UPROPERTY()
	float y0;
	UPROPERTY()
	float x1;
	UPROPERTY()
	float y1;
};


UCLASS()
class NOCUBETECH_API AGlobalObjectsGrid : public AActor
{
	GENERATED_BODY()
private:
	
	TArray<ActorListGridMultiple> grids = {};

	void test();
	void test1();


public:
	// Sets default values for this actor's properties
	AGlobalObjectsGrid();

	static const int LANDSCAPE_INDEX	= 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	StorableObjectReference StartTrackingObject(int gridId, UObject* object, float x0, float y0, float x1, float y1) {
		return grids[gridId].RegisterObject(object, x0, y0, x1, y1);
	}

	StorableObjectReference UpdateTrackingObject(int gridId, StorableObjectReference object, float x0, float y0, float x1, float y1) {
		return grids[gridId].UpdateObject(object, x0, y0, x1, y1);
	}

	void StopTrackingObject(int gridId, StorableObjectReference object) {
		grids[gridId].RemoveObject(object);
	}

	void FindObjects(int gridId, float x0, float y0, float x1, float y1, TArray<TWeakObjectPtr<UObject>>& result) {
		grids[gridId].GetObjectsInRect(x0, y0, x1, y1, result);
	}

};
