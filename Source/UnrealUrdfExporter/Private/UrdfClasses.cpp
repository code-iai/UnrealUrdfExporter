// Fill out your copyright notice in the Description page of Project Settings.


#include "UrdfClasses.h"
#include "Conversions.h"
#include "EditorFramework/AssetImportData.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

FString UUrdfClasses::GetXml()
{
  return GenerateXml();
}

void UUrdfLink::SetParent(AActor* InParent)
{
  Parent = InParent;
  if(Parent && Link)
    {
      Origin = Link->GetActorTransform().GetRelativeTransform(Parent->GetActorTransform());
      FConversions::UToROS(Origin);
    }
}

AActor* UUrdfClasses::GetParent()
{
  return Parent;
}

AActor* UUrdfLink::GetLink()
{
  return Link;
}

bool UUrdfLink::Init(AStaticMeshActor* InActor, FString InPackageName)
{
  if(InActor)
    {
      Link = InActor;
      Name = InActor->GetName();
      PackageName = InPackageName;

      MeshPath = Link->GetStaticMeshComponent()->GetStaticMesh()->AssetImportData->GetFirstFilename();
      int32 Index;
      if(MeshPath.FindLastChar(TCHAR('/'), Index))
        {
          MeshPath = MeshPath.RightChop(Index + 1);
        }

      MeshPath = MeshPath.LeftChop(3);

      // Origin = InActor->GetActorTransform();
      // FConversions::UToROS(Origin);
      return true;
    }
  return false;
}

FString UUrdfClasses::GenerateXmlOrigin(FTransform InOrigin)
{
  return TEXT("<origin rpy=\""
              + FString::SanitizeFloat(FMath::DegreesToRadians(InOrigin.Rotator().Roll)) + TEXT(" ")
              + FString::SanitizeFloat(FMath::DegreesToRadians(InOrigin.Rotator().Pitch)) + TEXT(" ")
              + FString::SanitizeFloat(FMath::DegreesToRadians(InOrigin.Rotator().Yaw))
              +"\" xyz=\""
              + FString::SanitizeFloat(InOrigin.GetLocation().X) + TEXT(" ")
              + FString::SanitizeFloat(InOrigin.GetLocation().Y) + TEXT(" ")
              + FString::SanitizeFloat(InOrigin.GetLocation().Z)
              +"\"/> \n");
}

FString UUrdfLink::GenerateXmlInertial()
{
  return TEXT("<inertial>\n<origin xyz=\"0 0 0.5\" rpy=\"0 0 0\"/>\n<mass value=\"1\"/>\n<inertia ixx=\"100\"  ixy=\"0\"  ixz=\"0\" iyy=\"100\" iyz=\"0\" izz=\"100\" />\n</inertial>\n");
}

FString UUrdfLink::GenerateXml()
{

  FString Xml = TEXT("<link name=\"" + Name + "\"> \n");
  Xml = Xml + GenerateXmlInertial();
  Xml = Xml + TEXT("<visual>\n");
  if(Parent)
    {
      Xml = Xml + GenerateXmlOrigin(Origin);
    }

  Xml = Xml + TEXT("<geometry>\n");


  Xml = Xml + TEXT("<mesh filename=\"package://" + PackageName
                   + TEXT("/meshes/visual/") + MeshPath + TEXT("dae")
                   +"\"/>\n");

  Xml = Xml + TEXT("</geometry>\n");
  Xml = Xml + TEXT("</visual>\n");
  Xml = Xml + TEXT("<collision>\n");
  if(Parent)
    {
      Xml = Xml + GenerateXmlOrigin(Origin);
    }

  Xml = Xml + TEXT("<geometry>\n");
  Xml = Xml + TEXT("<mesh filename=\"package://" + PackageName
                   + TEXT("/meshes/collision/") + MeshPath + TEXT("stl")
                   +"\"/>\n");
  Xml = Xml + TEXT("</geometry>\n");

  Xml = Xml + TEXT("</collision>\n");
  Xml = Xml + TEXT("</link>\n");
  return Xml;
}

void UUrdfLink::SetIsAttached(bool InIsAttached)
{
  bIsAttached = InIsAttached;
}

bool UUrdfLink::GetIsAttached()
{
  return bIsAttached;
}

void UUrdfJoint::Init(APhysicsConstraintActor* InActor)
{
  if(InActor)
    {
      Joint = InActor;
      Name = InActor->GetName();
      Parent = InActor->GetConstraintComp()->ConstraintActor2;
      Child = InActor->GetConstraintComp()->ConstraintActor1;
      if(Parent && Child)
        {
          ParentName = Parent->GetName();
          Origin = Joint->GetActorTransform().GetRelativeTransform(Parent->GetActorTransform());
          FConversions::UToROS(Origin);
          float Limit = 0;

          if(Joint->GetConstraintComp()->ConstraintInstance.GetLinearXMotion() != ELinearConstraintMotion::LCM_Locked)
            {
              AxisVector = FVector(1.0, 0, 0);
              Type = TEXT("prismatic");
              Limit = Joint->GetConstraintComp()->ConstraintInstance.GetLinearLimit();
            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetLinearYMotion() != ELinearConstraintMotion::LCM_Locked)
            {
              AxisVector = FVector(0, 1.0, 0);
              Type = TEXT("prismatic");
              Limit = Joint->GetConstraintComp()->ConstraintInstance.GetLinearLimit();
            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetLinearZMotion() != ELinearConstraintMotion::LCM_Locked)
            {
              AxisVector = FVector(0, 0, 1.0);
              Type = TEXT("prismatic");
              Limit = Joint->GetConstraintComp()->ConstraintInstance.GetLinearLimit();
            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetAngularSwing1Motion() == EAngularConstraintMotion::ACM_Free)
            {
              AxisVector = FVector(1.0, 0, 0);
              Type = TEXT("continuous");
            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetAngularSwing2Motion() == EAngularConstraintMotion::ACM_Free)
            {
              AxisVector = FVector(0, 1.0, 0);
              Type = TEXT("continuous");
            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetAngularTwistMotion() == EAngularConstraintMotion::ACM_Free)
            {
              AxisVector = FVector(0, 0, 1.0);
              Type = TEXT("continuous");
            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetAngularSwing1Motion() == EAngularConstraintMotion::ACM_Limited)
            {
              AxisVector = FVector(1.0, 0, 0);
              Type = TEXT("revolute");
              Limit = Joint->GetConstraintComp()->ConstraintInstance.GetAngularSwing1Limit();
              UpperLimit = FMath::DegreesToRadians(Joint->GetConstraintComp()->ConstraintInstance.AngularRotationOffset.Yaw + Limit);
              LowerLimit = FMath::DegreesToRadians(Joint->GetConstraintComp()->ConstraintInstance.AngularRotationOffset.Yaw - Limit);

            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetAngularSwing2Motion() == EAngularConstraintMotion::ACM_Limited)
            {
              AxisVector = FVector(0, 1.0, 0);
              Type = TEXT("revolute");
              Limit = Joint->GetConstraintComp()->ConstraintInstance.GetAngularSwing2Limit();
              UpperLimit = FMath::DegreesToRadians(Joint->GetConstraintComp()->ConstraintInstance.AngularRotationOffset.Pitch + Limit);
              LowerLimit = FMath::DegreesToRadians(Joint->GetConstraintComp()->ConstraintInstance.AngularRotationOffset.Pitch - Limit);
            }
          else if(Joint->GetConstraintComp()->ConstraintInstance.GetAngularTwistMotion() == EAngularConstraintMotion::ACM_Limited)
            {
              AxisVector = FVector(0, 0, 1.0);
              Type = TEXT("revolute");
              Limit = Joint->GetConstraintComp()->ConstraintInstance.GetAngularTwistLimit();
              UpperLimit = FMath::DegreesToRadians(Joint->GetConstraintComp()->ConstraintInstance.AngularRotationOffset.Roll + Limit);
              LowerLimit = FMath::DegreesToRadians(Joint->GetConstraintComp()->ConstraintInstance.AngularRotationOffset.Roll - Limit);
            }
        }
    }
}

FString UUrdfJoint::GetChildName()
{
  return Child->GetName();
}

AActor* UUrdfJoint::GetChild()
{
  return Child;
}

void UUrdfJoint::SetTypeFixed()
{
  Type = TEXT("fixed");
}

void UUrdfJoint::InitFixedJoint(AActor* InParent, AActor* InChild)
{
  SetTypeFixed();
  if(InParent && InChild)
    {
      ParentName = InParent->GetName();
      Child = InChild;
      Name = ParentName + TEXT("_") + Child->GetName() + TEXT("_joint");
      Origin = Child->GetActorTransform().GetRelativeTransform(InParent->GetActorTransform());
      FConversions::UToROS(Origin);
    }
}

void UUrdfJoint::InitFixedJoint(FString InParentName, AActor* InChild)
{
  SetTypeFixed();
  ParentName = InParentName;
  if(InChild)
    {
      Child = InChild;
      Name = ParentName + TEXT("_") + Child->GetName() + TEXT("_joint");
      //Parent is root joint -> origin is pose in world coordinates
      Origin = InChild->GetActorTransform();
      FConversions::UToROS(Origin);
    }
}

FString UUrdfJoint::GenerateXml()
{
  FString Xml = TEXT("");
  if(Child)
    {
      Xml = TEXT("<joint name=\"" + Name + "\" type=\"" + Type + "\"> \n");
      Xml = Xml + GenerateXmlOrigin(Origin);
      if(!Type.Equals(TEXT("fixed")))
      {
        Xml = Xml + GenerateAxis();

        if(!Type.Equals(TEXT("continuous")))
        {
          Xml = Xml + TEXT("<limit effort=\"1000.0\" lower=\"" + FString::SanitizeFloat(LowerLimit) +"\" upper=\"" + FString::SanitizeFloat(UpperLimit) + "\" velocity=\"1.5708\"/>");
        }
      }
      Xml = Xml + TEXT("<parent link=\"" + ParentName +"\"/>\n");
      Xml = Xml + TEXT("<child link=\"" + Child->GetName() +"\"/>\n");
      Xml = Xml + TEXT("</joint>\n");
    }
  return Xml;
}

FString UUrdfJoint::GenerateAxis()
{
  FString Axis = TEXT("<axis xyz=\"");

  Axis = Axis + FString::SanitizeFloat(AxisVector.X) + TEXT(" ")
    + FString::SanitizeFloat(AxisVector.Y) + TEXT(" ")
    + FString::SanitizeFloat(AxisVector.Z);

  Axis = Axis + TEXT("\"/>\n");

  return Axis;
}
