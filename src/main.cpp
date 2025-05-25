#include <pch.hpp>
#include <Application.hpp>

auto WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd) -> int {
    return Application::Run(hInstance);
}