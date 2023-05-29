#include "Collisions.h"

Collisions::Collisions(Player &player, MonsterCluster &monsterCluster, Score &score)
{
    this->player = &player;
    this->monsterCluster = &monsterCluster;
    this->score = &score;
    this->deathCooldown = 0;
}

void Collisions::checkCollisions()
{
    std::vector<Bullet> &bullets = player->getBullets();
    std::vector<std::vector<Monster>> &allMonsters = monsterCluster->getMonsters();

    for (Bullet &bullet : bullets)
    {
        if (bullet.getState() != alive)
        {
            continue;
        }

        for (std::vector<Monster> &monstersRow : allMonsters)
        {
            for (Monster &monster : monstersRow)
            {
                if (monster.getState() != alive)
                {
                    continue;
                }
                sf::FloatRect bulletBoundingBox = bullet.getBoundingBox();
                sf::FloatRect monsterBoundingBox = monster.getMonsterBounds();

                if (bulletBoundingBox.intersects(monsterBoundingBox))
                {
                    monster.kill();
                    score->addEnemyKilled();
                    score->addScore(100);
                    bullet.hide();
                    monsterCluster->destroyedCount++;
                    return;
                }
            }
        }
    }
    if (player->getState() == alive && deathCooldown == 0)
    {
        for (Bullet &bullet : monsterCluster->bulletVec)
        {
            sf::FloatRect bulletBoundingBox = bullet.getBoundingBox();
            sf::FloatRect playerBoundingBox = player->getPlayerBounds();
            if (bulletBoundingBox.intersects(playerBoundingBox))
            {
                player->die();
                bullet.hide();
                deathCooldown++;
                return;
            }
        }
    }
    else if (deathCooldown < 50)
    {
        deathCooldown++;
    }
    else
    {
        deathCooldown = 0;
        player->setState(alive);
    }
}