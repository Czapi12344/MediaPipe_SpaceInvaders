#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Entity/Player.h"
#include "Entity/MonsterCluster.h"
#include "GameProcessing/Collisions.h"
#include "HandTracking/HandTracker.h"
#include "GameProcessing/Score.h"
#define SPRITE_SPEED 400

void initText(sf::Font &font, sf::Text &text) {
    if (!font.loadFromFile("../resources/fonts/ALBAS.TTF"))
    {
        std::cout << "Error loading font file" << std::endl;
    }
    text.setFont(font);
    text.setCharacterSize(25);
    text.setFillColor(sf::Color::White);
    text.setPosition(10, 10);
}

void detectHandMovement(HandTracker &handTracker, Player &player) {
    sf::Vector2f wrist = handTracker.getWrist();
    sf::Vector2f handCoord = handTracker.convert(wrist);
    player.setDestination(handCoord);

    if (handTracker.isFistClosed())
    {
        player.shoot();
    }
}

void drawScene(sf::RenderWindow &window, Player &player, MonsterCluster &cluster, Score &score) {
    player.draw(window);
    player.drawBullets(window);
    cluster.draw(window);
    cluster.drawBullets(window);
    score.draw(window);
    window.display();
}

int main(int argc, char **argv)
{

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width, desktopMode.height, desktopMode.bitsPerPixel),
                            "Space Invaders", sf::Style::Default);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);

    Player player(window);
    player.setPosition(window.getSize().x / 2., window.getSize().y / 2.);

    MonsterCluster cluster;
    cluster.addRow(15);
    cluster.addRow(15);
    cluster.addRow(15);
    cluster.setPosition((float)(window.getSize().x / 2.), (float)(window.getSize().y / 10.));

    sf::Font font;
    sf::Text text;
    initText(font, text);

    Score score(1, font, text, player);
    Collisions collisionsProcessor(player, cluster, score);

    sf::Clock timer;
    sf::Time timeEnd;
    HandTracker handTracker(window.getView().getSize());

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                window.close();
            }
        }
        timeEnd = timer.restart();

        window.clear(sf::Color::Black);

        detectHandMovement(handTracker, player);
        player.update();
        cluster.move(SPRITE_SPEED / 3 * timeEnd.asSeconds(), window.getSize().x);
        collisionsProcessor.checkCollisions();

        drawScene(window, player, cluster, score);
    }

    handTracker.close();

    return 0;
}