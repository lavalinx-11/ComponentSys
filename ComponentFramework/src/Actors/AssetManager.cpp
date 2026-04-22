#include "Actors/AssetManager.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/ShaderComponent.h"


AssetManager::AssetManager()
{
    
}

AssetManager::~AssetManager()
{
    //RemoveAllComponents();
}

bool AssetManager::OnCreate()
{
    AddComponent<ShaderComponent>("PhongShader", std::weak_ptr<Component>(), "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");

  
    AddComponent<MeshComponent>("PlaneMesh",  std::weak_ptr<Component>(), "meshes/Plane.obj");
    AddComponent<MeshComponent>("PawnMesh",   std::weak_ptr<Component>(), "meshes/Pawn.obj");
    AddComponent<MeshComponent>("KingMesh",   std::weak_ptr<Component>(), "meshes/King.obj");
    AddComponent<MeshComponent>("QueenMesh",  std::weak_ptr<Component>(), "meshes/Queen.obj");
    AddComponent<MeshComponent>("BishopMesh", std::weak_ptr<Component>(), "meshes/Bishop.obj");
    AddComponent<MeshComponent>("KnightMesh", std::weak_ptr<Component>(), "meshes/Knight.obj");
    AddComponent<MeshComponent>("RookMesh",   std::weak_ptr<Component>(), "meshes/Rook.obj");

    
    AddComponent<MeshComponent>("SphereMesh", std::weak_ptr<Component>(), "meshes/Sphere.obj");
    AddComponent<MeshComponent>("CubeMesh",   std::weak_ptr<Component>(), "meshes/Cube.obj");

    AddComponent<MaterialComponent>("BoardMaterial", std::weak_ptr<Component>(), "textures/8x8_board_red.png");
    AddComponent<MaterialComponent>("BlackMaterial", std::weak_ptr<Component>(), "textures/BlackChessPiece.png");
    AddComponent<MaterialComponent>("WhiteMaterial", std::weak_ptr<Component>(), "textures/WhiteChessPiece.png");


    for (auto& pair : componentCatalog) {
        pair.second->OnCreate();
    }

    return true;}

void AssetManager::RemoveAllComponents()
{
    for (auto& pair : componentCatalog) {
        pair.second->OnDestroy();
    }
    componentCatalog.clear();
}

void AssetManager::ListAllComponents() const
{
    for (const auto& pair : componentCatalog) {
        std::cout << "Asset Loaded: " << pair.first << std::endl;
    }
}
