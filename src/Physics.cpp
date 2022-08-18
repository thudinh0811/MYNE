#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 overlap;
    auto& aCol = a->getComponent<CBoundingBox>();
    auto& bCol = b->getComponent<CBoundingBox>();

    if (aCol.has && bCol.has)
    {
        auto& aTrans = a->getComponent<CTransform>();
        auto& bTrans = b->getComponent<CTransform>();

        Vec2 delta(abs(aTrans.pos.x - bTrans.pos.x), abs(aTrans.pos.y - bTrans.pos.y));
        overlap.x = aCol.halfSize.x + bCol.halfSize.x - delta.x;
        overlap.y = aCol.halfSize.y + bCol.halfSize.y - delta.y;
    }
    return overlap;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 overlap;
    auto& aCol = a->getComponent<CBoundingBox>();
    auto& bCol = b->getComponent<CBoundingBox>();

    if (aCol.has && bCol.has)
    {
        auto& aTrans = a->getComponent<CTransform>();
        auto& bTrans = b->getComponent<CTransform>();

        Vec2 delta(abs(aTrans.prevPos.x - bTrans.prevPos.x), abs(aTrans.prevPos.y - bTrans.prevPos.y));
        overlap.x = aCol.halfSize.x + bCol.halfSize.x - delta.x;
        overlap.y = aCol.halfSize.y + bCol.halfSize.y - delta.y;
    }
    return overlap;
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
    Vec2 delta = e->getComponent<CTransform>().pos - pos;
    delta = delta.abs();

    return delta.y <= e->getComponent<CBoundingBox>().halfSize.y && delta.x <= e->getComponent<CBoundingBox>().halfSize.x;
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
    Vec2 r(b - a);
    Vec2 s(d - c);
    float rxs = r.cross(s);
    Vec2 cma = c - a;
    float t = cma.cross(s) / rxs;
    float u = cma.cross(r) / rxs;
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
    }
    else
    {
        return { false, Vec2(0,0) };
    }
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
    if (IsInside(a, e) || IsInside(b, e))
    {
        return true;
    }

    Vec2 points[4];
    auto& bb = e->getComponent<CBoundingBox>();
    auto& pos = e->getComponent<CTransform>().pos;
    points[0] = pos - bb.halfSize;
    points[1] = pos + Vec2(bb.halfSize.x, -bb.halfSize.y);
    points[2] = pos + Vec2(-bb.halfSize.x, bb.halfSize.y);
    points[3] = pos + bb.halfSize;

    return LineIntersect(a, b, points[0], points[1]).result
        || LineIntersect(a, b, points[0], points[2]).result
        || LineIntersect(a, b, points[3], points[1]).result
        || LineIntersect(a, b, points[3], points[2]).result;
}

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
