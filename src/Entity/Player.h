#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Bullet.h"

class Player
{
private:
    int playerNum;
    int score;
    bool isFiring;
    int cooldown;
    sf::RenderWindow *gameWindow;
    sf::Vector2f velocity;
    float maxVelocity;
    float minVelocity;
    float acceleration;
    float slow;
    int lives;
    State state;
    sf::Vector2i offset;
    sf::Vector2i destination;
    sf::Vector2f spawnPosition;

    sf::Texture shipTexture;
    sf::Sprite shipSprite;

    void initPhysics();
    void initShip();

    std::vector<Bullet> bulletVec;

public:
    Player(sf::RenderWindow &gameWindow);
    virtual ~Player();

    void setPosition(float x, float y);
    sf::Vector2f getPosition();

    void move(float dirX, float dirY);
    void move();
    void shoot();
    int getLives();
    State getState();
    void setState(State state);
    void die();

    void updateMovement();
    void updatePhysics();
    void updateBullets();
    void update();

    void draw(sf::RenderWindow &gameWindow);
    void drawBullets(sf::RenderWindow &gameWindow);
    void setDestination(sf::Vector2f destination);
    std::vector<Bullet> &getBullets();
    sf::FloatRect getPlayerBounds();
};
