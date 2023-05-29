#pragma once

#include <list>
#include "Monster.h"
#include "Bullet.h"
#include <stdlib.h>
#include <time.h>

enum Direction
{
    left,
    right,
    down
};
class MonsterCluster
{
private:
    sf::Vector2f gap;
    std::vector<std::vector<Monster>> rows;
    sf::Vector2f position;
    float leftEdge;
    float rightEdge;

    Direction nextDir;

public:
    int destroyedCount;
    int totalCount;
    int cooldown;
    std::vector<Bullet> bulletVec;
    MonsterCluster();
    void addRow(int numberOfMonsters);
    void setPosition(float xPos, float yPos);
    void move(float velocity, unsigned int windowWidth);
    void draw(sf::RenderWindow &gameWindow);
    std::vector<std::vector<Monster>> &getMonsters();
    void removeMonster(int row, int col);
    void shoot();
    void updateBullets();
    void drawBullets(sf::RenderWindow &gameWindow);
};
