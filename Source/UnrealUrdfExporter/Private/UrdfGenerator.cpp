// Fill out your copyright notice in the Description page of Project Settings.

#include "UrdfGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "Engine/StaticMeshActor.h"
#include "XmlFile.h"

FString UUrdfGenerator::GenerateUrdf()
{
  Urdf = TEXT("<?xml version=\"1.0\" ?> \n");
  Urdf = Urdf + TEXT("<robot name=\"") + RobotName + TEXT("\"> \n");

  // Root link
  Urdf = Urdf + TEXT("<link name=\"" + RootLinkName + "\"> \n");
  Urdf = Urdf + TEXT("<origin rpy=\"0 0 0\" xyz=\"0 0 0\"/> \n");
  Urdf = Urdf + TEXT("</link> \n");

  if(World)
    {
      UE_LOG(LogTemp, Log, TEXT("[%s]: Start generating Urdf"), *FString(__FUNCTION__));

      TArray<AActor*> Joints;
      UGameplayStatics::GetAllActorsOfClass(World, APhysicsConstraintActor::StaticClass(), Joints);



      TArray<AActor*> Links;
      UGameplayStatics::GetAllActorsOfClass(World, AStaticMeshActor::StaticClass(), Links);

      TMap<FString, UUrdfLink*> UrdfLinks;
      TArray<UUrdfJoint*> UrdfJoints;
      UE_LOG(LogTemp, Log, TEXT("[%s]: Start generating Links"), *FString(__FUNCTION__));
      for(auto & Link : Links)
        {
          UUrdfLink* UrdfLink = NewObject<UUrdfLink>(this);
          if(UrdfLink->Init(Cast<AStaticMeshActor>(Link), PackageName))
            {
              UrdfLinks.Add(Link->GetName(), UrdfLink);

              //if Actor is attached to another Actor, create a fixed joint
              AStaticMeshActor* TempActor = Cast<AStaticMeshActor>(Link->GetAttachParentActor());
              if(TempActor)
                {
                  UrdfLink->SetIsAttached(true);
                  UUrdfJoint* UrdfJoint = NewObject<UUrdfJoint>(this);
                  UrdfJoint->InitFixedJoint(TempActor, UrdfLink->GetLink());
                  UrdfJoints.Add(UrdfJoint);
                }
            }
        }

      UE_LOG(LogTemp, Log, TEXT("[%s]: Start generating Joints"), *FString(__FUNCTION__));
      for(auto & Joint : Joints)
        {
          UUrdfJoint* UrdfJoint = NewObject<UUrdfJoint>(this);
          UrdfJoint->Init(Cast<APhysicsConstraintActor>(Joint));
          UrdfJoints.Add(UrdfJoint);

          UUrdfLink* Child = UrdfLinks[UrdfJoint->GetChildName()];
          if(Child)
            {
              Child->SetParent(Joint);
            }
        }

      for(auto & UrdfLink : UrdfLinks)
        {
          Urdf = Urdf + UrdfLink.Value->GetXml();
          if(!UrdfLink.Value->GetParent() && !UrdfLink.Value->GetIsAttached())
            {
              UUrdfJoint* UrdfJoint = NewObject<UUrdfJoint>(this);
              UrdfJoint->InitFixedJoint(RootLinkName, UrdfLink.Value->GetLink());
              UrdfJoints.Add(UrdfJoint);
            }
        }
      for(auto & UrdfJoint : UrdfJoints)
        {
          Urdf = Urdf + UrdfJoint->GetXml();
        }
    }
  else
    {
        UE_LOG(LogTemp, Error, TEXT("[%s]: World is nullptr"), *FString(__FUNCTION__));
    }

  Urdf = Urdf + TEXT("</robot> \n");
  return Urdf;
}

void UUrdfGenerator::SaveToFile()
{
  FXmlFile* XmlFile = new FXmlFile(Urdf, EConstructMethod::ConstructFromBuffer);
  FString SavePath = FPaths::Combine(FPaths::ProjectContentDir(), FileName + TEXT(".urdf"));
  UE_LOG(LogTemp, Error, TEXT("[%s]: SavePath: %s"), *FString(__FUNCTION__), *SavePath);
  XmlFile->Save(SavePath);
}
