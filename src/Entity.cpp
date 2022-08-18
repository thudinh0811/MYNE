#include "Entity.h"

Entity::Entity(const size_t & id, const std::string & tag)
    : m_tag (tag)
    , m_id  (id)
{
                          
}
                          
bool Entity::isActive() const 
{ 
    return m_active; 
}
                          
void Entity::destroy()
{ 
    m_active = false; 
}
                          
size_t Entity::id() const
{
    return m_id;
}

const std::string & Entity::tag() const
{
    return m_tag;
}


// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
