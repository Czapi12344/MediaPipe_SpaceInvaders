#ifndef SPACEINVADERS_SCORE_H
#define SPACEINVADERS_SCORE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../Entity/Player.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Score
{
public:
    Score(int playerID, sf::Font font, sf::Text text, Player &player);
    void addScore(int points);
    void addEnemyKilled();
    void reset();
    void draw(sf::RenderWindow &window);
    void loadTop10();
    void saveTop10();
    int getScore();

private:
    int playerID;
    int score;
    int enemiesKilled;
    Player *player;
    static const int numTopScores = 10;
    std::vector<int> topScores;
    sf::Font scoreFont;
    sf::Text scoreText;
};

#endif // SPACEINVADERS_SCORE_H
