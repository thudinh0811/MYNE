#include "Action.h"
                          
Action::Action()
{

}
                          
Action::Action(const std::string & name, const std::string & type)
    : m_name(name)
    , m_type(type)
{

}

Action::Action(const std::string& name, const std::string& type, Vec2 pos)
    : m_name(name)
    , m_type(type)
    , m_pos(pos)
{
}

Action::Action(const std::string& name, Vec2 pos)
    : m_name(name)
    , m_pos(pos)
{

}

const std::string & Action::name() const
{
    return m_name;
}

const std::string & Action::type() const
{
    return m_type;
}

const Vec2& Action::pos() const
{
    return m_pos;
}

std::string Action::toString() const
{
    std::stringstream ss;
    ss << name() << " " << type();
    return ss.str();
}

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
