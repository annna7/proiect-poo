#include <iostream>
#include "Game.h"
#include "Food.h"
#include "Individual.h"
#include "Cell.h"
#include "CellFactory.h"
#include "IndividualType.h"
#include <SFML/Graphics.hpp>


template<typename K>
void Game::produceOffspring(int pos) {
    auto freeSpot = findFreeSpot(pos, 15);
    if (freeSpot == -1) {
        return;
    }
    auto offspring = CellFactory::createSuitor<K>(freeSpot / width, freeSpot % width);

    // Each baby starts off with 3 food points at birth.
    for (int i = 0; i < 3; ++i) {
        offspring->eat();
    }
    futureBoard[freeSpot] = offspring;
}

template<typename K>
void Game::mate(std::shared_ptr<K> indiv, std::shared_ptr<Suitor<K>> suitor) {
    if (indiv == nullptr || suitor == nullptr) {
        return;
    }

    // When a couple mates, they can either produce one, two, three, four or five babies - this number gets chosen randomly.
    int offspringQuantity = randomIntegerFromInterval(1, 5);
    for (int i = 0; i < offspringQuantity; ++i) {
        produceOffspring<K>(indiv->getPosition());
    }
    std::cout << "Mating!" << std::endl;
}


Game &Game::getInstance() {
    static Game instance;
    return instance;
}

// Total number of survivors: p1 * x1 + p2 * x2 + ...
// Total number of individuals: x1 + x2 + ...
// Number of individuals of given species, proportional to their fitness: (p1 * x1 / (total number of survivors)) * (total number of individuals)
std::unordered_map<IndividualType, int> Game::computeNewGeneration() {
    std::unordered_map<IndividualType, int> newGeneration;
    int totalIndividuals = getTotalIndividuals();
    int totalSurvivors = getTotalSurvivalRate() * totalIndividuals / 100;
    if (totalSurvivors == 0) {
        return currentGeneration;
    }
    for (auto type = (IndividualType)(INDIVIDUAL_TYPE_BEGIN + 1); type != INDIVIDUAL_TYPE_END; type = (IndividualType) (type + 1)) {
        newGeneration[type] = currentGeneration[type] == 0 ? 0 : (int) ((1.0 * survivorMap[type] / currentGeneration[type]) * currentGeneration[type] * totalIndividuals) / totalSurvivors;
    }
    return newGeneration;
}

void Game::assertFitnessOfIndividual(const std::shared_ptr<Individual>& individual) {
    if (!individual->checkIfAlive()) {
        if (individual->getPosition() >= 0 && individual->getPosition() < width * height) {
            board[individual->getPosition()] = nullptr;
        }
    } else {
        if (dynamic_pointer_cast<Keystone>(individual)) {
            survivorMap[KEYSTONE_TYPE] += 1;
        } else if (std::dynamic_pointer_cast<Clairvoyant>(individual)) {
            survivorMap[CLAIRVOYANT_TYPE] += 1;
        } else if (std::dynamic_pointer_cast<RedBull>(individual)) {
            survivorMap[REDBULL_TYPE] += 1;
        } else if (std::dynamic_pointer_cast<Ascendant>(individual)) {
            survivorMap[ASCENDANT_TYPE] += 1;
        } else {
            survivorMap[SUITOR_TYPE] += 1;
        }
    }
}

void Game::computeFitness() {
    for (auto &cell : board) {
        if (cell != nullptr) {
            // check if the cell has the same type as individual
            // for instance, if I pass a Redbull, check if the cell is a Redbull
            auto individualCell = dynamic_pointer_cast<Individual>(cell);
            if (individualCell != nullptr) {
                assertFitnessOfIndividual(individualCell);
            }
        }
    }

    for (int i = 0; i < width * height; ++i) {
        updateDisplayMatrix(i);
    }

    window.clear();
    window.draw(&displayMatrix[0], displayMatrix.size(), sf::Points);
    window.display();
}

void Game::endEpoch() {
    epochCounter++;
    computeFitness();
    showStatistics();
    resetGeneration(computeNewGeneration());
    isPaused = true;
}

void Game::menuDisplay() {
    sf::Text message = sf::Text("Epoch: " + std::to_string(epochCounter) + " has ended! Press SPACE to spawn an evolved generation!", font);
    message.setPosition(20, (float) height * Cell::CELL_SIZE);
    message.setCharacterSize(15);
    window.draw(message);
}

void Game::run() {
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        menuDisplay();
        if (!isPaused) {
            sf::Time elapsed = clock.getElapsedTime(); // get the elapsed time since the last call to getElapsedTime()
            if (elapsed.asMilliseconds() >= EPOCH_DURATION) {
                endEpoch();
            } else {
                display();
            }
        } else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                isPaused = false;
                clock.restart();
            }
        }
        window.display();
    }
}

void Game::display() {
    window.clear();
    displayMatrix.clear();
    displayMatrix.resize(width * height * Cell::CELL_SIZE * Cell::CELL_SIZE);
    for (int i = 0; i < width * height; i++) {
        if (board[i] != nullptr) {
            updateDisplayMatrix(i);
            auto individual = dynamic_pointer_cast<Individual>(board[i]);
            if (individual != nullptr) {
                int coords = findFoodInRange(individual, individual->getVision());
                if (coords != -1 && dynamic_pointer_cast<Individual>(futureBoard[coords]) == nullptr) {
                    futureBoard[coords] = individual;
                    individual->setCoords(coords / height, coords % height);
                    individual->eat();
                } else {
                    individual->move();
                    int newPosition = individual->getPosition();
                    if (newPosition > width * height - 1) {
                        newPosition = newPosition % (width * height);
                    }
                    if (futureBoard[newPosition] == nullptr) {
                        futureBoard[newPosition] = individual;
                    } else if (auto individualFound = dynamic_pointer_cast<Individual>(futureBoard[newPosition])){
                        // check whether one of them is a suitor
                        // can this be done more efficiently?
                        if (checkSuitor<Clairvoyant>(individual, dynamic_pointer_cast<Clairvoyant>(individualFound))
                                || checkSuitor<RedBull>(individual, dynamic_pointer_cast<RedBull>(individualFound))
                                        || checkSuitor<Keystone>(individual, dynamic_pointer_cast<Keystone>(individualFound))
                                                || checkSuitor<Ascendant>(individual, dynamic_pointer_cast<Ascendant>(individualFound))) {}
                        int freePosition = findFreeSpot(individual->getPosition(), 5);
                        if (freePosition != -1) {
                            futureBoard[freePosition] = individual;
                        }
                    } else if (coords != -1) {
                        futureBoard[coords] = individual;
                    }
                }
            } else {
                auto individualEaten = dynamic_pointer_cast<Individual>(futureBoard[i]);
                if (!individualEaten) {
                    futureBoard[i] = board[i];
                }
            }
        }
    }
    window.draw(&displayMatrix[0], displayMatrix.size(), sf::Points);
    board = futureBoard;
    futureBoard.clear();
    futureBoard.resize(width * height);
}

Game::Game() : width(MAX_X),
               height(MAX_Y)
//               currentGeneration[KEYSTONE_TYPE](promptUser("[YELLOW] Specify the desired number of Keystone's (no special abilities, but can sustain on a small quantity of food):", 0, 600)),
//               currentGeneration[CLAIRVOYANT_TYPE](promptUser("[BLUE] Specify the desired number of Clairvoyant's (can spot food from afar):", 0, 600)),
//               currentGeneration[REDBULL_TYPE](promptUser("[RED] Specify the desired number of RedBull's (fast on their feet, but high hunger):", 0, 600)),
//               currentGeneration[ASCENDANT_TYPE](promptUser("[PINK] Specify the desired number of Ascendant's (become overpowered once they eat the first time):", 0, 600)),
//               currentGeneration[SUITOR_TYPE](promptUser("[GRAY] Specify the desired number of Suitor's (each Suitor wants to mate with a specific breed of individuals described above (which will get picked randomly).):", 0, 600)),
//               quantityOfFood(promptUser("[DARK GREEN] Specify the desired quantity of food", 0, 2500))
{
    currentGeneration[KEYSTONE_TYPE] = promptUser("[YELLOW] Specify the desired number of Keystone's (no special abilities, but can sustain on a small quantity of food):", 0, 600);
    currentGeneration[CLAIRVOYANT_TYPE] = promptUser("[BLUE] Specify the desired number of Clairvoyant's (can spot food from afar):", 0, 600);
    currentGeneration[REDBULL_TYPE] = promptUser("[RED] Specify the desired number of RedBull's (fast on their feet, but very hungry!)", 0, 600);
    currentGeneration[ASCENDANT_TYPE] = promptUser("[PINK] Specify the desired number of Ascendant's (become much stronger once they encounter food for the first time", 0, 600);
    currentGeneration[SUITOR_TYPE] = promptUser("[SUITOR] Specify the desired number of Suitor's - each Suitor wants to mate with a specific breed of Individuals, which gets generated randomly.", 0, 600);
    quantityOfFood = promptUser("[DARK GREEN] Specify the desired quantity of food", 0, 2500);
    clock.restart();
    window.create(sf::VideoMode(width * Cell::CELL_SIZE, height * Cell::CELL_SIZE + BOTTOM_BAR_HEIGHT), "Game of Life");
    initializeFont(font);
    epochCounter = 0;
    resetBoard();
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(15);
}

void Game::generateCells() {
    board.clear();
    futureBoard.clear();
    board.resize(width * height);
    futureBoard.resize(width * height);
    int lowerBound = 0;

    std::cout << getTotalIndividuals() << std::endl;

    auto randomPositions = generateRandomArray(getTotalIndividuals() + quantityOfFood, 0, width * height);
    for (int i = lowerBound; i < lowerBound + currentGeneration[KEYSTONE_TYPE]; i++) {
        board[randomPositions[i]] = CellFactory::createKeystone(randomPositions[i] / height, randomPositions[i] % height);
    }
    lowerBound += currentGeneration[KEYSTONE_TYPE];
    for (int i = lowerBound; i < lowerBound + currentGeneration[CLAIRVOYANT_TYPE]; i++) {
        board[randomPositions[i]] = CellFactory::createClairvoyant(randomPositions[i] / height, randomPositions[i] % height);
    }
    lowerBound += currentGeneration[CLAIRVOYANT_TYPE];
    for (int i = lowerBound; i < lowerBound + currentGeneration[REDBULL_TYPE]; i++) {
        board[randomPositions[i]] = CellFactory::createRedBull(randomPositions[i] / height, randomPositions[i] % height);
    }
    lowerBound += currentGeneration[REDBULL_TYPE];
    for (int i = lowerBound; i < lowerBound + currentGeneration[ASCENDANT_TYPE]; i++) {
        board[randomPositions[i]] = CellFactory::createAscendant(randomPositions[i] / height, randomPositions[i] % height);
    }
    lowerBound += currentGeneration[ASCENDANT_TYPE];
    for (int i = lowerBound; i < lowerBound + currentGeneration[SUITOR_TYPE]; i++) {
        board[randomPositions[i]] = CellFactory::createSuitor(randomPositions[i] / height, randomPositions[i] % height);
    }
    lowerBound += currentGeneration[SUITOR_TYPE];
    for (int i = lowerBound; i < lowerBound + quantityOfFood; i++) {
        board[randomPositions[i]] = CellFactory::createFood(randomPositions[i] / height, randomPositions[i] % height);
    }
}

Game::~Game() {
    std::cout << "Destructor called\n";
}

std::ostream &operator<<(std::ostream &os, const Game &game) {
    os << " width: " << game.width << " height: " << game.height << " numberOfIndividuals: " << game.getTotalIndividuals()
       << " numberOfFood: " << game.quantityOfFood;
    return os;
}

void Game::initializeDisplay() {
    displayMatrix.resize(width * height * Cell::CELL_SIZE * Cell::CELL_SIZE);
    for (int i = 0; i < height * Cell::CELL_SIZE; ++i) {
        for (int j = 0; j < width * Cell::CELL_SIZE; ++j) {
            displayMatrix[i * width * Cell::CELL_SIZE + j] = sf::Vertex(sf::Vector2f((float)j, (float)i), sf::Color::Black);
        }
    }
}

void Game::updateDisplayMatrix(int i, sf::Color color) {
    int x = i / height;
    int y = i % height;
    for (int j = 0; j < Cell::CELL_SIZE; ++j) {
        for (int k = 0; k < Cell::CELL_SIZE; ++k) {
            displayMatrix[(x * Cell::CELL_SIZE + j) * width * Cell::CELL_SIZE + y * Cell::CELL_SIZE + k] =
                    sf::Vertex(sf::Vector2f((float)(y * Cell::CELL_SIZE + k), (float)(x * Cell::CELL_SIZE + j)), color);
        }
    }
}

void Game::updateDisplayMatrix(int i) {
    if (board[i] == nullptr) {
        updateDisplayMatrix(i, sf::Color::Black);
    } else {
        updateDisplayMatrix(i, board[i]->getColor());
    }
}

int Game::findFreeSpot(int pos, int radius) {
    int x = pos / height;
    int y = pos % height;
    // check in the circle centered at (x, y) with radius i
    for (int j = x - radius; j <= x + radius; ++j) {
        for (int k = y - radius; k <= y + radius; ++k) {
            int newPos = j * height + k;
            if (newPos >= 0 && newPos < width * height && futureBoard[newPos] == nullptr) {
                return newPos;
            }
        }
    }
    return -1;
}


int Game::findFoodInRange(const std::shared_ptr<Individual>& individual, int radius) {
    int position = individual->getPosition();
    int x = position / height;
    int y = position % height;
    // check in the circle centered at (x, y) with radius i
    for (int j = x - radius; j <= x + radius; ++j) {
        for (int k = y - radius; k <= y + radius; ++k) {
            int newPos = j * height + k;
            if (newPos >= 0 && newPos < width * height && board[newPos] != nullptr) {
                auto food = std::dynamic_pointer_cast<Food>(board[newPos]);
                if (food && !std::dynamic_pointer_cast<Individual>(futureBoard[newPos])) {
                    return newPos;
                }
            }
        }
    }
    return -1;
}

int Game::getTotalIndividuals() const {
    int totalIndividuals = 0;
    for (auto individualType = (IndividualType)(INDIVIDUAL_TYPE_BEGIN + 1); individualType != INDIVIDUAL_TYPE_END; individualType = (IndividualType)(individualType + 1)) {
        totalIndividuals += currentGeneration.at(individualType);
    }
    return totalIndividuals;
}

int Game::getTotalSurvivalRate() const {
    int totalSurvivalRate = 0;
    for (auto individualType = (IndividualType)(INDIVIDUAL_TYPE_BEGIN + 1); individualType != INDIVIDUAL_TYPE_END; individualType = (IndividualType)(individualType + 1)) {
        totalSurvivalRate += survivorMap.at(individualType);
    }
    return (int) (100.0 * totalSurvivalRate / getTotalIndividuals());
}

void Game::showStatistics() {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);
    text.setPosition(20, (float)height * Cell::CELL_SIZE + 20);
    std::string output;

    for (auto type = (IndividualType)(INDIVIDUAL_TYPE_BEGIN + 1); type != INDIVIDUAL_TYPE_END; type = IndividualType(type + 1)) {
        output += individualTypeToString(type) + ": " +
                  getPercentage(survivorMap[type], currentGeneration[type]) +
                  "( " + std::to_string(survivorMap[type]) + " / " + std::to_string(currentGeneration[type]) + ")\n";
    }
    output += "Total survival rate: " + std::to_string(getTotalSurvivalRate()) + "%\n";
    text.setString(output);
    window.draw(text);
}

void Game::resetGeneration(std::unordered_map<IndividualType, int> generation) {
    for (auto individualType = (IndividualType)(INDIVIDUAL_TYPE_BEGIN + 1); individualType != INDIVIDUAL_TYPE_END; individualType = (IndividualType)(individualType + 1)) {
        currentGeneration[individualType] = generation[individualType];
        survivorMap[individualType] = 0;
    }
    resetBoard();
}

void Game::resetBoard() {
    survivorMap.clear();
    generateCells();
    initializeDisplay();
}


template <typename T>
bool Game::checkSuitor(std::shared_ptr<Individual> a, std::shared_ptr<T> b) {

    if (dynamic_pointer_cast<Suitor<T>>(a)) {
        mate<T>(b, dynamic_pointer_cast<Suitor<T>>(a));
        return true;
    }
    return false;
}

