#include <Tupla.h>

#include "EditorLayer.h"
#include "../../Tupla-Runtime/Source/RuntimeLayer.h"
#include "Tupla/Core/EntryPoint.h"

namespace Tupla
{
    class Tuplitor final : public Application
    {
    public:
        Tuplitor(const ApplicationSpecification& spec) : Application(spec)
        {
            PushOverlay(new EditorLayer());
            PushOverlay(new RuntimeLayer());
        }
    };

    Application* CreateApplication(const ApplicationCommandLineArgs args)
    {
        ApplicationSpecification spec;
        spec.ApplicationName = L"Tuplitor - Japp";
        spec.CommandLineArgs = args;

        return new Tuplitor(spec);
    }
}
