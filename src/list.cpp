#include "list.hpp"
#include <algorithm>
#include <random>
#include <regex>
#include <stdexcept>

void choice_list::deserialize(const std::string& expr)  {
    rollable::deserialize(expr);
    if(expr[0] != 'l') {
        amount = std::stoi(expr);
    } else {
        amount = 1;
    }

    const std::regex list_regex("\\[(([^;]+;?)+)\\]");
    std::smatch list_match;

    if(std::regex_search(expr, list_match, list_regex)) {
        std::string vars_str = (*list_match.begin()).str();
        vars_str = vars_str.substr(1, vars_str.size() - 2);
        variants = roll_engine::split(vars_str, ";");
    } else {
        throw std::invalid_argument("inv syntax");
    }

    if(amount <= 0 || variants.empty()) {
        throw std::invalid_argument("inv amount & var");
    }
}

std::string choice_list::serialize() const  {
    std::string s;
    for(std::string& ss : result()) {
        s += ss;
        s += ", ";
    }
    return s.substr(0, s.size() - 2);
}

std::list<std::string> choice_list::do_roll()  {
    std::list<std::string> r;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(variants.begin(), variants.end(), g);
    for(int i = 0; i < amount; i++) {
        r.push_back(variants[i]);
    }
    return r;
}
