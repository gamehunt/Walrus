#ifndef __INIT_H
#define __INIT_H 1

#include <map>
#include "roll_engine.hpp"

class initiative: public rollable<std::map<std::string, int>> {
public:
    initiative(): rollable() {;}

    virtual void deserialize(const std::string&) override;
    virtual std::string serialize() const override;

protected:
    virtual std::map<std::string, int> do_roll() override;

private:
    std::map<std::string, std::pair<int, bool>> contestants;
    std::map<std::string, std::vector<int>> subresults;
};

#endif
