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
    //RemoveAllComponents();
}

bool AssetManager::OnCreate(const char* xmlFilePath)
{
    XMLDocument doc;
    XMLError err = doc.LoadFile(xmlFilePath);
    
    if (err != XML_SUCCESS) {
        // This will print "ErrorName: XML_ERROR_FILE_NOT_FOUND" 
        // or a syntax error if your XML has a typo!
        std::string errorMsg = "AssetManager failed to load XML file: ";
        errorMsg += xmlFilePath;
        errorMsg += " | Reason: ";
        errorMsg += doc.ErrorName(); 
        
        Debug::Error(errorMsg, __FILE__, __LINE__);
        return false;
    }

    XMLElement* root = doc.FirstChildElement("Assets");
    if (!root) {
        Debug::Error("XML is missing <Assets> root node!", __FILE__, __LINE__);
        return false;
    }
    
    XMLElement* shadersNode = root->FirstChildElement("Shaders");
    if (shadersNode) {
        for (XMLElement* e = shadersNode->FirstChildElement("Shader"); e != nullptr; e = e->NextSiblingElement("Shader")) {
            const char* name = e->Attribute("name");
            const char* vert = e->Attribute("vert");
            const char* frag = e->Attribute("frag");
            
            if (name && vert && frag) {
                AddComponent<ShaderComponent>(name, std::weak_ptr<Component>(), vert, frag);
            }
        }
    }

    XMLElement* meshesNode = root->FirstChildElement("Meshes");
    if (meshesNode) {
        for (XMLElement* e = meshesNode->FirstChildElement("Mesh"); e != nullptr; e = e->NextSiblingElement("Mesh")) {
            const char* name = e->Attribute("name");
            const char* file = e->Attribute("file");
            
            if (name && file) {
                AddComponent<MeshComponent>(name, std::weak_ptr<Component>(), file);
            }
        }
    }
    
    XMLElement* materialsNode = root->FirstChildElement("Materials");
    if (materialsNode) {
        for (XMLElement* e = materialsNode->FirstChildElement("Material"); e != nullptr; e = e->NextSiblingElement("Material")) {
            const char* name = e->Attribute("name");
            const char* file = e->Attribute("file");
            
            if (name && file) {
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
