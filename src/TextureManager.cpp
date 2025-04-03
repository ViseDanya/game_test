#include "TextureManager.h"
#include "SDL3_image/SDL_image.h"

#include <string>

constexpr char NORMAL_PLATFORM_TEXTURE_PATH[] =  "assets/normal.png";
constexpr char CONVEYOR_LEFT_TEXTURE_PATH[] =  "assets/conveyor_left.png";
constexpr char CONVEYOR_RIGHT_TEXTURE_PATH[] =  "assets/conveyor_right.png";
constexpr char PLAYER_TEXTURE_PATH[] =  "assets/player.png";
constexpr char WALL_TEXTURE_PATH[] =  "assets/wall.png";
constexpr char TRAMPOLINE_TEXTURE_PATH[] =  "assets/trampoline.png";
constexpr char SPIKES_TEXTURE_PATH[] =  "assets/spikes.png";
constexpr char FAKE_TEXTURE_PATH[] =  "assets/fake.png";

SDL_Texture* TextureManager::normalTexture;
SDL_Texture* TextureManager::conveyorLeftTexture;
SDL_Texture* TextureManager::conveyorRightTexture;
SDL_Texture* TextureManager::playerTexture;
SDL_Texture* TextureManager::wallTexture;
SDL_Texture* TextureManager::trampolineTexture;
SDL_Texture* TextureManager::spikesTexture;
SDL_Texture* TextureManager::fakeTexture;

std::string getFullTexturePath(const char* path)
{
	std::string fullPath;
	fullPath += SDL_GetBasePath();
	fullPath += path;
	return fullPath;
}

void TextureManager::loadAllTextures(SDL_Renderer* renderer)
{
	normalTexture = loadTexture(getFullTexturePath(NORMAL_PLATFORM_TEXTURE_PATH).c_str(), renderer);
	conveyorLeftTexture = loadTexture(getFullTexturePath(CONVEYOR_LEFT_TEXTURE_PATH).c_str(), renderer);
	conveyorRightTexture = loadTexture(getFullTexturePath(CONVEYOR_RIGHT_TEXTURE_PATH).c_str(), renderer);
	playerTexture = loadTexture(getFullTexturePath(PLAYER_TEXTURE_PATH).c_str(), renderer);
	wallTexture = loadTexture(getFullTexturePath(WALL_TEXTURE_PATH).c_str(), renderer);
	trampolineTexture = loadTexture(getFullTexturePath(TRAMPOLINE_TEXTURE_PATH).c_str(), renderer);
	spikesTexture = loadTexture(getFullTexturePath(SPIKES_TEXTURE_PATH).c_str(), renderer);
	fakeTexture = loadTexture(getFullTexturePath(FAKE_TEXTURE_PATH).c_str(), renderer);
}

SDL_Texture* TextureManager::loadTexture(const char* texturePath, SDL_Renderer* renderer)
{
	SDL_Texture *texture;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", texturePath);
	texture = IMG_LoadTexture(renderer, texturePath);
	return texture;
}


