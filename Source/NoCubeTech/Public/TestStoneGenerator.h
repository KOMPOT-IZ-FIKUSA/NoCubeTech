// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataStructures.h"
#include "ProceduralMeshComponent.h"
#include "RandomGenerator.h"
#include "VoxelSimplifications.h"
#include "TestStoneGenerator.generated.h"


USTRUCT()
struct NOCUBETECH_API FInterpolationSettings {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	float period;
	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	float multiplier;
};

USTRUCT()
struct NOCUBETECH_API FStoneColorSettings {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FInterpolationSettings color1Generation;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FLinearColor color1;

};

USTRUCT()
struct NOCUBETECH_API FStoneGeometrySettings {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	TArray<FInterpolationSettings> trilinearBiases;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	TArray<FStoneColorSettings> additionalColors;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	int64 seed;

};


UCLASS()
class NOCUBETECH_API ATestStoneGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestStoneGenerator();

	UProceduralMeshComponent* meshComponent;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FStoneGeometrySettings settings;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override {
		SetupGeometry();
	};

	static FVector CalculateBezier(FVector A, FVector B, FVector C, FVector D, float t) {
		float u = 1 - t;
		float tt = t * t;
		float uu = u * u;
		float uuu = uu * u;
		float ttt = tt * t;

		FVector P = uuu * A; //influence of A
		P += 3 * uu * t * B; //influence of B
		P += 3 * u * tt * C; //influence of C
		P += ttt * D; //influence of D

		return P;
	}

	bool ShouldTickIfViewportsOnly() const override
	{
		if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	static FVector GenerateVector(float a, float b, int frequency, int64 seed) {
		return FVector(
			RandomGenerator::PeriodicBilinearNoiseInterpolation(a * frequency, b * frequency, frequency, frequency, seed),
			RandomGenerator::PeriodicBilinearNoiseInterpolation(a * frequency, b * frequency, frequency, frequency, seed + 234752347),
			RandomGenerator::PeriodicBilinearNoiseInterpolation(a * frequency, b * frequency, frequency, frequency, seed + 23182387549)
		);
	}

	static FVector GenerateVector2(float x, float y, float z, float period, int64 seed) {
		return FVector(
			RandomGenerator::TrilinearNoiseInterpolation(x / period, y / period, z / period, seed),
			RandomGenerator::TrilinearNoiseInterpolation(x / period, y / period, z / period, seed + 121212347978),
			RandomGenerator::TrilinearNoiseInterpolation(x / period, y / period, z / period, seed + 456723534)
		) * 2 - 1;
	}

	static FVector GenerateVectorConvolutionSum(float x, float y, float z, float mainPeriod, float directionPeriod, int64 seed, int iterations) {
		FVector direction = GenerateVector2(x, y, z, directionPeriod, seed);
		FVector result = FVector(0, 0, 0);
		FVector point = FVector(x, y, z);
		for (int i = 0; i < iterations; i++) {
			result += GenerateVector2(point.X, point.Y, point.Z, mainPeriod, seed + i + 1);
			point += direction;
		}
		return result / iterations;
	}

	UFUNCTION(BlueprintCallable, CallInEditor)
	void SetupGeometry() {
		const int horizontalPoints = 3 + 20;
		const int verticalPointsOnHalfIncludingPoles = 2 + 10;

		FComplexMeshGeometry geometry = FComplexMeshGeometry();

		float horizontalAngle = 2 * PI / horizontalPoints;

		// top half

		TArray<FInt32Vector3> facesToAdd = {};

		int i = 0;
		for (int yi = 0; yi < verticalPointsOnHalfIncludingPoles; yi++) {
			float pitchAngle = (PI) * (0.5 - (float)yi / (verticalPointsOnHalfIncludingPoles - 1) / 2);
			float sinPitch = sin(pitchAngle); // 1 -> 0
			float cosPitch = cos(pitchAngle); // 0 -> 1
			// one more horizontal point for proper UV
			for (int xi = 0; xi < horizontalPoints + 1; xi++) {
				float sin_x = sin(horizontalAngle * xi);
				float cos_x = cos(horizontalAngle * xi);

				/*
				FGeometryVertex newVertex = FGeometryVertex(100 * cosPitch * cos_x, 100 * cosPitch * sin_x, 100 * sinPitch, 0, 0);
				float pitchAlpha = (float)yi / (verticalPointsOnHalfIncludingPoles - 1);
				newVertex.u = pitchAlpha * cos_x / 4 + 0.75;
				newVertex.v = pitchAlpha * sin_x / 4 + 0.75;
				*/

				FGeometryVertexI& vertex = geometry.AddVertex(100 * cosPitch * cos_x, 100 * cosPitch * sin_x, 100 * sinPitch);


				float pitchAlpha = (float)yi / (verticalPointsOnHalfIncludingPoles - 1);
				vertex.u = pitchAlpha * cos_x / 4 + 0.75;
				vertex.v = pitchAlpha * sin_x / 4 + 0.75;


				MorphStoneGeometry(vertex);
				SetStoneColor(vertex);


				i = yi * (horizontalPoints + 1) + xi;

				// if not last row and not last column (one column is duplicated to proper uv)
				if (yi < verticalPointsOnHalfIncludingPoles - 1 && xi < horizontalPoints) {
					facesToAdd.Add(FIntVector3(i, i + 1, i + horizontalPoints + 1));
					facesToAdd.Add(FIntVector3(i + horizontalPoints + 1, i + 1, i + horizontalPoints + 2));
				}
			}
		}

		//geometry.ScaleUV(2);

		for (FInt32Vector3 faceToAdd : facesToAdd) {
			geometry.AddFace(faceToAdd.X, faceToAdd.Y, faceToAdd.Z);
		}


		//geometry.ScaleUV(0.01);


		FString msg = TEXT("");

		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);

		TArray<FVector> vertices = TArray<FVector>();
		TArray<int32> triangles = TArray<int32>();
		TArray<FVector> normals = TArray<FVector>();
		TArray<FVector2D> uv0 = TArray<FVector2D>();
		TArray<FLinearColor> vertexColors = TArray<FLinearColor>();
		TArray<FProcMeshTangent> tangents = TArray<FProcMeshTangent>();
		geometry.Export(vertices, triangles, normals, uv0, vertexColors, tangents);



		meshComponent->ClearAllMeshSections();
		meshComponent->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uv0, vertexColors, tangents, true);
	}

	void MorphStoneGeometry(FGeometryVertexI& vertex) {

		FVector translation = FVector(0, 0, 0);
		for (int i = 0; i < settings.trilinearBiases.Num(); i++) {
			FInterpolationSettings s = settings.trilinearBiases[i];
			translation += GenerateVector2(vertex.x, vertex.y, vertex.z, s.period, settings.seed + i) * s.multiplier;
		}
		vertex.x += translation.X;
		vertex.y += translation.Y;

	}

	void SetStoneColor(FGeometryVertexI& vertex) {
		vertex.color = FLinearColor(96. / 255, 103. / 255, 107. / 255) / 3;
		vertex.color = FLinearColor(0, 0, 0);

		for (int i = 0; i < settings.additionalColors.Num(); i++) {
			FStoneColorSettings colorSettings = settings.additionalColors[i];
			float period = colorSettings.color1Generation.period;
			float multiplier = colorSettings.color1Generation.multiplier;
			if (period == 0) {
				period = 1;
			}
			float color1_k = RandomGenerator::TrilinearNoiseInterpolation(vertex.x / period, vertex.y / period, vertex.z / period, settings.seed + 237823 + i) * multiplier;
			vertex.color += colorSettings.color1 * color1_k;
		}

		float testR = 1 / (1 + exp(-vertex.x / 10));
		vertex.color = FLinearColor(vertex.x / 200. + 0.5, vertex.y / 200. + 0.5, 0, 0.9);

	}

};
