#include "SDL3/SDL.h"
#include "glm/vec2.hpp"
#include <vector>

struct Animation
{
    float frameRate;
    Uint64 frameStartTimeMS;
    std::vector<SDL_FRect> frames;
    int currentFrame;

    void setFrames(int numFrames, glm::vec2 startPos, glm::vec2 offset, glm::vec2 frameSize)
    {
        frames.clear();
        frames.reserve(numFrames);
        for(int i = 0; i < numFrames; i++)
        {
            const glm::vec2 frameStart = startPos + (float)i * offset;
            frames.push_back(SDL_FRect{frameStart.x, frameStart.y, frameSize.x, frameSize.y});
        }
    }

    static Animation createConveyorAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 16;
        animation.setFrames(4, glm::vec2{0,0}, glm::vec2{0,16}, glm::vec2{98,16});
        return animation;
    }
};
