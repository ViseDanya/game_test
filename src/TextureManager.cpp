#include "TextureManager.h"
#include "SDL3_image/SDL_image.h"
#include <iostream>
#include <string>

constexpr char NORMAL_PLATFORM_TEXTURE_PATH[] =  "normal.png";
constexpr char CONVEYOR_LEFT_TEXTURE_PATH[] =  "conveyor_left.png";
constexpr char CONVEYOR_RIGHT_TEXTURE_PATH[] =  "conveyor_right.png";
constexpr char PLAYER_TEXTURE_PATH[] =  "player.png";
constexpr char WALL_TEXTURE_PATH[] =  "wall.png";
constexpr char TRAMPOLINE_TEXTURE_PATH[] =  "trampoline.png";
constexpr char SPIKES_TEXTURE_PATH[] =  "spikes.png";
constexpr char FAKE_TEXTURE_PATH[] =  "fake.png";

SDL_Texture* TextureManager::normalTexture;
SDL_Texture* TextureManager::conveyorLeftTexture;
SDL_Texture* TextureManager::conveyorRightTexture;
SDL_Texture* TextureManager::playerTexture;
SDL_Texture* TextureManager::wallTexture;
SDL_Texture* TextureManager::trampolineTexture;
SDL_Texture* TextureManager::spikesTexture;
SDL_Texture* TextureManager::fakeTexture;

std::string getResourcePath(std::string resourcePath) 
{
    #ifdef RESOURCE_PATH
		std::cout << "bruh" << std::endl;
		std::string env = std::string(RESOURCE_PATH);
		std::cout << env << std::endl;
		std::string out = std::string(RESOURCE_PATH) + std::string(resourcePath);
		std::cout << out << std::endl;
		return out;
	#else
		std::cerr << "Error: RESOURCE_PATH not defined!" << std::endl;
		return "";
	#endif
}

void TextureManager::loadAllTextures(SDL_Renderer* renderer)
{
	normalTexture = loadTexture(getResourcePath(NORMAL_PLATFORM_TEXTURE_PATH).c_str(), renderer);
	conveyorLeftTexture = loadTexture(getResourcePath(CONVEYOR_LEFT_TEXTURE_PATH).c_str(), renderer);
	conveyorRightTexture = loadTexture(getResourcePath(CONVEYOR_RIGHT_TEXTURE_PATH).c_str(), renderer);
	playerTexture = loadTexture(getResourcePath(PLAYER_TEXTURE_PATH).c_str(), renderer);
	wallTexture = loadTexture(getResourcePath(WALL_TEXTURE_PATH).c_str(), renderer);
	trampolineTexture = loadTexture(getResourcePath(TRAMPOLINE_TEXTURE_PATH).c_str(), renderer);
	spikesTexture = loadTexture(getResourcePath(SPIKES_TEXTURE_PATH).c_str(), renderer);
	fakeTexture = loadTexture(getResourcePath(FAKE_TEXTURE_PATH).c_str(), renderer);
}

SDL_Texture* TextureManager::loadTexture(const char* filename, SDL_Renderer* renderer)
{
	SDL_Texture *texture;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
	texture = IMG_LoadTexture(renderer, filename);
	return texture;
}


