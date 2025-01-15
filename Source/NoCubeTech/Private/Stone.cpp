// Fill out your copyright notice in the Description page of Project Settings.


#include "Stone.h"

void AStone::BeginPlay() {
	Super::BeginPlay();
	geometryComponentSet = false;
}



void AStone::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	if (HasAuthority() && !IsSavableValid()) {
		return;
	}

	if (!geometryComponentSet) {
		mesh->ClearAllMeshSections();
		TArray<FVector> normals = {};
		TArray<FVector2D> uvs = {};
		TArray<FLinearColor> colors = {};
		TArray<FProcMeshTangent> tangents = {};

		for (int i = 0; i < vertices.Num(); i++) {
			normals.Add(FVector(0, 0, 1));
			uvs.Add(FVector2D(0, 0));
			colors.Add(FLinearColor(0.5, 0.5, 0.5));
			tangents.Add(FProcMeshTangent());
		}

		UMaterialInterface* stoneMaterial = LoadObject<UMaterialInterface>(this, TEXT("/Script/Engine.Material'/Game/ProceduralMaterials/ProceduralStoneMaterial.ProceduralStoneMaterial'"));

		mesh->CreateMeshSection_LinearColor(0, vertices, faces, normals, uvs, colors, tangents, true);
		mesh->SetMaterial(0, stoneMaterial);
		mesh->SetWorldLocationAndRotation(GetActorLocation(), FQuat::Identity);

		//meshComponent->SetMaterial()
		geometryComponentSet = true;
	}

}

AStone::AStone() {
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(FName(TEXT("Mesh")));
	mesh->RegisterComponent();
	AddInstanceComponent(mesh);
	mesh->SetupAttachment(rootSceneComponent);
	mesh->AttachToComponent(rootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SetReplicates(true);
}

void AStone::loadFromArchive(FArchive& ar) {
	Super::loadFromArchive(ar);
	ar << vertices;
	ar << faces;
}

void AStone::saveToArchive(FArchive& ar) {
	Super::loadFromArchive(ar);
	ar << vertices;
	ar << faces;
}



