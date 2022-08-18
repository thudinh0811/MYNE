#pragma once

#include "Common.h"
#include "Action.h"
#include "EntityManager.h"
                          
#include <memory>

class GameEngine;

struct NavPoint
{
    Vec2 position;
    std::vector<size_t> connections;
    NavPoint() {};
    NavPoint(Vec2 position) : position(position) {}
};

typedef std::map<int, std::string> ActionMap;
typedef std::vector<NavPoint> NavMap;


class Scene
{
protected: 
    
    GameEngine *    m_game = nullptr;
    EntityManager   m_entityManager;
    ActionMap       m_actionMap;
    bool            m_paused = false;
    bool            m_hasEnded = false;
    size_t          m_currentFrame = 0;

    virtual void onEnd() = 0;
    void setPaused(bool paused);

public:
                          
    Scene();
    Scene(GameEngine * gameEngine);

    
    virtual void bindActions() = 0;
    virtual void getKey(int code);
    virtual void update() = 0;
    virtual void sDoAction(const Action & action) = 0;
    virtual void openSettings() = 0;
    virtual void sRender() = 0;

    virtual void doAction(const Action& action);
    void simulate(const size_t frames);
    void registerAction(int inputKey, const std::string& actionName);
    Vec2 getPosition(float gx, float gy) const;
    Vec2 getGridPosition(float x, float y) const;
    void drawLine(const Vec2& p1, const Vec2& p2);

    size_t width() const;
    size_t height() const;
    size_t currentFrame() const;

    bool hasEnded() const;
    const ActionMap& getActionMap() const;
};

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
