#pragma once

#ifdef E_POINT_DEF
#error APPLICATION CANT HAVE MORE THAN ONE ENTRY POINT
#endif
#define E_POINT_DEF

#ifdef DIST
#include <windows.h>
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{
    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLine(), &argc);
#else
int main(int argc, char** argv)
{
#endif


    // Incase we need to swap window creating for another rendering pipeline?
    do
    {
        if(Tupla::Application::s_Application)
        {
            argc = 2;
            argv[1] = new char[Tupla::Application::Get().GetSpecification().WorkingDirectory.size() + 1];
            memcpy(argv[1], Tupla::Application::Get().GetSpecification().WorkingDirectory.c_str(), Tupla::Application::Get().GetSpecification().WorkingDirectory.size() + 1);
            delete Tupla::Application::s_Application;
        }
        Tupla::CreateApplication({ argc, argv });
        Tupla::Application::Get().Run();
    }
    while (Tupla::Application::m_Restarting);
}