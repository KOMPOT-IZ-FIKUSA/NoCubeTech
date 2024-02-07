// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EditorFramework/AssetImportData.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingDynamic.h"
#include "CubeSpawnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOCUBETECH_API UCubeSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCubeSpawnComponent();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor", meta = (AllowedClasses = "Actor"))
	TWeakObjectPtr<AActor> actorTemplate;

	UPROPERTY()
	bool newCreated;

	UPROPERTY()
	ULevelStreamingDynamic* createdLevel;

	UPROPERTY(EditAnywhere)
	FString chunkLevelName;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
