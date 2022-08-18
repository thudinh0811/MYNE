#pragma once
                          
#include "Common.h"
#include "Entity.h"
                          
struct Intersect { bool result; Vec2 pos; };

namespace Physics
{
    bool IsInside(const Vec2& pos, std::shared_ptr<Entity> e);
    Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

    Intersect LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
    bool EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e);
}

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
