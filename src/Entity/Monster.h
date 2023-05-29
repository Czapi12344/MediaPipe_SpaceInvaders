#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "EntityState.h"

class Monster
{
private:
    static sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f spriteSize;
    State state;

public:
    Monster();
    ~Monster();
    Monster(const Monster &monster);
    void move(sf::Vector2f velocity);
    void draw(sf::RenderWindow &gameWindow);
    void setPosition(float x, float y);
    float getHeight();
    float getWidth();
    sf::Vector2f getPosition();
    void kill();
    State getState();

    sf::FloatRect getMonsterBounds();

    Monster operator=(const Monster &monster);
};
