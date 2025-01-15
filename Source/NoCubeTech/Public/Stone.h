// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeAdditionalObject.h"
#include "Net/UnrealNetwork.h"
#include "ProceduralMeshComponent.h"
#include "Stone.generated.h"

/**
 *
 */
UCLASS()
class NOCUBETECH_API AStone : public ALandscapeAdditionalObject
{
	GENERATED_BODY()
private:
	bool geometryComponentSet;



public:

	UPROPERTY()
	UProceduralMeshComponent* mesh;

	UPROPERTY()
	TArray<FVector> vertices;
	UPROPERTY()
	TArray<int> faces;

	void Tick(float DeltaTime) override;

	void BeginPlay() override;

	AStone();


	void loadFromArchive(FArchive& ar) override;
	void saveToArchive(FArchive& ar) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
	{
	}


};
