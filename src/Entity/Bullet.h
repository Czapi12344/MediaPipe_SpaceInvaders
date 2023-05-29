#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "EntityState.h"

class Bullet
{

private:
    sf::CircleShape circle;
    int speed;
    State state;
public:
    Bullet(float x, float y, int speed);

    void updateBullet();
    void hide();
    void draw(sf::RenderWindow &gameWindow);

    sf::FloatRect getBoundingBox();

    State getState();
};
