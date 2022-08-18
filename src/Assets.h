#pragma once

#include "Common.h"
#include "Animation.h"

class Assets
{
    std::map<std::string, sf::Texture>      m_textureMap;
    std::map<std::string, Animation>        m_animationMap;
    std::map<std::string, sf::Font>         m_fontMap;
    std::map<std::string, sf::SoundBuffer>  m_soundBufferMap;
    std::map<std::string, sf::Sound>        m_soundMap;
    std::map<std::string, std::string>      m_shaderMap;
    std::map<std::string, sf::Sound>        m_musicMap;

    void addTexture(const std::string & textureName, const std::string & path, bool smooth = true);
    void addAnimation(const std::string & animationName, const std::string & textureName, size_t frameCount, size_t speed);
    void addFont(const std::string & fontName, const std::string & path);
    void addSound(const std::string& fontName, const std::string& path);
    void addShader(const std::string& shaderName, const std::string& path);
    void addMusic(const std::string& fontName, const std::string& path);

public:

    Assets();

    void loadFromFile(const std::string & path);
    void setSFXVolume(int v);
    void setMusicVolume(int v);

    const sf::Texture & getTexture(const std::string & textureName) const;
    const Animation &   getAnimation(const std::string & animationName) const;
    const sf::Font &    getFont(const std::string & fontName) const;
    sf::Sound&          getSound(const std::string& soundName);
    sf::Sound&          getMusic(const std::string& soundName);
    const std::string&   getShader(const std::string& shaderName);
};

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
