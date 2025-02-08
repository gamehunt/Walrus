#include <algorithm>
#include <dice.hpp>
#include <functional>
#include <utility>

void dice::deserialize(const std::string& expr) {
    rollable::deserialize(expr);
    if(expr[0] != 'd') {
        amount = std::stoi(expr);
    } else {
        amount = 1;
    }
    auto start = expr.begin();
    while(*start != 'd') {
        start++;
    }
    sides = std::stoi(expr.substr(start - expr.begin() + 1)); 
}

std::string dice::serialize(const unsigned int& result) const {
    return std::to_string(result);
}

int dice::getSides() {
    return sides;
}

unsigned int dice::do_roll() {
    int s = 0;
    for(int i = 0; i < amount; i++) {
        int r = roll_engine::bounded_rand(1, sides);
        s += r;
        subresults.push_back(r);
    }
    std::sort(subresults.begin(), subresults.end(),  std::greater<int>());
    return s;
}

const std::vector<int>& dice::getSubresults() {
    return subresults;
}
