// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UrdfGenerator.h"
#include "UrdfExporterActor.generated.h"

UCLASS()
class UNREALURDFEXPORTER_API AUrdfExporterActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUrdfExporterActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

        UPROPERTY(VisibleAnywhere)
        UUrdfGenerator* UrdfGenerator;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
