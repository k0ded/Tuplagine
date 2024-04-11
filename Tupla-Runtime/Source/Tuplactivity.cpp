#include "RuntimeLayer.h"
#include "Tupla/Core/Application.h"
#include "Tupla/Core/EntryPoint.h"

namespace Tupla
{
    class Tuplactivity final : public Application
    {
    public:
        Tuplactivity(const ApplicationSpecification& spec) : Application(spec)
        {
            PushOverlay(new RuntimeLayer());
        }
    };

    Application* CreateApplication(const ApplicationCommandLineArgs args)
    {
        Tupla::ApplicationSpecification spec;
        spec.ApplicationName = L"Tuplitor - Game";
        spec.CommandLineArgs = args;
        spec.ApplicationDirectory = args.Args[0];

        return new Tuplactivity(spec);
    }
}
