#include "Monster.h"

sf::Texture Monster::texture;

Monster::Monster()
{
    texture.loadFromFile("../resources/monster/k1.png");
    texture.setSmooth(true);
    sprite.setTexture(texture);
    sprite.scale(sf::Vector2f(0.5f, 0.5f));
    spriteSize.y = sprite.getGlobalBounds().height;
    spriteSize.x = sprite.getGlobalBounds().width;
    state = alive;
}

Monster::~Monster() = default;

Monster::Monster(const Monster &monster)
{
    this->sprite = monster.sprite;
    this->spriteSize = monster.spriteSize;
    this->state = monster.state;
}

void Monster::move(sf::Vector2f velocity)
{
    sprite.move(velocity);
}

void Monster::draw(sf::RenderWindow &gameWindow)
{
    if (state == alive)
    {
        gameWindow.draw(sprite);
    }
}

void Monster::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

float Monster::getHeight()
{
    return spriteSize.y;
}

float Monster::getWidth()
{
    return spriteSize.x;
}

sf::FloatRect Monster::getMonsterBounds()
{
    return sprite.getGlobalBounds();
}

Monster Monster::operator=(const Monster &monster)
{
    return Monster(monster);
}

void Monster::kill()
{
    state = destroyed;
}

State Monster::getState()
{
    return state;
}

sf::Vector2f Monster::getPosition()
{
    return this->sprite.getPosition();
}
