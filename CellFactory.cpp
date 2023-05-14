//
// Created by anna on 5/14/23.
//

#include "CellFactory.h"
#include "Utils.h"

std::shared_ptr<Ascendant> CellFactory::createAscendant(int x, int y) {
    return std::make_shared<Ascendant>(x, y);
}

std::shared_ptr<RedBull> CellFactory::createRedBull(int x, int y) {
    return std::make_shared<RedBull>(x, y);
}

std::shared_ptr<Keystone> CellFactory::createKeystone(int x, int y) {
    return std::make_shared<Keystone>(x, y);
}

std::shared_ptr<Clairvoyant> CellFactory::createClairvoyant(int x, int y) {
    return std::make_shared<Clairvoyant>(x, y);
}

std::shared_ptr<Food> CellFactory::createFood(int x, int y) {
    return std::make_shared<Food>(x, y);
}

template<typename IndividualType>
std::shared_ptr<Suitor<IndividualType>> CellFactory::createSuitor(int x, int y) {
    return std::shared_ptr<Suitor<IndividualType>>();
}

std::shared_ptr<Individual> CellFactory::createSuitor(int x, int y) {
    switch (randomIntegerFromInterval(0, 3)) {
        case 0:
            return std::make_shared<Suitor<Ascendant>>(x, y);
        case 1:
            return std::make_shared<Suitor<RedBull>>(x, y);
        case 2:
            return std::make_shared<Suitor<Keystone>>(x, y);
        case 3:
            return std::make_shared<Suitor<Clairvoyant>>(x, y);
        default:
            return nullptr;
    }
}