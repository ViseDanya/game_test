#pragma once

struct SDL_Texture;
struct SDL_Renderer;

struct TextureManager
{
    SDL_Renderer* renderer;
    SDL_Texture* normalTexture;
    SDL_Texture* conveyorLeftTexture;
    SDL_Texture* conveyorRightTexture;
    
    TextureManager(SDL_Renderer* renderer) : renderer(renderer) {};
    void loadAllTextures();

    private:
    SDL_Texture* loadTexture(const char* filename);
};