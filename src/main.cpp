///\/\/\\\\/\/\\\/\/\/\/\/\\\/\\\/\/\/\\\\/\/\\\/\/\/\/\/\\\/\\\
//
//  Assignment       COMP4300 - Assignment 4
//  Professor:       David Churchill
//  Year / Term:     2021FALL
//  File Name:       main.cpp
// 
//  Student Name:    Kyle Price
//  Student User:    keprice
//  Student Email:   keprice@mun.ca
//  Student ID:      201707320
//
//  Student Name:    Thu Dinh
//  Student User:    tvadinh
//  Student Email:   tvadinh@mun.ca
//  Student ID:      201843257
//
//  Group Member(s): Thu Dinh, Kyle Price
//
///\/\/\\\\/\/\\\/\/\/\/\/\\\/\\\/\/\/\\\\/\/\\\/\/\/\/\/\\\/\\\

#include <SFML/Graphics.hpp>
                          
#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    if (!sf::Shader::isAvailable())
    {
        std::cerr << "Shaders not available" << std::endl;
        return -1;
    }

    GameEngine g("assets.txt");
    g.run();
}

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
