// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkAnchorActor.h"
#include "Kismet/GameplayStatics.h"
#include "TestSavableActor.h"

// Sets default values
ATestSavableActor::ATestSavableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("DefaultSceneComponent")));
	SetRootComponent(rootSceneComponent);

}

// Called when the game starts or when spawned
void ATestSavableActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATestSavableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestSavableActor::EndPlay(EEndPlayReason::Type endPlayReason) {
	if (HasValidRootComponent()) {
		float x = rootSceneComponent->GetComponentLocation().X;
		float y = rootSceneComponent->GetComponentLocation().Y;
		TArray<AActor*> anchors = TArray<AActor*>();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChunkAnchor::StaticClass(), anchors);
		for (AActor* anchor : anchors) {
			AChunkAnchor* anchor1 = Cast<AChunkAnchor>(anchor);
			if (anchor1) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Saving object in EndPlay"));
				if (anchor1->IsPositionInBounds(x, y)) {
					SaveToChunkAnchor(anchor1);
					GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Success (in bounds)"));
					break;
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Cannot find chunk anchor"));
				}
			}
		}
	}

}

