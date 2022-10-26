// Fill out your copyright notice in the Description page of Project Settings.


#include "UrdfExporterActor.h"


// Sets default values
AUrdfExporterActor::AUrdfExporterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
        UrdfGenerator = CreateDefaultSubobject<UUrdfGenerator>(FName(*(GetName() + TEXT("_UrdfGenerator"))));
        UrdfGenerator->World = GetWorld();
}

// Called when the game starts or when spawned
void AUrdfExporterActor::BeginPlay()
{
	Super::BeginPlay();

        FString Urdf = UrdfGenerator->GenerateUrdf();

        UE_LOG(LogTemp, Error, TEXT("%s"), *Urdf);

        UrdfGenerator->SaveToFile();
}

// Called every frame
void AUrdfExporterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
