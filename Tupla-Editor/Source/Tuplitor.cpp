#include <Tupla.h>

#include "EditorLayer.h"
#include "ProjectSelectionLayer.h"
#include "../../Tupla-Runtime/Source/RuntimeLayer.h"
#include "Tupla/Core/EntryPoint.h"

namespace Tupla
{
    class Tuplitor final : public Application
    {
    public:
        Tuplitor(const ApplicationSpecification& spec) : Application(spec)
        {
            if(spec.WorkingDirectory.empty())
            {
                PushOverlay(new ProjectSelectionLayer());
            }
            else
            {
                PushOverlay(new EditorLayer());
                PushOverlay(new RuntimeLayer());
            }
        }
    };

    Application* CreateApplication(const ApplicationCommandLineArgs args)
    {
        ApplicationSpecification spec;
        spec.ApplicationName = L"Tuplitor - Japp";
        spec.CommandLineArgs = args;

        if(args.Count > 1)
        {
            spec.WorkingDirectory = args.Args[1];
        }

        return new Tuplitor(spec);
    }
}
