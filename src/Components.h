#pragma once

#include "Animation.h"
#include "Assets.h"

class Entity;

class Component
{
public:
    bool has = false;
};

class CTransform : public Component
{
public:
    Vec2 pos = { 0.0, 0.0 };
    Vec2 prevPos = { 0.0, 0.0 };
    Vec2 scale = { 1.0, 1.0 };
    Vec2 velocity = { 0.0, 0.0 };
    Vec2 facing = { 0.0, 1.0 };
    float angle = 0;

    CTransform() {}
    CTransform(const Vec2& p)
        : pos(p) {}
    CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
        : pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}

};
                          
class CLifeSpan : public Component
{
public:
    size_t lifeleft = 0;
    CLifeSpan() {}
    CLifeSpan(int duration)
        : lifeleft(duration) {}
};

class CGravity : public Component
{
public:
    float gravity = 0;
    bool canJump = false;
    CGravity() {}
    CGravity(float g) : gravity(g) {}
};

class CDamage : public Component
{
public:
    int damage = 1;
    CDamage() {}
    CDamage(int d)
        : damage(d) {}
};

class CInvincibility : public Component
{
public:
    int iframes = 0;
    CInvincibility() {}
    CInvincibility(int f)
        : iframes(f) {}
};
                          
class CHealth : public Component
{
public:
    int max = 1;
    int current = 1;
    CHealth() {}
    CHealth(int m, int c)
        : max(m), current(c) {}
};

class CInput : public Component
{
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool attack = false;
    bool next = false;
    bool last = false;

    CInput() {}
};

class CBoundingBox : public Component
{
public:
    Vec2 size;
    Vec2 halfSize;
    bool blockMove = false;
    bool blockVision = false;
    CBoundingBox() {}
    CBoundingBox(const Vec2& s)
        : size(s), halfSize(s.x / 2, s.y / 2) {}
    CBoundingBox(const Vec2& s, bool m, bool v)
        : size(s), blockMove(m), blockVision(v), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
    Animation animation;
    bool repeat = false;
    CAnimation() {}
    CAnimation(const Animation& animation, bool r)
        : animation(animation), repeat(r) {}
};

class CState : public Component
{
public:
    std::string state = "stand";
    CState() {}
    CState(const std::string& s) : state(s) {}
};

class CFollowPlayer : public Component
{
public:
    Vec2 home = { 0, 0 };
    float speed = 0;
    float followDist = 0;
    Vec2 lastGroundPos;
    CFollowPlayer() {}
    CFollowPlayer(Vec2 p, float s, float dist)
        : home(p), speed(s), followDist(dist) {}

};
                          
class CPatrol : public Component
{
public:
    std::vector<Vec2> positions;
    size_t currentPosition = 0;
    float speed = 0;
    CPatrol() {}
    CPatrol(std::vector<Vec2>& pos, float s) : positions(pos), speed(s) {}
};

class CDoor : public Component
{
public:
    std::string leveltxt;
    CDoor() {}
    CDoor(const std::string& lvltxt) : leveltxt(lvltxt) {}
};

class CDrag : public Component
{
public:
    bool dragging = false;
    static std::shared_ptr<Entity> attached;
    CDrag() { attached = 0; }
};

class CCount : public Component
{
public:
    int count = 0;
    CCount() {}
    CCount(int amount) : count(amount) {}
};

class CWeaponReset : public Component
{
public:
    int frames = 0;
    CWeaponReset() {}
    CWeaponReset(int frames) : frames(frames) {}
};

class CItem : public Component
{
public:
    std::string type = "";
    int cooldown = 0;
    CItem() {}
    CItem(const std::string& type, int cd) : type(type), cooldown(cd) {}
};

class CStatusEffect : public Component
{
public:
    std::string effect = "";
    int duration = 0;
    CStatusEffect() {}
    CStatusEffect(const std::string& effect, int duration) : effect(effect), duration(duration) {}
};

class CKnock : public Component
{
public:
    int frames = 0;
    CKnock() {}
    CKnock(int frames) : frames(frames) {}
};

class CShader : public Component
{
public:
    std::string shader;
    CShader() {}
    CShader(const std::string& shader) : shader(shader) {}
};

class CShootPlayer : public Component
{
public:
    float attackSpeed = 0;
    float attackRange = 0;
    std::string projectile = "";
    CShootPlayer() {}
    CShootPlayer(const std::string& pr, float as, float ar) : projectile(pr), attackSpeed(as), attackRange(ar) {}
};

class CExit : public Component
{
public:
    CExit() {};
};

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
