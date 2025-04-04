#pragma once

#include "Components/Box.h"

struct Collider
{
    Box box;
    bool isEnabled = true;
    bool isOneWay = false;
    bool isTrigger = false;
};