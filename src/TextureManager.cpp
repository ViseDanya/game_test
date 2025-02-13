#include "TextureManager.h"
#include "SDL3_image/SDL_image.h"

constexpr char NORMAL_PLATFORM_TEXTURE_PATH[] =  "../assets/normal.png";

void TextureManager::loadAllTextures()
{
	normalTexture = loadTexture(NORMAL_PLATFORM_TEXTURE_PATH);
}

SDL_Texture* TextureManager::loadTexture(const char* filename)
{
	SDL_Texture *texture;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
	texture = IMG_LoadTexture(renderer, filename);
	return texture;
}


