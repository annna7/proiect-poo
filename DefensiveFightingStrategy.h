//
// Created by anna on 5/21/23.
//

#ifndef OOP_DEFENSIVEFIGHTINGSTRATEGY_H
#define OOP_DEFENSIVEFIGHTINGSTRATEGY_H

#include "FightingStrategy.h"
#include "FightingOutcome.h"
#include <memory>

class DefensiveFightingStrategy : public FightingStrategy {
public:
    FightingOutcome fight(const std::shared_ptr<FightingStrategy> &other) override;
    sf::Color getColor() override;
    [[nodiscard]] std::shared_ptr<FightingStrategy> clone() const override {
        return std::make_shared<DefensiveFightingStrategy>(*this);
    }
};


#endif //OOP_DEFENSIVEFIGHTINGSTRATEGY_H
