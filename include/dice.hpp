#ifndef __DICE_H
#define __DICE_H 1

#include <vector>
#include "roll_engine.hpp"

class dice: public rollable<unsigned int> {
public:
    dice(): rollable() {;}

    virtual void deserialize(const std::string&) override;
    virtual std::string serialize() const override;

    int getSides();

    const std::vector<int>& getSubresults();

protected:
    virtual unsigned int do_roll() override;

private:
    unsigned int amount, sides;

    std::vector<int> subresults;
};

#endif
