#include "initiative.hpp"
#include <format>
#include <regex>
#include <stdexcept>

void initiative::deserialize(const std::string& expr)  {
    rollable::deserialize(expr);
    const std::regex init_list_regex("\\[(\\([^;]+;[\\+\\-]\\d+\\*?\\),?)+\\]");
    std::smatch init_list_match;
    std::string list;
    if(std::regex_search(expr, init_list_match, init_list_regex)) {
       list = (*init_list_match.begin()).str(); 
       list = list.substr(1, list.size() - 2);
       std::vector<std::string> contestants_list = roll_engine::split(list, ",");
       for(auto& c : contestants_list) {
           std::string cc = c.substr(1, c.size() - 2);
           std::vector<std::string> contestant = roll_engine::split(cc, ";");
           if(contestant.size() != 2) {
               throw std::invalid_argument("contestant.size() != 2");
           }
           std::string name = contestant[0];
           std::string mod  = contestant[1];
           bool adv = mod.ends_with("*");
           int modifier = 0;
           if(adv) {
               modifier = std::stoi(mod.substr(1, mod.size() - 2));
           } else {
               modifier = std::stoi(mod);
           }
           contestants[name] = {modifier, adv};
       }
    } else {
        throw std::invalid_argument("inv syntax");
    }
}

std::string initiative::serialize() const  {
    auto r = result();
    std::vector<std::pair<int, std::string>> sorted;
    for(auto [name, value] : r) {
        sorted.push_back({value, name});
    }
    std::sort(sorted.begin(), sorted.end(), [this](
                const std::pair<int, std::string>& a, 
                const std::pair<int, std::string>& b) -> bool {
        int mod_a = contestants.at(a.second).first;
        int mod_b = contestants.at(a.second).first;
        return a.first == b.first ? mod_a >= mod_b : a.first > b.first;
    });
    std::string formatted;
    for(auto p : sorted) {
        if(!subresults.contains(p.second)) {
            formatted += std::format("__{}__ (`{}`) - ", p.second, p.first);
        } else {
            std::string subr;
            for(auto& sr : subresults.at(p.second)) {
                int raw = sr - contestants.at(p.second).first;
                if(raw == 20 || raw == 1) {
                    subr += std::format("**{}**, ", sr);
                } else {
                    subr += std::to_string(sr) + ", ";
                }
            }
            formatted += std::format("__{}__ (`{}` ⟵ [{}]) - ", p.second, p.first, subr.substr(0, subr.size() - 2));
        }
    }
    return formatted.substr(0, formatted.size() - 3);
}

std::map<std::string, int> initiative::do_roll()  {
    std::map<std::string, int> r;
    for(auto [k, v] : contestants) {
        int res1 = roll_engine::bounded_rand(1, 20) + v.first;
        int res2 = roll_engine::bounded_rand(1, 20) + v.first;
        int res  = 0;
        if(v.second) {
            res = std::max(res1, res2);
            subresults[k].push_back(res1);
            subresults[k].push_back(res2);
            std::sort(subresults[k].begin(), subresults[k].end(), std::greater<int>());
        } else {
            res = res1;
        }
        r[k] = res;
    }
    return r;
}
