// Fill out your copyright notice in the Description page of Project Settings.


#include "LandscapeTileManager.h"
#include "ProceduralMeshComponent.h"
#include "Engine/Texture2D.h"
#include "Components/SceneComponent.h"
#include "UObject/ObjectPtr.h"
#include "Rendering/Texture2DResource.h"
#include "TextureResource.h"
#include "PlainsBiome.h"
#include "HillsBiome.h"
#include "StaticMeshResources.h"
#include "GlobalChunkRegistry.h"
#include "AbstractBiome.h"
#include "Kismet/GameplayStatics.h"

#include "MeshDescription.h"
#include "WorldGenerator.h"
#include "GlobalWorldGenerator.h"
#include "WorldPartition/WorldPartition.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

// Sets default values for this component's properties
ULandscapeTileManager::ULandscapeTileManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickInterval(0.1);
}



// Called when the game starts
void ULandscapeTileManager::BeginPlay()
{
	Super::BeginPlay();
	materialBase = LoadObject<UMaterial>(NULL, TEXT("/Script/Engine.Material'/Game/ProceduralMaterials/ProceduralGroundMaterial.ProceduralGroundMaterial'"));
	CheckForProceduralComponentAndCreateIfNotPresent();
	FindWorldGenerator();

}

void ULandscapeTileManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {
}


void ULandscapeTileManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!worldGenerator.IsValid() || !worldGenerator->GetGenerator()) {
		FindWorldGenerator();
		return;
	}
	ValidateTrackedPawns();
	SetupLandscapeIfNecessary();
}


void ULandscapeTileManager::FindWorldGenerator() {
	AActor* worldGenerator_ = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalWorldGenerator::StaticClass());
	if (worldGenerator_) {
		AGlobalWorldGenerator* generatorActor = ((AGlobalWorldGenerator*)worldGenerator_);
		worldGenerator = generatorActor;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot find world generator"));
		UE_LOG(LogTemp, Warning, TEXT("Cannot find world generator"))
	}
}

int ULandscapeTileManager::CalculateDetails() const {
	if (!worldGenerator.IsValid() || !worldGenerator->GetGenerator()) return MinTileDetails;
	FVector selfLocation = GetComponentLocation();
	selfLocation.Z = worldGenerator->GetGenerator()->GenerateHeight(selfLocation.X, selfLocation.Y);
	int calculatedDetails;
	if (trackedPawns.Num() == 0) {
		calculatedDetails = MinTileDetails;
	}
	else {
		float distance = 1000000000;
		float p_distance;
		for (TWeakObjectPtr<APawn> pawn : trackedPawns) {
			if (!pawn->GetRootComponent()) {
				continue;
			}
			FVector pawnLocation = pawn->GetRootComponent()->GetComponentLocation();
			if (abs(pawnLocation.X - selfLocation.X) < TileSize / 2 && abs(pawnLocation.Y - selfLocation.Y) < TileSize / 2) {
				distance = 0;
				break;
			}
			else {
				p_distance = FVector::Dist(selfLocation, pawnLocation);
			}
			if (p_distance < distance) {
				distance = p_distance;
			}
		}

		if (distance > 0) {
			calculatedDetails = (int)((float)MaxTileDetails * fmin(1, DetailsDistanceUnit / distance));
			if (calculatedDetails < MinTileDetails) {
				calculatedDetails = MinTileDetails;
			}
		}
		else {
			calculatedDetails = MaxTileDetails;
		}
	}
	calculatedDetails = (int)pow(2, ceil(log2(calculatedDetails)));
	return calculatedDetails;
}

void ULandscapeTileManager::SetupLandscapeIfNecessary() {
	if (!worldGenerator.IsValid() || !UpdateAutomaticaly || !worldGenerator->GetGenerator()) {
		return;
	}

	int calculatedDetails = CalculateDetails();


	if (CurrentDetails == -1 || calculatedDetails > CurrentDetails) {
		if (CurrentDetails == -1) {
			CurrentDetails = 4;
		}
		else {
			CurrentDetails = calculatedDetails;
		}
		SetupGeometry();
		SetupMaterial();
	}
}

void ULandscapeTileManager::ValidateTrackedPawns() {

	TArray<TWeakObjectPtr<APawn>> validPawns;
	for (TWeakObjectPtr<APawn> pawn : trackedPawns) {
		if (pawn.IsValid()) {
			validPawns.Add(pawn);
		}
	}
	trackedPawns = validPawns;
}

void ULandscapeTileManager::CheckForProceduralComponentAndCreateIfNotPresent() {
	if (GetOwner() == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Cannot define owner for LandscapeTIleManager"));
	}
	meshComponent = GetOwner()->GetComponentByClass<UProceduralMeshComponent>();
	if (!meshComponent) {
		FName meshComponentName(TEXT("ProceduralMesh_") + GetOwner()->GetName());
		meshComponent = NewObject<UProceduralMeshComponent>(this, UProceduralMeshComponent::StaticClass(), meshComponentName);
		if (!meshComponent) {
			UE_LOG(LogTemp, Warning, TEXT("Cannot define meshComponent "));
			return;
		}
		meshComponent->RegisterComponent();
		meshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		meshComponent->CreationMethod = EComponentCreationMethod::Instance;
	}
	if (
		meshComponent->GetRelativeLocation() != FVector::Zero() ||
		meshComponent->GetRelativeRotation() != FRotator(0, 0, 0) ||
		meshComponent->GetRelativeScale3D() != FVector(1, 1, 1))
	{
		UE_LOG(LogTemp, Log, TEXT("Setting relative transform"));
		meshComponent->SetRelativeTransform(FTransform::Identity);
	}
}

void ULandscapeTileManager::SetupGeometry() {
	if (!worldGenerator.IsValid() || !worldGenerator->GetGenerator()) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Called ULandscapeTileManager::SetupGeometry with invalid generator."));
		UE_LOG(LogTemp, Warning, TEXT("Called ULandscapeTileManager::SetupGeometry with invalid generator."));
		return;
	}
	const WorldGenerator& generator = *worldGenerator->GetGenerator();
	CheckForProceduralComponentAndCreateIfNotPresent();
	float globalCenterX = GetComponentLocation().X;
	float globalCenterY = GetComponentLocation().Y;
	float x;
	float y = -TileSize / 2.;
	float deltaX = TileSize / (float)CurrentDetails;
	float deltaXHalf = TileSize / (float)CurrentDetails / 2.;
	float deltaY = TileSize / (float)CurrentDetails;
	float deltaYHalf = TileSize / (float)CurrentDetails / 2.;
	int textureSize = CurrentDetails * PixelsBySquare;
	float uMin = 2. / (float)textureSize;
	float vMin = 2. / (float)textureSize;
	float u;
	float v = vMin;
	float deltaU = (1. - 2 * uMin) / (float)CurrentDetails;
	float deltaV = (1. - 2 * vMin) / (float)CurrentDetails;
	TArray<FVector> vertices;
	TArray<FVector> normals;
	FJsonSerializableArrayInt triangles;
	TArray<FVector2D> uv;
	TArray<FLinearColor> colors;
	TArray<FProcMeshTangent> tangents;
	int verticesCountX = CurrentDetails + 1;
	int verticesCountY = CurrentDetails + 1;
	int currentVertexIndex, prevVertexXIndex, prevVertexYIndex, prevVertexXYIndex;

	float middlePointHeight;

	float debug_last_used_v;

	for (int yi = 0; yi < verticesCountY; yi++) {
		x = -TileSize / 2.;
		u = uMin;
		for (int xi = 0; xi < verticesCountX; xi++) {
			vertices.Add(FVector(x, y, generator.GenerateHeight(globalCenterX + x, globalCenterY + y)));
			normals.Add(FVector(0, 0, 1));

			if (xi > 0 and yi > 0) {
				currentVertexIndex = yi * verticesCountX + xi;
				prevVertexXIndex = currentVertexIndex - 1;
				prevVertexYIndex = currentVertexIndex - verticesCountX;
				prevVertexXYIndex = prevVertexYIndex - 1;

				middlePointHeight = generator.GenerateHeight(globalCenterX + x - deltaXHalf, globalCenterY + y - deltaYHalf);


				// there are two ways to create two triangles from 4 vertices (two diagonal sewings)
				// if it is better to sew current and prevXY
				// else it is better to sew prevX and prevY
				if (
					abs((vertices[currentVertexIndex].Z + vertices[prevVertexXYIndex].Z) / 2 - middlePointHeight) <
					abs((vertices[prevVertexXIndex].Z + vertices[prevVertexYIndex].Z) / 2 - middlePointHeight)
					)
				{
					triangles.Add(prevVertexXYIndex);
					triangles.Add(currentVertexIndex);
					triangles.Add(prevVertexYIndex);

					triangles.Add(currentVertexIndex);
					triangles.Add(prevVertexXYIndex);
					triangles.Add(prevVertexXIndex);
				}
				else {

					triangles.Add(currentVertexIndex);
					triangles.Add(prevVertexYIndex);
					triangles.Add(prevVertexXIndex);

					triangles.Add(prevVertexXYIndex);
					triangles.Add(prevVertexXIndex);
					triangles.Add(prevVertexYIndex);

				}



			}
			uv.Add(FVector2D(u, v));
			debug_last_used_v = v;
			colors.Add(FLinearColor::White);
			tangents.Add(FProcMeshTangent());
			u += deltaU;
			x += deltaX;
		}
		v += deltaV;
		y += deltaY;
	}
	meshComponent->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uv, colors, tangents, true);
}

void ULandscapeTileManager::SetupMaterial() {
	if (!worldGenerator.IsValid() || !worldGenerator.Get()) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Called ULandscapeTileManager::SetupMaterial with invalid generator."));
		UE_LOG(LogTemp, Warning, TEXT("Called ULandscapeTileManager::SetupMaterial with invalid generator."));
		return;
	}
	if (!materialBase) {
		// TODO: warning logging otha staf fuck
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Base Material is null"));
	}
	CheckForProceduralComponentAndCreateIfNotPresent();
	meshComponent->SetMaterial(0, materialBase);
}

UTexture2D* ULandscapeTileManager::CreateTextureBGRAWithMips(uint8* data, int width, int height, FName textureName) {
	check(false); // legacy

	UTexture2D* texture = UTexture2D::CreateTransient(width, height, PF_B8G8R8A8, textureName);
	if (texture == nullptr) {
		return nullptr;
	}
	void* TextureData = texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, data, width * height * 4);
	texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	texture->MipGenSettings = TMGS_SimpleAverage;
	//Declaring buffers here to reduce reallocs
	//We double buffer mips, using the prior buffer to build the next buffer
	TArray<uint8> _mipRGBAs;
	TArray<uint8> _mipRGBBs;

	bool ticker = false;
	//Let's try making one ourself
	const uint8* priorData = data;
	int priorwidth = width;
	int priorheight = height;

	while (width > 0 && height > 0)
	{
		int mipwidth = priorwidth >> 1;
		int mipheight = priorheight >> 1;
		if (mipwidth == 0 || mipheight == 0)
		{
			break;
		}
		TArray<uint8>* mipRGBAs = ticker ? &_mipRGBAs : &_mipRGBBs;
		ticker = !ticker;
		mipRGBAs->Reset();
		mipRGBAs->AddUninitialized(mipwidth * mipheight * 4);

		int dataPerRow = priorwidth * 4;

		//Average out the values
		uint8* dataOut = mipRGBAs->GetData();
		for (int y = 0; y < mipheight; y++)
		{
			auto* dataInRow0 = priorData + (dataPerRow * y * 2);
			auto* dataInRow1 = dataInRow0 + dataPerRow;
			for (int x = 0; x < mipwidth; x++)
			{
				int totalB = *dataInRow0++;
				int totalG = *dataInRow0++;
				int totalR = *dataInRow0++;
				int totalA = *dataInRow0++;
				totalB += *dataInRow0++;
				totalG += *dataInRow0++;
				totalR += *dataInRow0++;
				totalA += *dataInRow0++;

				totalB += *dataInRow1++;
				totalG += *dataInRow1++;
				totalR += *dataInRow1++;
				totalA += *dataInRow1++;
				totalB += *dataInRow1++;
				totalG += *dataInRow1++;
				totalR += *dataInRow1++;
				totalA += *dataInRow1++;

				totalB >>= 2;
				totalG >>= 2;
				totalR >>= 2;
				totalA >>= 2;

				*dataOut++ = (uint8)totalB;
				*dataOut++ = (uint8)totalG;
				*dataOut++ = (uint8)totalR;
				*dataOut++ = (uint8)totalA;
			}
			dataInRow0 += priorwidth * 2;
			dataInRow1 += priorwidth * 2;
		}

		// Allocate next mipmap.
		texture->GetPlatformData()->Mips.Add(new FTexture2DMipMap());
		FTexture2DMipMap* Mip = &texture->GetPlatformData()->Mips.Last(0);
		Mip->SizeX = mipwidth;
		Mip->SizeY = mipheight;
		Mip->BulkData.Lock(LOCK_READ_WRITE);
		void* mipData = Mip->BulkData.Realloc(mipRGBAs->Num());
		FMemory::Memcpy(mipData, mipRGBAs->GetData(), mipRGBAs->Num());
		Mip->BulkData.Unlock();

		priorData = mipRGBAs->GetData();
		priorwidth = mipwidth;
		priorheight = mipheight;
	}
	texture->SRGB = false;
	texture->AddressX = TextureAddress::TA_Clamp;
	texture->AddressY = TextureAddress::TA_Clamp;
	texture->UpdateResource();
	return texture;
}
