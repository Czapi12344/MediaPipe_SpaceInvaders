#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "../Entity/MonsterCluster.h"
#include "../Entity/Player.h"
#include "../GameProcessing/Score.h"
class Collisions
{

private:
    MonsterCluster *monsterCluster;
    Player *player;
    Score *score;
    int deathCooldown;

public:
    Collisions(Player &player, MonsterCluster &monsterCluster, Score &score);
    void checkCollisions();
};