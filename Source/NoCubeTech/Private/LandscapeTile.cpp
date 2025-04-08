// Fill out your copyright notice in the Description page of Project Settings.


#include "LandscapeTile.h"
#include "Net/UnrealNetwork.h"


ALandscapeTile::ALandscapeTile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(1);

}



// Called when the game starts
void ALandscapeTile::BeginPlay()
{
	Super::BeginPlay();


	SetReplicates(true);

	materialBase = LoadObject<UMaterial>(NULL, TEXT("/Script/Engine.Material'/Game/ProceduralMaterials/ProceduralGroundMaterial.ProceduralGroundMaterial'"));
	CheckForProceduralComponentAndCreateIfNotPresent();
	FindWorldGenerator();

}

void ALandscapeTile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALandscapeTile, TileSize);
	DOREPLIFETIME(ALandscapeTile, xIndex);
	DOREPLIFETIME(ALandscapeTile, yIndex);
}

void ALandscapeTile::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}


void ALandscapeTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!worldGenerator.IsValid() || !worldGenerator->GetGenerator()) {
		FindWorldGenerator();
		return;
	}

	// Got with authority or after sync with server
	if (TileSize > 0) {
		SetupLandscapeIfNecessary();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Undefined fields found during ALandscapeTile::Tick"));
	}
}


void ALandscapeTile::FindWorldGenerator() {
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

int ALandscapeTile::CalculateDetails() const {
	if (!worldGenerator.IsValid() || !worldGenerator->GetGenerator()) return MinTileDetails;
	FVector selfLocation = GetActorLocation();
	selfLocation.Z = worldGenerator->GetGenerator()->GenerateHeight(selfLocation.X, selfLocation.Y);
	int calculatedDetails;

	TArray<FVector> playersPositions = TArray<FVector>();
	if (HasAuthority()) {
		TArray<AActor*> playerControllers = TArray<AActor*>();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), playerControllers);
		for (AActor* controller : playerControllers) {
			APawn* pawn = Cast<APlayerController>(controller)->GetPawn();
			if (!pawn) {
				continue;
			}
			else {
				playersPositions.Add(pawn->GetActorLocation());
			}
		}
	}
	else {
		//playersPositions.Add()
	}

	if (playersPositions.Num() == 0) {
		calculatedDetails = MinTileDetails;
	}
	else {
		float distance = 1000000000;
		float p_distance;
		for (FVector pawnLocation : playersPositions) {
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

void ALandscapeTile::SetupLandscapeIfNecessary() {
	if (!worldGenerator.IsValid() || !worldGenerator->GetGenerator()) {
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

void ALandscapeTile::CheckForProceduralComponentAndCreateIfNotPresent() {
	mesh = GetComponentByClass<UProceduralMeshComponent>();
	if (!mesh) {
		FName meshComponentName(TEXT("ProceduralMesh_") + GetNameUsingId());
		mesh = NewObject<UProceduralMeshComponent>(this, UProceduralMeshComponent::StaticClass(), meshComponentName);
		if (!mesh) {
			// TODO: logging
			return;
		}
		mesh->CreationMethod = EComponentCreationMethod::Instance;
		mesh->RegisterComponent();
		mesh->SetupAttachment(rootSceneComponent);
		mesh->AttachToComponent(rootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (
		mesh->GetRelativeLocation() != FVector::Zero() ||
		mesh->GetRelativeRotation() != FRotator(0, 0, 0) ||
		mesh->GetRelativeScale3D() != FVector(1, 1, 1))
	{
		mesh->SetRelativeTransform(FTransform::Identity);
	}
}

void ALandscapeTile::SetupGeometry() {
	if (!worldGenerator.IsValid() || !worldGenerator->GetGenerator()) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Called ALandscapeTile::SetupGeometry with invalid generator."));
		UE_LOG(LogTemp, Warning, TEXT("Called ALandscapeTile::SetupGeometry with invalid generator."));
		return;
	}
	const WorldGenerator& generator = *worldGenerator->GetGenerator();
	CheckForProceduralComponentAndCreateIfNotPresent();
	float globalCenterX = GetActorLocation().X;
	float globalCenterY = GetActorLocation().Y;
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

	for (int yi = 0; yi < verticesCountY; yi++) {
		x = -TileSize / 2.;
		u = uMin;
		for (int xi = 0; xi < verticesCountX; xi++) {
			float height = generator.GenerateHeight(globalCenterX + x, globalCenterY + y);
			vertices.Add(FVector(x, y, height));
			float dx = generator.GenerateHeight(globalCenterX + x + 1, globalCenterY + y) - height;
			float dy = generator.GenerateHeight(globalCenterX + x, globalCenterY + y + 1) - height;

			FVector normal = FVector(-dx, -dy, 1);
			normal.Normalize();
			normals.Add(normal);

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
			colors.Add(FLinearColor::White);

			float l = sqrtf(1 + dx * dx);

			tangents.Add(FProcMeshTangent(1 / l, 0, dx / l));
			u += deltaU;
			x += deltaX;
		}
		v += deltaV;
		y += deltaY;
	}
	mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uv, colors, tangents, true);
}

void ALandscapeTile::SetupMaterial() {
	if (!worldGenerator.IsValid() || !worldGenerator.Get()) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Called ALandscapeTile::SetupMaterial with invalid generator."));
		UE_LOG(LogTemp, Warning, TEXT("Called ALandscapeTile::SetupMaterial with invalid generator."));
		return;
	}
	mesh->SetMaterial(0, materialBase);

}