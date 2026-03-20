#pragma once
#include <vector>
#include <iostream>
#include "Components/Component.h"
#include "Components/TransformComponent.h"	
#include <MMath.h>	

class Actor : public Component {
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator= (const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

private:
	std::vector<Ref<Component>> components;

public:
	Actor(std::weak_ptr<Component> parent_);
	~Actor();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;



	template<typename ComponentTemplate>
	void AddComponent(Ref<ComponentTemplate> component_) {
		if (GetComponent<ComponentTemplate>().get() != nullptr) {
#ifdef _DEBUG
			std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
			return;
		}
		components.push_back(component_);
	}

	template<typename ComponentTemplate, typename ... Args>
	void AddComponent(Args&& ... args_) {
		/// before you add the component ask if you have the component in the list already,
		/// if so - don't add a second one. 
		if (GetComponent<ComponentTemplate>().get() != nullptr) {
#ifdef _DEBUG
			std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
			return;
		}
		/// Finish building the component and add the component to the list
		components.push_back(std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...));
	}

	//                       <- Get Component ->
	

	template<typename ComponentTemplate>
	Ref<ComponentTemplate> GetComponent() const {
		for (auto component : components) {
			if (dynamic_cast<ComponentTemplate*>(component.get())) {
				/// This is a dynamic cast designed for shared_ptr's
				/// https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
				return std::dynamic_pointer_cast<ComponentTemplate>(component);
			}
		}
		return Ref<ComponentTemplate>(nullptr);
	}


	template<typename ComponentTemplate>
	void RemoveComponent() {
		for (auto it = components.begin(); it != components.end(); ++it) {
			// Use dynamic_pointer_cast to check the type
			if (std::dynamic_pointer_cast<ComponentTemplate>(*it)) {
				(*it)->OnDestroy();
				components.erase(it); // Erasing the shared_ptr automatically deletes the object!
				break;
			}
		}
	}



	Matrix4 GetModelMatrix() const;
	void ListComponents() const;
	void RemoveAllComponents();
};
