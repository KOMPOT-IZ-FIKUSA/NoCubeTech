// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalObjectsGrid.h"

// Sets default values
AGlobalObjectsGrid::AGlobalObjectsGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	grids = {};
}


void AGlobalObjectsGrid::test1() {
	
	URectTest* rect1 = NewObject<URectTest>();
	rect1->x0 = -1003912;
	rect1->y0 = -675828;
	rect1->x1 = -982903;
	rect1->y1 = -594801;
	//StorableObjectReference ref1 = StartTrackingObject(0, rect1, rect1->x0, rect1->y0, rect1->x1, rect1->y1);

	URectTest* rect2 = NewObject<URectTest>();
	rect2->x0 = -984924;
	rect2->y0 = -683378;
	rect2->x1 = -979491;
	rect2->y1 = -673921;
	StorableObjectReference ref2 = StartTrackingObject(0, rect2, rect2->x0, rect2->y0, rect2->x1, rect2->y1);
	
	TArray<TWeakObjectPtr<UObject>> foundObjects;
	foundObjects = {};

	FindObjects(0, rect1->x0, rect1->y0, rect1->x1, rect1->y1, foundObjects);
	if (foundObjects.Num() == 0) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Second not found for first"));
	}

	/*
	foundObjects = {};
	FindObjects(0, rect2->x0, rect2->y0, rect2->x1, rect2->y1, foundObjects);
	if (foundObjects.Num() == 0) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Second not found for second"));
	}
	*/

}
void AGlobalObjectsGrid::test() {

	TArray<URectTest*> allBoxes = {};
	for (int i = 0; i < 1000; i++) {


		// max size of 1000 meters
		float sizeX = RandomGenerator::IntToFloat(i) * 100000 + 10;
		float sizeY = RandomGenerator::IntToFloat(i * 2) * 100000 + 10;
		// position from -10km to 10km
		float posX = RandomGenerator::IntToFloat(i * 3) * 2000000 - 1000000;
		float posY = RandomGenerator::IntToFloat(i * 4) * 2000000 - 1000000;

		if (posX - sizeX / 2 == posX + sizeX / 2 || posY - sizeY / 2 == posY + sizeY / 2) {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Test failed: invalid input data"));
			return;
		}

		URectTest* rect = NewObject<URectTest>();
		rect->x0 = posX - sizeX / 2;
		rect->y0 = posY - sizeY / 2;
		rect->x1 = posX + sizeX / 2;
		rect->y1 = posY + sizeY / 2;
		allBoxes.Add(rect);

		StorableObjectReference ref = StartTrackingObject(0, rect, rect->x0, rect->y0, rect->x1, rect->y1);
	}

	FString msg1;

	int totalChecks = 0;
	int errors = 0;

	for (int index1 = 0; index1 < allBoxes.Num(); index1++) {
		for (int index2 = 0; index2 < allBoxes.Num(); index2++) {
			URectTest* box1 = allBoxes[index1];
			URectTest* box2 = allBoxes[index2];

			float intersectionXMin = fmaxf(box1->x0, box2->x0);
			float intersectionYMin = fmaxf(box1->y0, box2->y0);
			float intersectionXMax = fminf(box1->x1, box2->x1);
			float intersectionYMax = fminf(box1->y1, box2->y1);

			bool trueIntersection = false;
			if (intersectionXMax > intersectionXMin && intersectionYMax > intersectionYMin) {
				trueIntersection = true;
			}

			bool intersectionByGrid = false;
			TArray<TWeakObjectPtr<UObject>> foundObjects = {};
			FindObjects(0, box1->x0, box1->y0, box1->x1, box1->y1, foundObjects);

			
			for (TWeakObjectPtr<UObject> foundRect : foundObjects) {
				if (foundRect.Get() == box2) {
					intersectionByGrid = true;
					break;
				}
			}
			
			totalChecks += 1;
			if (intersectionByGrid != trueIntersection) {
				errors += 1;

				if (errors == 1) {
					msg1 = TEXT("");
					msg1 = msg1.Appendf(TEXT("Error for boxes: %d %d || %f %f %f %f || %f %f %f %f"), index1, index2, box1->x0, box1->y0, box1->x1, box1->y1, box2->x0, box2->y0, box2->x1, box2->y1);
					GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, msg1);
				}

			}
		}
	}

	msg1 = TEXT("");
	msg1 = msg1.Appendf(TEXT("Errors: %d / %d"), errors, totalChecks);
	GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, msg1);

}

// Called when the game starts or when spawned
void AGlobalObjectsGrid::BeginPlay()
{
	Super::BeginPlay();
	grids = {};
	grids.Add(ActorListGridMultiple(400, 12800));
}

void AGlobalObjectsGrid::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AGlobalObjectsGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

