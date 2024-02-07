// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeSpawnComponent.h"
#include "WorldPartition/WorldPartition.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingDynamic.h"
#include "WorldPartition/WorldPartitionLevelStreamingDynamic.h"

// Sets default values for this component's properties
UCubeSpawnComponent::UCubeSpawnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UCubeSpawnComponent::BeginPlay()
{
	Super::BeginPlay();
	newCreated = false;

	bool success;
	ULevelStreamingDynamic* level = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), chunkLevelName, FVector(12800 * 4, 0, 0), FRotator::ZeroRotator, success);
	GetWorld()->AddStreamingLevel(level);
	createdLevel = level;


	return;
	FActorSpawnParameters parameters;
	for (int xi = 0; xi < 5; xi++) {
		for (int yi = 0; yi < 5; yi++) {
			FVector pos = FVector(xi * 1, yi * 1, 0);
			AActor* actor = GetWorld()->SpawnActor<AActor>(pos, FRotator(), parameters);
			USceneComponent* component = NewObject<USceneComponent>(actor);
			component->RegisterComponent();
			actor->AddInstanceComponent(component);
			actor->SetRootComponent(component);
			component->SetWorldLocation(pos);
		}
	}
}


// Called every frame
void UCubeSpawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	return;

	FString str = TEXT("");
	for (int i = 0; i < GetWorld()->GetStreamingLevels().Num(); i++) {
		str.AppendInt((uint8)GetWorld()->GetStreamingLevels()[i]->GetLevelStreamingState());
		str.AppendChar(' ');
	}
	GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, str);
	
	
	return;


	if (newCreated) {
		return;
	}
	if (GetWorld()->GetTimeSeconds() - GetOwner()->CreationTime < 1) {
		return;
	}
	newCreated = true;



	//str = TEXT("Lifetime: ");
	//str.AppendInt(GetWorld()->GetTimeSeconds() - GetOwner()->CreationTime);
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, str);

	FActorSpawnParameters parameters;
	parameters.OverrideLevel = createdLevel->GetLoadedLevel();
	AActor* actor = GetWorld()->SpawnActor<AActor>(FVector(0, 0, 0), FRotator(), parameters);


	UStaticMeshComponent* component = NewObject<UStaticMeshComponent>(actor);
	component->RegisterComponent();
	actor->AddInstanceComponent(component);
	actor->SetRootComponent(component);
	component->SetStaticMesh(StaticMesh);
	component->SetSimulatePhysics(true);
	component->SetRelativeScale3D(FVector(5, 5, 5));


	str = TEXT("created ");
	str.AppendInt(GetWorld()->GetStreamingLevels().Num());
	str.Append(" ");
	//str.AppendInt(GetOwner()->IsInPersistentLevel());
	GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, str);




}

