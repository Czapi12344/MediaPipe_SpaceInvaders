#include "Bullet.h"
#include <iostream>

Bullet::Bullet(float x, float y, int speed)
{
    this->circle.setPosition(x - 5, y);
    this->circle.setRadius(5);
    this->circle.setOutlineColor(sf::Color::White);
    this->circle.setOutlineThickness(0);
    this->speed = speed;
    this->state = alive;
}

void Bullet::draw(sf::RenderWindow &gameWindow)
{
    if(state == alive){
        gameWindow.draw(this->circle);
    }
}

void Bullet::updateBullet()
{
    this->circle.setPosition(this->circle.getPosition().x, this->circle.getPosition().y - this->speed);
}

sf::FloatRect Bullet::getBoundingBox(){
    return circle.getGlobalBounds();
}

State Bullet::getState() {
    return state;
}

void Bullet::hide() {
    state = destroyed;
}
