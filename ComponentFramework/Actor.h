#pragma once
#include <vector>
#include <iostream>
#include "Component.h"
#include "TransformComponent.h"
#include <MMath.h>

class Actor : public Component {
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator= (const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

private:
	std::vector<Component*> components;

public:
	Actor(Component* parent_);
	~Actor();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;

	//template<typename ComponentTemplate, typename ... Args>
	//void AddComponent(Args&& ... args_) {
	//	ComponentTemplate* componentObject = new ComponentTemplate(std::forward<Args>(args_)...);
	//	components.push_back(componentObject);

	//}

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

	/*template<typename ComponentTemplate>
	ComponentTemplate* GetComponent() {
		for (auto component : components) {
			if (dynamic_cast<ComponentTemplate*>(component) != nullptr) {
				return dynamic_cast<ComponentTemplate*>(component);
			}
		}
		return nullptr;
	}*/

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
		for (size_t i = 0; i < components.size(); i++) {
			if (dynamic_cast<ComponentTemplate*>(components[i]) != nullptr) {
				components[i]->OnDestroy();
				delete components[i];
				components.erase(components.begin() + i);
				break;
			}
		}
	}



	Matrix4 GetModelMatrix();
	void ListComponents() const;
	void RemoveAllComponents();
};
