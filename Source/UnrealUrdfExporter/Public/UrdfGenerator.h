// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UrdfClasses.h"
#include "UrdfGenerator.generated.h"

/**
 *
 */
UCLASS()
class UNREALURDFEXPORTER_API UUrdfGenerator : public UObject
{
	GENERATED_BODY()
public:

        FString GenerateUrdf();

        void SaveToFile();

        UPROPERTY(VisibleAnywhere)
        UWorld* World;

        UPROPERTY(EditAnywhere)
        FString RobotName = TEXT("robot");

        UPROPERTY(EditAnywhere)
        FString RootLinkName = TEXT("Root");

        /*~ Package path in urdf will look like "package://PackageName/meshes/visual for visual".  */
        UPROPERTY(EditAnywhere)
        FString PackageName = TEXT("");

        UPROPERTY(EditAnywhere)
        FString FileName = TEXT("default");

        UPROPERTY()
        FString Urdf = TEXT("");
};
