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
    Tupla::CreateApplication({argc, argv});

    // Incase we need to swap window creating for another rendering pipeline?
    do
    {
        Tupla::Application::Get().Run();
    }
    while (Tupla::Application::Get().m_Restarting);
}