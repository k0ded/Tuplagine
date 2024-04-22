#pragma once
#include "Component/IComponent.h"

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
        const char* GetName() { return m_Name.c_str(); };
        const std::string& ToString() const { return m_Name; };

    private:
        std::string m_Name = "GameObject";
        std::vector<Ref<IComponent>> m_Components;

#ifdef DEBUG
        std::vector<void*> m_HOTComponents;
#endif

        Scene* m_Scene;
        GUID m_Id {};
    };
}
