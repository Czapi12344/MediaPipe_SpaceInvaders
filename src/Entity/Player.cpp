#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Player.h"
#include "Bullet.h"

#define WINDOW_Y_OFFSET 40
#define MOVEMENT_MARGIN 10

void Player::initPhysics()
{

    this->maxVelocity = 10.f;
    this->minVelocity = 1.f;
    this->acceleration = 3.f;
    this->slow = 0.95f;
}

void Player::initShip()
{

    this->shipTexture.loadFromFile("../resources/player/s1.png");

    this->shipTexture.setSmooth(true);
    this->shipSprite.setTexture(this->shipTexture);
    this->shipSprite.scale(sf::Vector2f(0.1f, 0.1f));
}

Player::Player(sf::RenderWindow &gameWindow)
{
    this->gameWindow = &gameWindow;
    this->initPhysics();
    this->initShip();
    this->isFiring = false;
    this->score = 0;
    this->offset.x = gameWindow.getPosition().x + shipSprite.getGlobalBounds().width / 2;
    this->offset.y = gameWindow.getPosition().y + WINDOW_Y_OFFSET + shipSprite.getGlobalBounds().height / 2;
    this->destination.x = -1;
    this->destination.y = -1;
    cooldown = 0;
    lives = 3;
}

Player::~Player()
{
}

void Player::setPosition(float x, float y)
{
    this->spawnPosition.x = x;
    this->spawnPosition.y = y;
    this->shipSprite.setPosition(x, y);
}

sf::Vector2f Player::getPosition()
{
    return this->shipSprite.getPosition();
}

void Player::shoot()
{
    if (isFiring)
    {
        return;
    }

    isFiring = true;
    Bullet bullet(this->shipSprite.getPosition().x + (this->shipSprite.getGlobalBounds().width / 2), this->shipSprite.getPosition().y, 5);
    this->bulletVec.push_back(bullet);
}

void Player::move(float dirX, float dirY)
{

    this->velocity.x += dirX * acceleration;
    this->velocity.y += dirY * acceleration;

    if (std::abs(this->velocity.x) > this->maxVelocity)
    {
        if (this->velocity.x < 0)
        {
            this->velocity.x = this->maxVelocity * -1.f;
        }
        else
        {
            this->velocity.x = this->maxVelocity;
        }
    }

    if (std::abs(this->velocity.y) > this->maxVelocity)
    {
        if (this->velocity.y < 0)
        {
            this->velocity.y = this->maxVelocity * -1.f;
        }
        else
        {
            this->velocity.y = this->maxVelocity;
        }
    }
}

void Player::move()
{
    sf::Vector2i dir;
    dir.x = destination.x - getPosition().x;
    dir.y = destination.y - getPosition().y;
    this->velocity.x += dir.x * acceleration;
    this->velocity.y += dir.y * acceleration;

    if (dir.x < MOVEMENT_MARGIN && dir.x > -MOVEMENT_MARGIN)
    {
        this->velocity.x = 0;
    }
    else if (std::abs(this->velocity.x) > this->maxVelocity)
    {
        if (this->velocity.x < 0)
        {
            this->velocity.x = this->maxVelocity * -1.f;
        }
        else
        {
            this->velocity.x = this->maxVelocity;
        }
    }

    if (dir.y < MOVEMENT_MARGIN && dir.y > -MOVEMENT_MARGIN)
    {
        this->velocity.y = 0;
    }
    else if (std::abs(this->velocity.y) > this->maxVelocity)
    {
        if (this->velocity.y < 0)
        {
            this->velocity.y = this->maxVelocity * -1.f;
        }
        else
        {
            this->velocity.y = this->maxVelocity;
        }
    }
}

void Player::updateMovement()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        this->destination = sf::Mouse::getPosition() - this->offset;
        std::cout << "X: " << destination.x << "  Y: " << destination.y << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        this->destination.x = -1;
        this->move(0.f, -0.4f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        this->destination.x = -1;
        this->move(0.f, 0.4f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        this->destination.x = -1;
        this->move(-1.f, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        this->destination.x = -1;
        this->move(1.f, 0.f);
    }
    if (destination.x != -1)
    {
        this->move();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    {
        shoot();
    }
}

void Player::updatePhysics()
{

    this->velocity *= this->slow;

    if (std::abs(this->velocity.x) < this->minVelocity)
    {
        this->velocity.x = 0.f;
    }
    if (std::abs(this->velocity.y) < this->minVelocity)
    {
        this->velocity.y = 0.f;
    }

    this->shipSprite.move(this->velocity);
}

void Player::updateBullets()
{
    for (std::vector<Bullet>::iterator bullet = bulletVec.begin(); bullet != bulletVec.end(); bullet++)
    {
        bullet->updateBullet();
    }
}

void Player::update()
{

    this->updateMovement();
    this->updatePhysics();

    // left
    if (this->shipSprite.getPosition().x < 0)
    {
        this->velocity.x = 0;
        setPosition(0, this->shipSprite.getPosition().y);
    }

    // up
    if (this->shipSprite.getPosition().y < 0)
    {
        this->velocity.y = 0;
        setPosition(this->shipSprite.getPosition().x, 0);
    }

    // bottom
    if (this->shipSprite.getGlobalBounds().top + this->shipSprite.getGlobalBounds().height > this->gameWindow->getSize().y)
    {
        this->velocity.y = 0;
        setPosition(this->shipSprite.getGlobalBounds().left, this->gameWindow->getSize().y - this->shipSprite.getGlobalBounds().height);
    }

    // right
    if (this->shipSprite.getGlobalBounds().left + this->shipSprite.getGlobalBounds().width > this->gameWindow->getSize().x)
    {
        this->velocity.x = 0;
        setPosition(this->gameWindow->getSize().x - this->shipSprite.getGlobalBounds().width, this->shipSprite.getGlobalBounds().top);
    }

    if (cooldown < 10 && isFiring == true)
    {
        cooldown++;
    }
    else if (cooldown == 10)
    {
        isFiring = false;
        cooldown = 0;
    }
    updateBullets();
}

void Player::draw(sf::RenderWindow &gameWindow)
{
    gameWindow.draw(this->shipSprite);
}

void Player::drawBullets(sf::RenderWindow &gameWindow)
{
    for (Bullet bullet : this->bulletVec)
    {
        bullet.draw(gameWindow);
    }
}

std::vector<Bullet> &Player::getBullets()
{
    return this->bulletVec;
}

sf::FloatRect Player::getPlayerBounds()
{
    return shipSprite.getGlobalBounds();
}

void Player::setDestination(sf::Vector2f destination)
{
    this->destination.x = destination.x;
    this->destination.y = destination.y;
}

int Player::getLives()
{
    return this->lives;
}

void Player::die()
{
    if (this->lives > 0)
    {
        this->setPosition(this->spawnPosition.x, this->spawnPosition.y);
        this->lives--;
        this->state = destroyed;
    }
}

State Player::getState()
{
    return this->state;
}
void Player::setState(State state)
{
    this->state = state;
}