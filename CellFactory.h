//
// Created by anna on 5/14/23.
//

#ifndef OOP_CELLFACTORY_H
#define OOP_CELLFACTORY_H


#include "Individual.h"
#include "Utils.h"
#include "Ascendant.h"
#include "Suitor.h"
#include "RedBull.h"
#include "Keystone.h"
#include "Clairvoyant.h"

class CellFactory {
public:
    static std::shared_ptr<Ascendant> createAscendant(int x, int y);
    static std::shared_ptr<RedBull> createRedBull(int x, int y);
    static std::shared_ptr<Keystone> createKeystone(int x, int y);
    static std::shared_ptr<Clairvoyant> createClairvoyant(int x, int y);
    static std::shared_ptr<Individual> createIndividual(int x, int y, IndividualType type);
    template<typename IndividualType>
    static std::shared_ptr<Suitor<IndividualType>> createSuitor(int x, int y);

    static std::shared_ptr<Individual> createSuitor(int x, int y);

    static std::shared_ptr<Food> createFood(int x, int y);
};

template <typename IndividualType>
std::shared_ptr<Suitor<IndividualType>> CellFactory::createSuitor(int x, int y) {
    return std::make_shared<Suitor<IndividualType>>(x, y);
}


#endif //OOP_CELLFACTORY_H
