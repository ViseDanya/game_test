#include "TextureManager.h"
#include "SDL3_image/SDL_image.h"

constexpr char NORMAL_PLATFORM_TEXTURE_PATH[] =  "../assets/normal.png";
constexpr char CONVEYOR_LEFT_TEXTURE_PATH[] =  "../assets/conveyor_left.png";
constexpr char CONVEYOR_RIGHT_TEXTURE_PATH[] =  "../assets/conveyor_right.png";
constexpr char PLAYER_TEXTURE_PATH[] =  "../assets/player.png";

void TextureManager::loadAllTextures()
{
	normalTexture = loadTexture(NORMAL_PLATFORM_TEXTURE_PATH);
	conveyorLeftTexture = loadTexture(CONVEYOR_LEFT_TEXTURE_PATH);
	conveyorRightTexture = loadTexture(CONVEYOR_RIGHT_TEXTURE_PATH);
	playerTexture = loadTexture(PLAYER_TEXTURE_PATH);
}

SDL_Texture* TextureManager::loadTexture(const char* filename)
{
	SDL_Texture *texture;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
	texture = IMG_LoadTexture(renderer, filename);
	return texture;
}


