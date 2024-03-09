#include <Tupla.h>

#include "EditorLayer.h"
#include "Tupla/Core/EntryPoint.h"

namespace Tupla
{
    class Tuplitor final : public Application
    {
    public:
        Tuplitor(const ApplicationSpecification& spec) : Application(spec)
        {
            PushOverlay(new EditorLayer());
        }
    };

    Application* CreateApplication(const ApplicationCommandLineArgs args)
    {
        ApplicationSpecification spec;
        spec.ApplicationName = "Tuplitor - Japp";
        spec.CommandLineArgs = args;

        return new Tuplitor(spec);
    }
}
