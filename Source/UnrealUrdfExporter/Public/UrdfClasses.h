// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "UrdfClasses.generated.h"

/**
 *
 */
UCLASS()
class UNREALURDFEXPORTER_API UUrdfClasses : public UObject
{
	GENERATED_BODY()

public:

        FString GetXml();
        AActor* GetParent();

protected:

        virtual FString GenerateXml(){return TEXT("");};
        FString GenerateXmlOrigin(FTransform InOrigin);

        UPROPERTY()
        AActor* Parent = nullptr;

        UPROPERTY()
        FString Name = TEXT("");

        UPROPERTY()
        FTransform Origin;
};

UCLASS()
class UNREALURDFEXPORTER_API UUrdfLink : public UUrdfClasses
{
	GENERATED_BODY()
public:
        bool Init(AStaticMeshActor* InActor, FString InPackageName);

        void SetParent(AActor* InParent);
        AActor* GetLink();
        void SetIsAttached(bool InIsAttached);
        bool GetIsAttached();

        // UPROPERTY()
        //   bool bFixedJointReq = false;

protected:
        UPROPERTY()
        AStaticMeshActor* Link = nullptr;

        UPROPERTY()
        bool bIsAttached = false;

        virtual FString GenerateXml() override;
        FString GenerateXmlInertial();

private:
        UPROPERTY()
        FString MeshPath = TEXT("");

        UPROPERTY()
        FString PackageName = TEXT("");
};

UCLASS()
class UNREALURDFEXPORTER_API UUrdfJoint : public UUrdfClasses
{
	GENERATED_BODY()
public:
        void Init(APhysicsConstraintActor* InActor);
        void InitFixedJoint(AActor* InParent, AActor* InChild);
        void InitFixedJoint(FString InParentName, AActor* InChild);
        FString GetChildName();
        AActor* GetChild();

        virtual FString GenerateXml() override;
        FString GenerateAxis();
protected:


        UPROPERTY()
          FString ParentName;

        UPROPERTY()
          FVector AxisVector;

        UPROPERTY()
          FString Type = TEXT("fixed");

        UPROPERTY()
          float UpperLimit = 0;

        UPROPERTY()
          float LowerLimit = 0;

        UPROPERTY()
        APhysicsConstraintActor* Joint = nullptr;

        UPROPERTY()
        AActor* Child = nullptr;

        void SetTypeFixed();
};
