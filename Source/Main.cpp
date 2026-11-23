#include <memory>
#include "NightmareGame.h"
#include "SpinoCore/Core/Window/WindowProps.h"
#include "SpinoCore/Core/Window/WindowSize.h"
#include "SpinoCore/Launch.h"
#include "SpinoCore/LaunchOptions.h"

int main() {
    SpinoCore::LaunchOptions options{};
    options.gameInfo.name = "SpinoCore Nightmare";
    options.windowProps.title = "SpinoCore Nightmare";
    options.windowProps.size = SpinoCore::Core::Window::WINDOW_SIZE_MEDIUM;
    options.windowProps.windowMode = SpinoCore::Core::Window::WindowMode::BorderlessFullscreen;
    options.windowProps.hideSystemCursor = true;
    options.enableGlobalLight = true;

    return SpinoCore::Launch(std::make_unique<Nightmare::NightmareGame>(), options);
}
