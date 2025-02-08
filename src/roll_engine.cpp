#include "roll_engine.hpp"
#include "cparse/shunting-yard.h"
#include "dice.hpp"
#include <format>
#include <iostream>
#include <experimental/random>
#include <regex>

int roll_engine::bounded_rand(int start, int end) {
    return std::experimental::randint(start, end);
}

roll_engine::result roll_engine::eval(std::string content) {
    roll_engine::result r;

    int amount = 1;
    
    const std::regex amount_regex("\\d+#");
    std::smatch amount_match;

    if(std::regex_search(content, amount_match, amount_regex, std::regex_constants::match_continuous)) {
        auto sub = *amount_match.begin();
        amount = std::stoi(sub.str());
        content = std::regex_replace(content, amount_regex, "");
    }

    if(amount > 1) {
        for(int i = 0; i < amount; i++) {
            roll_engine::result sub_result = eval(content);
            if(!sub_result.valid) {
                r.valid = false;
                return r;
            }
            r.value += sub_result.value + "\n";
        }
    } else {
        const std::regex dice_regex("\\d*d\\d+");
        std::smatch dice_match;
        char dice_placeholder = 'a';

        cparse::TokenMap vars;

        std::string original_content = content;

        while(std::regex_search(content, dice_match, dice_regex)) {
            auto match = *dice_match.begin();
            dice d;
            std::string match_str = match.str();
            d.deserialize(match_str);
            std::string str = std::string(1, dice_placeholder);
            int roll = d.roll();
            vars[str] = roll;
            content = std::regex_replace(content, dice_regex, str);
            std::string subresults;
            for(int r : d.getSubresults()) {
                if(r == 1 || r == d.getSides()) {
                    subresults += std::format("**{}**, ", r);
                } else {
                    subresults += std::to_string(r) + ", ";
                }
            }
            subresults = subresults.substr(0, subresults.size() - 2);
            original_content = std::regex_replace(original_content, dice_regex, std::format("[{}] {}", subresults, match_str));
            dice_placeholder++;
        }

        try {
            double v = cparse::calculator::calculate(content.c_str(), &vars).asDouble();
            r.value  = std::format("` {} ` ⟵ {}", v, original_content);
        } catch(std::exception e) {
            r.valid = false;
            return r;
        }
    }

    r.valid = true;
    return r;
}
