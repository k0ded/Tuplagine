#pragma once
#include <Tupla/Core/Base.h>

#define CLASS_COMPONENT_TYPE(x) const char* GetName() override{ return T_STRINGIFY_MACRO(x);}

class IComponent
{
public:
    IComponent()
    {
        HRESULT result = CoCreateGuid(&m_Id);

    	if(FAILED(result))
        {
            LOG_ERROR("Failed to give component a GUID");
        }
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
