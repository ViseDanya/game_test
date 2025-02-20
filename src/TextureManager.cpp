#include "TextureManager.h"
#include "SDL3_image/SDL_image.h"

constexpr char NORMAL_PLATFORM_TEXTURE_PATH[] =  "../assets/normal.png";
constexpr char CONVEYOR_LEFT_TEXTURE_PATH[] =  "../assets/conveyor_left.png";
constexpr char CONVEYOR_RIGHT_TEXTURE_PATH[] =  "../assets/conveyor_right.png";
constexpr char PLAYER_TEXTURE_PATH[] =  "../assets/player.png";
constexpr char WALL_TEXTURE_PATH[] =  "../assets/wall.png";
constexpr char TRAMPOLINE_TEXTURE_PATH[] =  "../assets/trampoline.png";
constexpr char SPIKES_TEXTURE_PATH[] =  "../assets/spikes.png";

SDL_Texture* TextureManager::normalTexture;
SDL_Texture* TextureManager::conveyorLeftTexture;
SDL_Texture* TextureManager::conveyorRightTexture;
SDL_Texture* TextureManager::playerTexture;
SDL_Texture* TextureManager::wallTexture;
SDL_Texture* TextureManager::trampolineTexture;
SDL_Texture* TextureManager::spikesTexture;

void TextureManager::loadAllTextures(SDL_Renderer* renderer)
{
	normalTexture = loadTexture(NORMAL_PLATFORM_TEXTURE_PATH, renderer);
	conveyorLeftTexture = loadTexture(CONVEYOR_LEFT_TEXTURE_PATH, renderer);
	conveyorRightTexture = loadTexture(CONVEYOR_RIGHT_TEXTURE_PATH, renderer);
	playerTexture = loadTexture(PLAYER_TEXTURE_PATH, renderer);
	wallTexture = loadTexture(WALL_TEXTURE_PATH, renderer);
	trampolineTexture = loadTexture(TRAMPOLINE_TEXTURE_PATH, renderer);
	spikesTexture = loadTexture(SPIKES_TEXTURE_PATH, renderer);
}

SDL_Texture* TextureManager::loadTexture(const char* filename, SDL_Renderer* renderer)
{
	SDL_Texture *texture;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
	texture = IMG_LoadTexture(renderer, filename);
	return texture;
}


