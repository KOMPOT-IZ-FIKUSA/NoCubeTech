// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbstractAddObjSpecification.h"
#include "AAdditionalObject.generated.h"


UCLASS()
class NOCUBETECH_API AAdditionalObject : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAdditionalObject();

protected:
	
	
	UPROPERTY()
	UAbstractAddObjSpecification* specification;
	
	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FBox GetAdditionalBounds() const {
		check(false); // not implemented
		return FBox(FVector::ZeroVector, FVector::ZeroVector);
	};

	virtual bool ContainsPoint(FVector point) const {
		check(false); // not implemented
		return false;
	};


};
