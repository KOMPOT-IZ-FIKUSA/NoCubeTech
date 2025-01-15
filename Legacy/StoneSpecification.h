// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeAddObjSpecification.h"
#include "Stone.h"
#include "StoneSpecification.generated.h"

/**
 * 
 */
UCLASS()
class NOCUBETECH_API UStoneSpecification : public ULandscapeAddObjSpecification
{
	GENERATED_BODY()
public:
	UStoneSpecification();
	~UStoneSpecification();

	UPROPERTY()
	FVector position;
	UPROPERTY()
	TArray<FVector> vertices;
	UPROPERTY()
	TArray<int> faces;
	


	virtual bool CanExistWithAnother(UAbstractAddObjSpecification* other) const override {
		TArray<FString> otherTags = other->GetTags();
		UStoneSpecification* otherAsStone = Cast<UStoneSpecification>(other);
		if (otherAsStone) {
			return true;
		}
		else {
			return true;
		}
	}


	void SerializeSpecs(FArchive& ar) override {
		ar << position;
		ar << vertices;
		ar << faces;
	}

	AAdditionalObject* CreateObject(AGlobalChunkRegistry* chunkRegistry) override {
		FActorSpawnParameters parameters = FActorSpawnParameters();
		parameters.Name = TEXT("SpawnedStone");
		parameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
		FTransform transform = FTransform(position);
		AActor* result = GetWorld()->SpawnActor(AStone::StaticClass(), &transform, parameters);
		AStone* result1 = Cast<AStone>(result);
		result1->SetupByDefault(chunkRegistry);
		result1->specification = this;
		return result1;
	}

	bool GetHeightIfExistsAt(float x, float y, float& heightResult) override;
};
