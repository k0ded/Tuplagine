#pragma once
#include <Tupla/Core/Base.h>

#define CLASS_COMPONENT_TYPE(x) const char* GetName() override{ return T_STRINGIFY_MACRO(x);}

class IComponent
{
public:
    IComponent()
    {
        CoCreateGuid(&m_Id);
    }
    virtual ~IComponent() = default;

    virtual void Update() {}
    virtual void FixedUpdate() {}
    
    virtual const char* GetName();
    virtual std::string ToString() { return GetName(); }
    GUID GetId() const { return m_Id; }

protected:
    GUID m_Id;
};
