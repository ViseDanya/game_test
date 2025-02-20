#pragma once

struct SDL_Texture;
struct SDL_Renderer;

struct TextureManager
{
    static SDL_Texture* normalTexture;
    static SDL_Texture* conveyorLeftTexture;
    static SDL_Texture* conveyorRightTexture;
    static SDL_Texture* playerTexture;
    static SDL_Texture* wallTexture;
    static SDL_Texture* trampolineTexture;
    static SDL_Texture* spikesTexture;
    
    static void loadAllTextures(SDL_Renderer* renderer);

    private:
    static SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer);
};