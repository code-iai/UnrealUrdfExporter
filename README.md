# UnrealUrdfExporter

## Usage

- place UnrealUrdfExporterActor into the world
- select the actor
- double click on URDFGenerator
- input
  - RobotName: files argument robot name in urdf
  - RootLink: creates virtual link for root e.g. apartment_root
  - PackageName: Package where the meshes are
  - FileName: Name of the urdf
- press BeginPlay

## Assumptions for Modeling in UE4

- ConstraintActor1 is Child
- ConstraintActor2 is Parent
- Origin of Child is constraint frame


## Possible Issues

- Constraint axis and limits not tested
