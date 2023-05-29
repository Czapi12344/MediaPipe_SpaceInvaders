#include "MonsterCluster.h"

MonsterCluster::MonsterCluster()
{
    leftEdge = 0;
    rightEdge = 0;
    gap.x = Monster().getWidth();
    gap.y = Monster().getHeight();
    nextDir = right;
    destroyedCount = 0;
    totalCount = 0;
    cooldown = 0;
}

void MonsterCluster::addRow(int numberOfMonsters)
{
    std::vector<Monster> row;
    for (int i = 0; i < numberOfMonsters; i++)
    {
        Monster monster;
        totalCount++;
        row.push_back(monster);
    }
    rows.push_back(row);
    setPosition(position.x, position.y);
}

void MonsterCluster::setPosition(float xPos, float yPos)
{
    position.x = xPos;
    position.y = yPos;
    int rowIndex = 0;
    int longestRowLength = 0;
    for (std::vector<Monster> &row : rows)
    {
        int monsterIndex = 0;
        int currentRowSize = row.size();

        float rowLength = currentRowSize * Monster().getWidth() + gap.x * (currentRowSize - 1);
        if (row.size() > longestRowLength)
        {
            longestRowLength = currentRowSize;
            leftEdge = xPos - rowLength / 2;
            rightEdge = xPos + rowLength / 2;
        }

        for (Monster &monster : row)
        {
            monster.setPosition(xPos + (monster.getWidth() + gap.x) * (float)monsterIndex - rowLength / 2., yPos + (monster.getHeight() + gap.y) * (float)rowIndex);
            monsterIndex++;
        }

        rowIndex++;
    }
}

void MonsterCluster::draw(sf::RenderWindow &gameWindow)
{
    for (std::vector<Monster> &row : rows)
    {
        for (Monster &monster : row)
        {
            monster.draw(gameWindow);
        }
    }
}

void MonsterCluster::move(float velocity, unsigned int windowWidth)
{
    for (std::vector<Monster> &row : rows)
    {
        for (Monster &monster : row)
        {
            if (nextDir == down)
            {
                monster.move(sf::Vector2f(0, velocity * 4));
            }
            else if (nextDir == right)
            {
                monster.move(sf::Vector2f(velocity, 0));
            }
            else if (nextDir == left)
            {
                monster.move(sf::Vector2f(-velocity, 0));
            }
        }
    }

    if (cooldown < 50)
    {
        cooldown++;
    }
    else if (cooldown == 50)
    {
        this->shoot();
        cooldown = 0;
    }
    updateBullets();

    if (nextDir == right)
    {
        leftEdge += velocity;
        rightEdge += velocity;
        if (rightEdge >= windowWidth)
        {
            nextDir = down;
        }

        return;
    }

    if (nextDir == left)
    {
        leftEdge -= velocity;
        rightEdge -= velocity;
        if (leftEdge <= 0)
        {
            nextDir = down;
        }

        return;
    }

    if (nextDir == down)
    {
        if (leftEdge <= 0)
        {
            nextDir = right;
        }
        else if (rightEdge >= windowWidth)
        {
            nextDir = left;
        }
    }
}

std::vector<std::vector<Monster>> &MonsterCluster::getMonsters()
{
    return this->rows;
}

void MonsterCluster::removeMonster(int row, int col)
{
    std::vector<Monster> monstersRow = rows[row];
    auto iterator = monstersRow.begin() + col;
    monstersRow.erase(iterator);
    if (monstersRow.empty())
    {
        auto iteratorRows = rows.begin() + row;
        rows.erase(iteratorRows);
    }
}

void MonsterCluster::shoot()
{
    srand(time(NULL));
    std::vector<std::vector<Monster>> &monsters = getMonsters();
    int randomPosition = rand() % monsters.back().size();
    for (int i = monsters.size() - 1; i >= 0; i--)
    {
        if (monsters.at(i).size() > randomPosition && monsters.at(i).at(randomPosition).getState() == alive)
        {
            Monster monster = monsters.at(i).at(randomPosition);
            Bullet bullet(monster.getPosition().x + (monster.getMonsterBounds().width / 2), monster.getPosition().y, -5);
            this->bulletVec.push_back(bullet);
            return;
        }
    }
}

void MonsterCluster::updateBullets()
{
    for (std::vector<Bullet>::iterator bullet = bulletVec.begin(); bullet != bulletVec.end(); bullet++)
    {
        bullet->updateBullet();
    }
}

void MonsterCluster::drawBullets(sf::RenderWindow &gameWindow)
{
    for (Bullet bullet : this->bulletVec)
    {
        bullet.draw(gameWindow);
    }
}