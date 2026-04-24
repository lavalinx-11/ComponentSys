#include "Actors/AssetManager.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/ShaderComponent.h"
#include "Engine/tinyxml2.h"

using namespace tinyxml2;
AssetManager::AssetManager()
{
    
}

AssetManager::~AssetManager()
{
    RemoveAllComponents();
}

bool AssetManager::OnCreate(const char* xmlFilePath)
{
    XMLDocument doc;
    XMLError err = doc.LoadFile(xmlFilePath);
  
    // Find XML File
    if (err != XML_SUCCESS) {
        std::string errorMsg = "AssetManager failed to load XML file: ";
        errorMsg += xmlFilePath;
        errorMsg += " | Reason: ";
        errorMsg += doc.ErrorName(); 
        
        Debug::Error(errorMsg, __FILE__, __LINE__);
        return false;
    }

    // Get the rood of the XML which is assets
    XMLElement* root = doc.FirstChildElement("Assets");
    if (!root) {
        Debug::Error("XML is missing <Assets> root node!", __FILE__, __LINE__);
        return false;
    }
    
    // Grab shaders
    XMLElement* shadersNode = root->FirstChildElement("Shaders");
    if (shadersNode) {
        for (XMLElement* e = shadersNode->FirstChildElement("Shader"); e != nullptr; e = e->NextSiblingElement("Shader")) {
            const char* name = e->Attribute("name");
            const char* vert = e->Attribute("vert");
            const char* frag = e->Attribute("frag");
            
            if (name && vert && frag) {
                // Add shader component
                AddComponent<ShaderComponent>(name, std::weak_ptr<Component>(), vert, frag);
            }
        }
    }
    
// Grab meshes
    XMLElement* meshesNode = root->FirstChildElement("Meshes");
    if (meshesNode) {
        for (XMLElement* e = meshesNode->FirstChildElement("Mesh"); e != nullptr; e = e->NextSiblingElement("Mesh")) {
            const char* name = e->Attribute("name");
            const char* file = e->Attribute("file");
            
            if (name && file) {
                // Add component
                AddComponent<MeshComponent>(name, std::weak_ptr<Component>(), file);
            }
        }
    }
    
    // Grab materials
    XMLElement* materialsNode = root->FirstChildElement("Materials");
    if (materialsNode) {
        for (XMLElement* e = materialsNode->FirstChildElement("Material"); e != nullptr; e = e->NextSiblingElement("Material")) {
            const char* name = e->Attribute("name");
            const char* file = e->Attribute("file");
            
            if (name && file) {
                // Add material
                AddComponent<MaterialComponent>(name, std::weak_ptr<Component>(), file);
            }
        }
    }

    
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
