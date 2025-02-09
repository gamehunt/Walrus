#include <algorithm>
#include <dice.hpp>
#include <functional>
#include <stdexcept>

void dice::deserialize(const std::string& expr) {
    rollable::deserialize(expr);
    if(expr[0] != 'd') {
        amount = std::stoi(expr);
    } else {
        amount = 1;
    }
    if(amount <= 0) {
        throw std::invalid_argument("amount <= 0");
    }
    auto start = expr.begin();
    while(*start != 'd') {
        start++;
    }
    sides = std::stoi(expr.substr(start - expr.begin() + 1)); 
    if(sides <= 1) {
        throw std::invalid_argument("sides <= 1");
    }
}

std::string dice::serialize() const {
    return std::to_string(result());
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
