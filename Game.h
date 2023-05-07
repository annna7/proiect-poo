#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <set>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <memory>
#include "Individual.h"
#include "Utils.h"
#include "Food.h"
#include "Cell.h"

class Game {
public:
    static Game &getInstance();
    void run();
    Game(const Game &other) = delete;
    Game& operator=(const Game &other) = delete;
    ~Game();
    friend std::ostream &operator<<(std::ostream &os, const Game &game);

private:
    std::unordered_map<int, std::pair<int, int>> fitnessCache;
    std::vector<std::shared_ptr<Cell>> board;
    std::vector<std::shared_ptr<Cell>> futureBoard;
    std::vector<sf::Vertex> displayMatrix;

    int width;
    int height;
    int numberOfIndividuals;
    int quantityOfFood;

    sf::Font font;
    sf::RenderWindow window;

    Game();
    void generateCreatures();
    void display();
    void initializeDisplay();
    void updateDisplayMatrix(int i);
    void updateDisplayMatrix(int i, sf::Color color);
    bool isPaused = false;
    int epochCounter = 0;
    static const int EPOCH_DURATION = 3000;
    static const int BOTTOM_BAR_HEIGHT = 50;
    void endEpoch();
    void menuDisplay();
    long long initialTime;
    void computeFitness();
    const static std::unordered_map<int, std::string> raceDict;
    void showStatistics();
    int findFreeSpot(const std::shared_ptr<Individual>&, int radius);
    int findFoodInRange(const std::shared_ptr<Individual>&, int radius);

};
