#include <filesystem>
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
            PushOverlay(new EditorLayer());
            PushOverlay(new RuntimeLayer());
        }
    };

    Application* CreateApplication(const ApplicationCommandLineArgs args)
    {
        ApplicationSpecification spec;
        spec.WindowProperties.Title = L"Tuplitor - Japp";
        spec.WindowProperties.Position = WindowPosition::Center;
        spec.CommandLineArgs = args;
        spec.ApplicationDirectory = std::filesystem::path(args.Args[0]).parent_path().string(); // Gets the directory instead of the exe path.

        if(args.Count > 1)
        {
            spec.WorkingDirectory = args.Args[1];
        }
        else
        {
            spec.WindowProperties.Width = 900;
            spec.WindowProperties.Height = 600;
        }

        if (spec.WorkingDirectory.empty())
        {
            spec.WorkingDirectory = spec.ApplicationDirectory + "/../../../../Project";
        }

        return new Tuplitor(spec);
    }
}
