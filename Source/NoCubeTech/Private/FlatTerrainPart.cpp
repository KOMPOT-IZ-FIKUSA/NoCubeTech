// Fill out your copyright notice in the Description page of Project Settings.


#include "FlatTerrainPart.h"

void AFlatTerrainPart::BeginPlay() {
	Super::BeginPlay();
	geometrySet = false;
}

AFlatTerrainPart::AFlatTerrainPart() : storableReference() {
	geometrySet = false;
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(FName(TEXT("Mesh")));
	mesh->RegisterComponent();
	AddInstanceComponent(mesh);
	mesh->SetupAttachment(rootSceneComponent);
	mesh->AttachToComponent(rootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	vertices = {};
	normals = {};
	faces = {};
	uvs = {};

	SetReplicates(true);

}

void AFlatTerrainPart::loadFromArchive(FArchive& ar) {
	Super::loadFromArchive(ar);
	ar << vertices;
	ar << normals;
	ar << faces;
}

void AFlatTerrainPart::saveToArchive(FArchive& ar) {
	Super::loadFromArchive(ar);
	ar << vertices;
	ar << normals;
	ar << faces;
}

void AFlatTerrainPart::calculateBoundsGlobal(FVector& min_, FVector& max_) {
	check(vertices.Num() > 0);
	min_ = vertices[0];
	max_ = min_;
	for (FVector& vertex : vertices) {
		min_ = FVector::Min(min_, vertex);
		max_ = FVector::Max(max_, vertex);
	}
	FVector selfLocation = GetActorLocation();
	min_ += selfLocation;
	max_ += selfLocation;
}

bool AFlatTerrainPart::GetAbsoluteLandscapeHeight(float x, float y, float& out) {
	FVector min_;
	FVector max_;
	calculateBoundsGlobal(min_, max_);
	FVector delta = max_ - min_;
	if (x >= min_.X && x <= max_.X && y >= min_.Y && y <= max_.Y) {
		out = max_.Z;
		return true;
	}
	else {
		return false;
	}

}

void AFlatTerrainPart::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	if (HasAuthority() && !IsSavableValid()) {
		return;
	}

	//check(vertices.Num() > 0);

	if (storableReference.GetObjPtr() == nullptr && false) {
		AGlobalObjectsGrid* grid = GetGlobalObjectsGrid();
		FVector min_;
		FVector max_;
		calculateBoundsGlobal(min_, max_);
		check(grid != nullptr);
		storableReference = grid->StartTrackingObject(AGlobalObjectsGrid::LANDSCAPE_INDEX, this, min_.X, min_.Y, max_.X, max_.Y);

		check(storableReference.GetObjPtr() != nullptr);
	}

	if (!geometrySet && vertices.Num() > 0) {
		mesh->ClearAllMeshSections();



		TArray<FLinearColor> colors = {};
		TArray<FProcMeshTangent> tangents = {};

		/*
		for (int i = 0; i < vertices.Num(); i++) {
			uvs.Add(FVector2D(0, 0));
			tangents.Add(FProcMeshTangent());
		}
		*/

		//UMaterialInterface* stoneMaterial = LoadObject<UMaterialInterface>(this, TEXT("/Script/Engine.Material'/Game/ProceduralMaterials/ProceduralGroundMaterial.ProceduralGroundMaterial'"));
		UMaterialInterface* stoneMaterial = LoadObject<UMaterialInterface>(this, TEXT("/Script/Engine.Material'/Game/ProceduralMaterials/StoreUvGroundInterpolation.StoreUvGroundInterpolation'"));
		
		mesh->CreateMeshSection_LinearColor(0, vertices, faces, normals, uvs, colors, tangents, true);
		mesh->SetMaterial(0, stoneMaterial);
		mesh->SetWorldLocationAndRotation(GetActorLocation(), FQuat::Identity);

		geometrySet = true;
	}

}

const float AFlatTerrainPart::MIN_BASE_SCALE = 10000;
const float AFlatTerrainPart::MAX_BASE_SCALE = 20000;

TArray<FVector2D> AFlatTerrainPart::Create2DRandomFigure(int64 seedForThisObject, float chunkSize) {
	TArray<FVector2D> result = {};

	float scaleX = MIN_BASE_SCALE + RandomGenerator::IntToFloat(seedForThisObject++) * (MAX_BASE_SCALE - MIN_BASE_SCALE);
	float scaleY = MIN_BASE_SCALE + RandomGenerator::IntToFloat(seedForThisObject++) * (MAX_BASE_SCALE - MIN_BASE_SCALE);


	if (scaleX > chunkSize * 3) {
		scaleX = chunkSize * 3;
	}
	if (scaleY > chunkSize * 3) {
		scaleY = chunkSize * 3;
	}

	int numberOfVertices = 3 + RandomGenerator::IntToInt(seedForThisObject++) % 10;

	for (int i = 0; i < numberOfVertices; i++) {
		float alphaLower = ((float)i + 0.1) / (float)numberOfVertices * 2 * PI;
		float alphaUpper = ((float)i + 0.9) / (float)numberOfVertices * 2 * PI;



		float randomAlpha = RandomGenerator::IntToFloat(seedForThisObject++) * (alphaUpper - alphaLower) + alphaLower;
		float randomDistance = RandomGenerator::IntToFloat(seedForThisObject++) * 0.25 + 0.25;

		result.Add({
			cos(randomAlpha) * randomDistance * scaleX,
			sin(randomAlpha) * randomDistance * scaleY
			});
	}
	return result;

}

void AFlatTerrainPart::CreateGeometryOfOneStone(TArray<FVector>& verticesOut, TArray<int>& facesOut, TArray<FVector>& normalsOut, int seed) {
	verticesOut = {};
	facesOut = {};
	TArray<FVector2D> shape2D = Create2DRandomFigure(seed, this->getChunkRegistry()->WorldPartitionCellSize);
	
	FVector2D randomBias = { RandomGenerator::IntToFloat(seed + 3950682) * 1000 - 500 , RandomGenerator::IntToFloat(seed + 3926682) * 10000 - 5000 };

	for (FVector2D& v : shape2D) v += randomBias;


	int topMiddleVertexIndex = shape2D.Num() * 2;

	float topFaceScale = RandomGenerator::IntToFloat(seed + 123123123) * 0.4 + 0.7;

	float maxHeight1 = -10000;
	float maxHeight2 = -10000;
	float minHeight = 100000000000;
	WorldGenerator* generator = globalWorldGenerator->GetGenerator();
	TArray<float> absoulteHeightsOfInitialFigure = {};
	FVector selfLocation = GetActorLocation();

	// Find the first and the second maximum absolute heights
	for (FVector2D point2D : shape2D) {
		float absoluteHeight = generator->GenerateHeight(selfLocation.X + point2D.X, selfLocation.Y + point2D.Y);
		absoulteHeightsOfInitialFigure.Add(absoluteHeight);
		if (absoluteHeight > maxHeight2) {
			if (absoluteHeight > maxHeight1) {
				maxHeight2 = maxHeight1;
				maxHeight1 = absoluteHeight;
			}
			else {
				maxHeight2 = absoluteHeight;
			}
		}
		if (absoluteHeight < minHeight) {
			minHeight = absoluteHeight;
		}
	}
	float flatHeight = maxHeight2;
	if (maxHeight1 - minHeight < 1000) {
		flatHeight += 1600 * RandomGenerator::IntToFloat(seed + 234234);
	}

	FVector averageTopVertex = FVector(0, 0, 0);

	for (int pointIndex = 0; pointIndex < shape2D.Num(); pointIndex++) {
		FVector2D point2D = shape2D[pointIndex];
		
		float absoluteHeight = absoulteHeightsOfInitialFigure[pointIndex];
		float relativeHeight = absoluteHeight - selfLocation.Z;

		verticesOut.Add({ point2D.X, point2D.Y, fminf(relativeHeight, flatHeight - selfLocation.Z)});
		uvs.Add({ 0, flatHeight - absoluteHeight });
		normalsOut.Add({ 0, 0, 1 });


		verticesOut.Add({ point2D.X, point2D.Y, flatHeight - selfLocation.Z });
		uvs.Add({ 0, 0 });
		normalsOut.Add({ 0, 0, 1 });

		int i = verticesOut.Num();
		averageTopVertex += verticesOut[i - 1];


		if (i == shape2D.Num() * 2) {
			facesOut.Add(0);
			facesOut.Add(i - 2);
			facesOut.Add(1);

			facesOut.Add(i - 2);
			facesOut.Add(i - 1);
			facesOut.Add(1);

			facesOut.Add(1);
			facesOut.Add(i - 1);
			facesOut.Add(topMiddleVertexIndex);

		}
		else {
			facesOut.Add(i - 2);
			facesOut.Add(i - 1);
			facesOut.Add(i + 1);

			facesOut.Add(i);
			facesOut.Add(i - 2);
			facesOut.Add(i + 1);

			facesOut.Add(i + 1);
			facesOut.Add(i - 1);
			facesOut.Add(topMiddleVertexIndex);
		}
	}
	averageTopVertex /= shape2D.Num();
	verticesOut.Add(averageTopVertex);
	uvs.Add({ 0, 0 });
	normalsOut.Add({ 0, 0, 1 });

	// For every top vertex
	for (int i = 1; i < verticesOut.Num(); i += 2) {
		verticesOut[i] = averageTopVertex + (verticesOut[i] - averageTopVertex) * topFaceScale;
	}
	//verticesOut[shape2D.Num() * 2].Z = maxHeight2;

}

void AFlatTerrainPart::setupByDefault() {


	vertices = {};
	faces = {};
	normals = {};
	uvs = {};

	if (!tryFindGeneratorIfNecessary()) {
		return;
	}

	if (this->getChunkRegistry() != nullptr) {
		WorldGenerator* generator = globalWorldGenerator->GetGenerator();
		FVector selfLocation = GetActorLocation();
		int64 objectSeed = this->globalWorldGenerator->GetGenerator()->GetSeed() + selfLocation.X * 10 + selfLocation.Y * 1000;
		int stonesNumber = RandomGenerator::IntToInt(objectSeed + 1346611) % 5 + 1;
		for (int stoneIterationInCluster = 0; stoneIterationInCluster < stonesNumber; stoneIterationInCluster++) {
			
			TArray<FVector> newVertices = {};
			TArray<int> newFaces = {};
			TArray<FVector> newNormals = {};
			this->CreateGeometryOfOneStone(newVertices, newFaces, newNormals, objectSeed + stoneIterationInCluster * 234235);

			int n = vertices.Num();
			for (FVector& vertex : newVertices) {
				vertices.Add(vertex);
			}
			for (FVector& normal : newNormals) {
				normals.Add(normal);
			}
			for (int vertexIndexForFace : newFaces) {
				faces.Add(vertexIndexForFace + n);
			}
		}

		AGlobalObjectsGrid* grid = GetGlobalObjectsGrid();
		FVector min_;
		FVector max_;
		calculateBoundsGlobal(min_, max_);
		check(grid != nullptr);
		storableReference = grid->StartTrackingObject(AGlobalObjectsGrid::LANDSCAPE_INDEX, this, min_.X, min_.Y, max_.X, max_.Y);

	}



}

void AFlatTerrainPart::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (storableReference.GetObjPtr() != nullptr) {
		GetGlobalObjectsGrid()->StopTrackingObject(AGlobalObjectsGrid::LANDSCAPE_INDEX, storableReference);
	}
	Super::EndPlay(EndPlayReason);
}
