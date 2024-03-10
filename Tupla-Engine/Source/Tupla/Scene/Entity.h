#pragma once
#include "Component/IComponent.h"

namespace Tupla
{
    class Entity
    {
    public:
        Entity() { CoCreateGuid(&m_Id); }
        
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

    private:
        std::string m_Name;
        std::vector<Ref<IComponent>> m_Components;
        GUID m_Id {};
    };
}
