// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTestActor.h"

// Sets default values
AMyTestActor::AMyTestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyTestActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, TEXT("AMyTestActor::BeginPlay with authority"));


		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.Name = FName(TEXT("MyTestActor"));

		spawned = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector(2000, 0, 193000), FRotator::ZeroRotator, SpawnParams);
		spawned->SetActorLabel(TEXT("MyTestActor"));
		USceneComponent* newRootComponent = NewObject<USceneComponent>(spawned.Get());
		newRootComponent->RegisterComponent();
		spawned->AddInstanceComponent(newRootComponent);
		spawned->SetRootComponent(newRootComponent);
		newRootComponent->SetWorldLocation(FVector(2000, 0, 193000));
		newRootComponent->SetIsReplicated(true);
		spawned->SetReplicates(true);
		spawned->bAlwaysRelevant = true;
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, TEXT("AMyTestActor::BeginPlay without authority"));
		Destroy();
	}

}

// Called every frame
void AMyTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority()) {
		spawned->SetActorLocation(spawned->GetActorLocation() + FVector(1, 0, 0));
		FString str = TEXT("");
		str.Appendf(TEXT("%f"), spawned->GetActorLocation().X);
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, str);
	}

}

