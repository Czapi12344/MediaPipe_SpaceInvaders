#include "Score.h"
#include <SFML/Graphics.hpp>
#include "../Entity/Player.h"
Score::Score(int playerID, sf::Font font, sf::Text text, Player &player)
{
    this->player = &player;
    this->playerID = playerID;
    this->score = 0;
    this->enemiesKilled = 0;
    this->scoreFont = font;
    this->scoreText = text;
}
void Score::addScore(int points)
{
    this->score += points;
}

void Score::addEnemyKilled()
{
    this->enemiesKilled++;
}

void Score::reset()
{
    this->score = 0;
    this->enemiesKilled = 0;
}

int Score::getScore()
{
    return this->score;
}
void Score::draw(sf::RenderWindow &window)
{

    this->scoreText.setString("Score: " + std::to_string(this->score) + "\nKilled: " + std::to_string(this->enemiesKilled) + "\nLives:" + std::to_string(player->getLives()));

    window.draw(this->scoreText);
}

void Score::saveTop10()
{
    std::ofstream outFile("top10.txt");
    if (!outFile.is_open())
    {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }

    std::vector<int> sortedScores;
    std::ifstream inFile("top10.txt");
    if (inFile.is_open())
    {
        int score;
        while (inFile >> score)
        {
            sortedScores.push_back(score);
        }
        inFile.close();
    }

    sortedScores.push_back(score);
    std::sort(sortedScores.rbegin(), sortedScores.rend());

    for (int i = 0; i < std::min(10, (int)sortedScores.size()); i++)
    {
        outFile << sortedScores[i] << "\n";
    }

    outFile.close();
}

void Score::loadTop10()
{
    std::ifstream inFile("top10.txt");
    if (!inFile.is_open())
    {
        std::cerr << "Unable to open file for reading.\n";
        return;
    }

    std::vector<int> sortedScores;
    int score;
    while (inFile >> score)
    {
        sortedScores.push_back(score);
    }
    inFile.close();

    std::sort(sortedScores.rbegin(), sortedScores.rend());

    std::cout << "Top 10 Scores:\n";
    for (int i = 0; i < std::min(10, (int)sortedScores.size()); i++)
    {
        std::cout << (i + 1) << ". " << sortedScores[i] << "\n";
    }
}
