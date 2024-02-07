// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGenerator.h"
#include "GameFramework/Actor.h"
#include "GlobalWorldGenerator.generated.h"

UCLASS()
class NOCUBETECH_API AGlobalWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalWorldGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	WorldGenerator* generator;

	// called in BeginPlay if the generator is nullptr
	void initGenerator();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	WorldGenerator* GetGenerator() { return generator; }

	UPROPERTY(EditAnywhere)
	UTexture2D* grassTexture;

	UPROPERTY(EditAnywhere)
	UTexture2D* rockTexture;

	UPROPERTY(EditAnywhere)
	UTexture2D* rockTexture2;

	UPROPERTY(EditAnywhere)
	UTexture2D* rockTexture3;

	UPROPERTY(EditAnywhere)
	UTexture2D* dirtTexture1;

};
