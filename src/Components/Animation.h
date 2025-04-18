#pragma once

#include "Constants.h"
#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <vector>

struct Animation
{
    float frameRate;
    Uint64 frameStartTimeMS;
    std::vector<SDL_FRect> frames;
    int currentFrame;
    bool isPlaying = true;
    bool isLooping = true;

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

    static Animation createPlayerIdleAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 1;
        animation.frames.push_back(SDL_FRect{32*8,0,32,32});
        return animation;
    }

    static Animation createPlayerRunRightAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{0,32}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerRunLeftAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{0,0}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerFallIdleAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{0,32*4}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerFallRightAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{0,32*3}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerFallLeftAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{0,32*2}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerIdleDamagedAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(2, glm::vec2{32*8,0}, glm::vec2{0,32}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerRunRightDamagedAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{32*4,32}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerRunLeftDamagedAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{32*4,0}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerFallIdleDamagedAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{32*4,32*4}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerFallRightDamagedAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{32*4,32*3}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createPlayerFallLeftDamagedAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 8;
        animation.setFrames(4, glm::vec2{32*4,32*2}, glm::vec2{32,0}, glm::vec2{32,32});
        return animation;
    }

    static Animation createTrampolineAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 16;
        animation.isPlaying = false;
        animation.isLooping = false;
        animation.frames.reserve(12);
        animation.setFrames(6, glm::vec2{0,0}, glm::vec2{0,22}, glm::vec2{96,22});
        animation.frames.insert(animation.frames.end(), animation.frames.rbegin(), animation.frames.rend());
        return animation;
    }

    static Animation createFakeAnimation()
    {
        Animation animation;
        animation.currentFrame = 0;
        animation.frameStartTimeMS = SDL_GetTicks();
        animation.frameRate = 7;
        animation.isPlaying = false;
        animation.isLooping = false;
        animation.frames.reserve(7);
        animation.setFrames(6, glm::vec2{0,0}, glm::vec2{0,FAKE_HEIGHT}, glm::vec2{PLATFORM_WIDTH,FAKE_HEIGHT});
        animation.frames.push_back(animation.frames.front());
        return animation;
    }
};
