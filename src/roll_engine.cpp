#include "roll_engine.hpp"
#include "cparse/shunting-yard.h"
#include "dice.hpp"
#include "initiative.hpp"
#include "list.hpp"
#include <format>
#include <experimental/random>
#include <regex>

const std::regex roll_engine::amount_regex("\\d+#");
const std::regex roll_engine::dice_regex("\\d*d\\d+");
const std::regex roll_engine::list_regex("\\d*l\\[([^;]+;?)+\\]");
const std::regex roll_engine::initiative_regex("init\\[(\\([^;]+;[\\+\\-]\\d+\\*?\\),?)+\\]");
const std::regex roll_engine::comment_regex("#.*");

int roll_engine::bounded_rand(int start, int end) {
    return std::experimental::randint(start, end);
}

std::vector<std::string> roll_engine::split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}


void roll_engine::eval_expr(result& r, std::string content) {
    std::smatch dice_match;
    std::smatch comment_match;

    char dice_placeholder = 'a';

    cparse::TokenMap vars;

    std::string comment = "";
    if(std::regex_search(content, comment_match, comment_regex)) {
        auto sub = *comment_match.begin();
        comment = sub.str().substr(1);
        content = std::regex_replace(content, comment_regex, "");
    }

    std::vector<std::string> dices;

    while(std::regex_search(content, dice_match, dice_regex)) {
        auto match = *dice_match.begin();
        dice d;
        std::string match_str = match.str();
        d.deserialize(match_str);
        std::string str = std::string(1, dice_placeholder);
        int roll = d.roll();
        vars[str] = roll;
        content = std::regex_replace(content, dice_regex, str, std::regex_constants::format_first_only);
        std::string subresults;
        for(int r : d.getSubresults()) {
            if(r == 1 || r == d.getSides()) {
                subresults += std::format("**{}**, ", r);
            } else {
                subresults += std::to_string(r) + ", ";
            }
        }
        subresults = subresults.substr(0, subresults.size() - 2);
        dices.push_back(std::format("[{}] {}", subresults, d.getExpression()));
        dice_placeholder++;
    }

    double v = cparse::calculator::calculate(content.c_str(), &vars).asDouble();
    for(char a = 'a'; a < dice_placeholder; a++) {
        content = std::regex_replace(content, std::regex(std::string(1, a)), dices[a - 'a']);
    }
    r.value  = std::format("{}` {} ` ⟵ {}", comment.empty() ? "" : comment + ", ", v, content);
}

void roll_engine::eval_list(result& r, std::string content) {
    std::smatch comment_match;

    std::string comment = "";
    if(std::regex_search(content, comment_match, comment_regex)) {
        auto sub = *comment_match.begin();
        comment = sub.str().substr(1);
        content = std::regex_replace(content, comment_regex, "");
    }

    choice_list l;
    l.deserialize(content);
    l.roll();
    r.value = std::format("{}` {} ` ⟵ {}", comment.empty() ? "" : comment + ", ", l.serialize(), content);
}

void roll_engine::eval_init(result& r, std::string content) {
    initiative init;
    init.deserialize(content);
    init.roll();
    r.value = init.serialize();
}

roll_engine::result roll_engine::eval(std::string content) {
    roll_engine::result r(true);

    std::smatch amount_match;
    std::smatch list_match;
    std::smatch init_match;

    int amount = 1;
    if(std::regex_search(content, amount_match, amount_regex, std::regex_constants::match_continuous)) {
        auto sub = *amount_match.begin();
        amount = std::stoi(sub.str());
        content = std::regex_replace(content, amount_regex, "", std::regex_constants::match_continuous);
    }

    if(amount > 1) {
        for(int i = 0; i < amount; i++) {
            roll_engine::result sub_result = eval(content);
            if(!sub_result.valid) {
                r.valid = false;
                return r;
            }
            while(sub_result.value.ends_with(" ")) {
                sub_result.value = sub_result.value.substr(0, sub_result.value.size() - 1);
            }
            r.value += sub_result.value + "\n";
        }
    } else if(std::regex_search(content, list_match, list_regex, std::regex_constants::match_continuous)){
        eval_list(r, (*list_match.begin()).str());
    } else if(std::regex_search(content, init_match, initiative_regex, std::regex_constants::match_continuous)) {
        eval_init(r, (*init_match.begin()).str());
    } else if(std::regex_search(content, dice_regex)){
        eval_expr(r, content);
    } else if(content.starts_with("&") || content.starts_with("r")) {
        eval_expr(r, content.substr(1));
    } else {
        r.valid = false;
    }

    return r;
}
