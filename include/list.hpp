#ifndef __LIST_H
#define __LIST_H 1

#include <list>
#include <vector>
#include "roll_engine.hpp"

class choice_list: public rollable<std::list<std::string>> {
public:
    choice_list(): rollable() {;}

    virtual void deserialize(const std::string&) override;
    virtual std::string serialize() const override;

protected:
    virtual std::list<std::string> do_roll() override;

private:
    unsigned int amount;
    std::vector<std::string> variants;
};

#endif
