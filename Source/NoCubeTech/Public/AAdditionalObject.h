// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalWorldGenerator.h"
#include "ChunkSavableActor.h"
#include "AdditionalObjectsGeneratorRegistry.h"
#include "GlobalObjectsGrid.h"
#include "AAdditionalObject.generated.h"


UCLASS()
class NOCUBETECH_API AAdditionalObject : public AChunkSavableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAdditionalObject();
	

protected:

	TWeakObjectPtr<AGlobalWorldGenerator> globalWorldGenerator;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FBox GetAdditionalBounds() const {
		PURE_VIRTUAL(AAdditionalObject::GetAdditionalBounds, return FBox(FVector::ZeroVector, FVector::ZeroVector););
	};

	virtual bool ContainsPoint(FVector point) const {
		PURE_VIRTUAL(AAdditionalObject::ContainsPoint, return false;);
	};
protected:

	virtual void saveToArchive(FArchive& ar) override {
		
	}

	virtual void loadFromArchive(FArchive& ar) override {
		if (!tryFindGeneratorIfNecessary()) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot find AGlobalWorldGenerator during AAdditionalObject::loadFromArchive. Destroying..."));
			DestroySavable(FDestroySavableActorMode::NONE);
			return;
		}

	}

	bool tryFindGeneratorIfNecessary() {
		if (globalWorldGenerator.IsValid() && globalWorldGenerator->GetGenerator()) {
			return true;
		}
		AActor* worldGenerator_ = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalWorldGenerator::StaticClass());
		if (worldGenerator_) {
			AGlobalWorldGenerator* generatorActor = ((AGlobalWorldGenerator*)worldGenerator_);
			globalWorldGenerator = generatorActor;
			return true;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot find world generator"));
			UE_LOG(LogTemp, Warning, TEXT("AChunkAdditionalObjectsGenerator : Cannot find world generator"))
				return false;
		}
	}

	AGlobalObjectsGrid* GetGlobalObjectsGrid() {
		return Cast<AGlobalObjectsGrid>(UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalObjectsGrid::StaticClass()));
	}

};
