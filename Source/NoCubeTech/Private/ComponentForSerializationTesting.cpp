// Fill out your copyright notice in the Description page of Project Settings.


#include "ComponentForSerializationTesting.h"

// Sets default values for this component's properties
UComponentForSerializationTesting::UComponentForSerializationTesting()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	testvar1 = 0;
	// ...
}


// Called when the game starts
void UComponentForSerializationTesting::BeginPlay()
{
	Super::BeginPlay();
	testvar1 += 1;
	// ...

	FVector selfLocation = GetComponentLocation();
	if (selfLocation.X == 0 && selfLocation.Y == 0) {
		FString str = TEXT("");
		str.Appendf(TEXT("BeginPlay for component, var1 = %d"), testvar1);
		GEngine->AddOnScreenDebugMessage(-1, 600, FColor::White, str);
	}

}


void UComponentForSerializationTesting::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	

	FVector selfLocation = GetComponentLocation();
	if (selfLocation.X == 0 && selfLocation.Y == 0) {
		FString str = TEXT("");
		str.Appendf(TEXT("EndPlay for component, var1 = %d"), testvar1);
		GEngine->AddOnScreenDebugMessage(-1, 600, FColor::White, str);
	}

}


// Called every frame
void UComponentForSerializationTesting::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

