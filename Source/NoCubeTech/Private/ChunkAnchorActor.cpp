// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkAnchorActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LandscapeTileManager.h"
#include "ChunkAdditionalObjectsGenerator.h"
#include "ChunkSave.h"
#include "ChunkSavableActor.h"
#include "LandscapeTile.h"
#include "BasicChunkData.h"
#include "GlobalChunkRegistry.h"

// Sets default values
AChunkAnchor::AChunkAnchor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("DefaultSceneComponent")));
	SetRootComponent(rootSceneComponent);
	chunkRegistry = nullptr;
	ticksSinceBeginPlay = 0;
}

// Called when the game starts or when spawned
void AChunkAnchor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickInterval(0.5);
	ticksSinceBeginPlay = 0;
	
}



// Called every frame
void AChunkAnchor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChunkAnchor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (!HasAuthority()) {
		return;
	}

	if (chunkRegistry.IsValid()) {
		float x = RootComponent->GetComponentLocation().X;
		float y = RootComponent->GetComponentLocation().Y;
		chunkRegistry->UnloadChunk(x, y);
	}

}


bool AChunkAnchor::IsPositionInBounds(float x, float y) {
	if (chunkRegistry.IsValid()) {
		float selfX = RootComponent->GetComponentLocation().X;
		float selfY = RootComponent->GetComponentLocation().Y;
		float selfMinX = selfX - chunkRegistry->WorldPartitionCellSize / 2;
		float selfMaxX = selfX + chunkRegistry->WorldPartitionCellSize / 2;
		float selfMinY = selfY - chunkRegistry->WorldPartitionCellSize / 2;
		float selfMaxY = selfY + chunkRegistry->WorldPartitionCellSize / 2;
		return
			x >= selfMinX && x < selfMaxX &&
			y >= selfMinY && y < selfMaxY;
	}
	return false;
}
bool AChunkAnchor::IsPositionInBounds(FVector pos) {
	return IsPositionInBounds(pos.X, pos.Y);
}


void AChunkAnchor::postInitWithChunkRegistry(AGlobalChunkRegistry* chunkRegistry_) {
	if (!HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to run AChunkAnchor::postInitWithChunkRegistry without server authority"));
		return;
	}
	chunkRegistry = chunkRegistry_;
	setSelfLabel();

	FString key = TEXT("basic");
	UBasicChunkData* chunkDataInstance = Cast<UBasicChunkData>(chunkRegistry->GetChunkData(GetActorLocation().X, GetActorLocation().Y, key, UBasicChunkData::StaticClass()));
	if (!chunkDataInstance) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot get chunk data instance in AChunkAnchor::loadContentOrInitIfNeccesarily"));
		return;
	}

	loadActorsWhichAreNotLoaded(chunkDataInstance);

	if (!chunkDataInstance->IsDataCreated()) {
		initializeContent();
		chunkDataInstance->SetDataCreated();
	}
}

void AChunkAnchor::setSelfLabel() {
	FVector selfLocation = rootSceneComponent->GetComponentLocation();
	int xi = chunkRegistry->PositionToIndex(selfLocation.X);
	int yi = chunkRegistry->PositionToIndex(selfLocation.Y);
	FString actorName = TEXT("ChunkAnchor_");
	actorName.Appendf(TEXT("%d_%d"), xi, yi);
	SetActorLabel(actorName);
}

float AChunkAnchor::FindDistanceToClosestPlayer() {
	if (!HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to run AChunkRegistry::FindDistanceToClosestPlayer without server authority"));
		return -1;
	}
	if (!chunkRegistry.IsValid()) {
		return -1;
	}
	float result = FLT_MAX;
	FVector selfLocation = GetActorLocation();
	TArray<AActor*> playersFound = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), playersFound);
	for (AActor* playerController : playersFound) {
		APawn* controlledPawn = Cast<APlayerController>(playerController)->GetPawn();
		if (!controlledPawn) {
			continue;
		}
		float distance = FVector::DistXY(controlledPawn->GetActorLocation(), selfLocation);
		if (distance < result) {
			result = distance;
		}
	}
	return result;
}

void AChunkAnchor::loadActorsWhichAreNotLoaded(UBasicChunkData* chunkDataInstance) {
	if (!HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to run AChunkAnchor::loadActorsWhichAreNotLoaded without server authority"));
		return;
	}
	for (auto It = chunkDataInstance->CreateActorsConstIterator(); It; ++It) {
		if (chunkRegistry->IsSavableActorTracked(It.Value().ActorSavableUID)) {
			continue;
		}
		FSavedActorContainer container = It.Value();
		FActorSpawnParameters parameters = FActorSpawnParameters();
		parameters.Owner = this;
		parameters.Name = FName(container.ActorName);
		parameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
		AActor* spawnedActor = GetWorld()->SpawnActor(container.ActorClass, &container.ActorTransform, parameters);
		if (!spawnedActor) continue;
		AChunkSavableActor* spawnedActor1 = Cast<AChunkSavableActor>(spawnedActor);
		FMemoryReader memoryReader = FMemoryReader(container.BinaryData);
		spawnedActor1->SetupByLoading(memoryReader, chunkRegistry.Get());
	}
}

void AChunkAnchor::initializeContent() {
	if (!HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to run AChunkAnchor::initializeContent without server authority"));
		return;
	}
	
	FVector selfLocation = rootSceneComponent->GetComponentLocation();
	int xi = chunkRegistry->PositionToIndex(selfLocation.X);
	int yi = chunkRegistry->PositionToIndex(selfLocation.Y);
	FActorSpawnParameters parameters;
	FTransform landscapeTileTransform = FTransform(FVector(selfLocation.X, selfLocation.Y, 0));
	AActor* spawnedTile = GetWorld()->SpawnActor(ALandscapeTile::StaticClass(), &landscapeTileTransform, parameters);
	ALandscapeTile* landscapeTile = Cast<ALandscapeTile>(spawnedTile);
	landscapeTile->SetupByDefault(chunkRegistry.Get());
	
	FString actorLabel;
	actorLabel = TEXT("");
	actorLabel.Appendf(TEXT("AdditionalObjectsGenerator_%d_%d"), xi, yi);
	parameters.OverrideLevel = GetLevel();
	parameters.Owner = this;
	objectsGenerator = GetWorld()->SpawnActor<AChunkAdditionalObjectsGenerator>(parameters);
	objectsGenerator->SetActorLabel(actorLabel);
	objectsGenerator->SetActorLocation(selfLocation);
	objectsGenerator->SetupByDefault(chunkRegistry.Get());
}

AChunkAdditionalObjectsGenerator* AChunkAnchor::GetAdditionalObjectsGenerator() {
	return objectsGenerator;
}