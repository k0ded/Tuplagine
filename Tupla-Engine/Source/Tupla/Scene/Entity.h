#pragma once
#include "Component/IComponent.h"
#include "Hot/HotCoordinator.h"
#include "Hot/HotDefines.h"

namespace Tupla
{
	class Scene;

	class Entity
    {
    public:
        Entity(Scene* aScene);

        template <typename T> requires(std::is_base_of_v<IComponent, T>)
        void AddComponent()
        {
            m_Components.push_back(CreateRef<T>());
        }

        template <typename T> requires(!std::is_base_of_v<IComponent, T>)
    	void AddComponent(const T& data = T())
        {
            m_Coordinator->AddComponent(m_HOT, data);
#ifdef DEBUG
            m_HOTComponents.push_back(&m_Coordinator->GetComponent<T>(m_HOT));
#endif
        }
        
        template <typename T> requires(std::is_base_of_v<IComponent, T>)
        void RemoveComponent()
        {
            for (auto i = m_Components.begin(); i != m_Components.end(); ++i)
            {
                if(dynamic_cast<T>(*(i->get())))
                {
                    m_Components.erase(i);
                    break;
                }
            }
        }

        template <typename T> requires(!std::is_base_of_v<IComponent, T>)
    	void RemoveComponent()
        {
            m_Coordinator.GetComponent<T>(m_HOT);
        }
        
        void RemoveComponent(const GUID aId)
        {
            for (auto i = m_Components.begin(); i != m_Components.end(); ++i)
            {
                if(i->get()->GetId() == aId)
                {
                    m_Components.erase(i);
                    break;
                }   
            }
        }

        GUID GetId() const { return m_Id; }

    private:
        std::string m_Name;
        std::vector<Ref<IComponent>> m_Components;

#ifdef DEBUG
        std::vector<void*> m_HOTComponents;
#endif

        HOTEntity m_HOT;
        Scene* m_Scene;
        HotCoordinator* m_Coordinator;
        GUID m_Id {};
    };
}
