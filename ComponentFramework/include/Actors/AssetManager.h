#pragma once
#include <string>
#include <iostream>
#include <unordered_map> 
#include "Components/Component.h"
#include "Engine/Debug.h"

class AssetManager {
private:
	std::unordered_map<std::string, Ref<Component>> componentCatalog;
    AssetManager();
    ~AssetManager();
public:
static AssetManager& GetInstance() {
	static AssetManager instance;
	return instance;
}

	
    bool OnCreate(const char* xmlFilePath);
    void RemoveAllComponents();
    void ListAllComponents() const;

    template<typename ComponentTemplate, typename ... Args>
  void AddComponent(const char* name, Args&& ... args_) {
        Ref<ComponentTemplate> t = std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...);
        componentCatalog[name] = t;
    }

     template<typename ComponentTemplate>
Ref<ComponentTemplate> GetComponent(const char* name) const
    {
	    auto id = componentCatalog.find(name);
    	if (id == componentCatalog.end()) {
    		Debug::Error("Can't find requested component", __FILE__, __LINE__);
    		return Ref<ComponentTemplate>(nullptr);
    	}
    	return std::dynamic_pointer_cast<ComponentTemplate>(id->second);
    }
};