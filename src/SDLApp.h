#pragma once

#include "Singleton.h"

#include <memory>

class Screen;
struct SDL_Window;
struct SDL_Renderer;

class SDLApp : public Singleton<SDLApp>
{
friend class Singleton<SDLApp>;
public:
    void init(const std::string name, const int width, const int height);
    void run();
    void changeScreen(Screen* screen);
    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }

private:
    SDLApp() = default;
    ~SDLApp();

    void initSDL(const std::string name, const int width, const int height);
    void initImGUI();
    void update();
    void render();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Screen* currentScreen = nullptr;
};