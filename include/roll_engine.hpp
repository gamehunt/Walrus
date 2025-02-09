#ifndef __ROLL_ENGINE_H
#define __ROLL_ENGINE_H 1

#include <regex>
#include <vector>
#include <string>

template<class T> class rollable {
public:
    rollable() {
    }

    virtual ~rollable() {;}

    T roll() {
        T v = do_roll();
        _result = v;
        return v;
    }

    T result() const {
        return _result;
    }

    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& expr) {
        this->expr = expr;
    };

    std::string getExpression() const {
        return expr;
    }

private:
    std::string expr;

protected:
    virtual T do_roll() = 0;

private:
    T _result;
};

class roll_engine {
public:
    class result {
    public:
        result(bool valid = false): valid(valid) {}

        bool valid;
        std::string value;
    };
    static int bounded_rand(int start, int end);
    static result eval(std::string content);
    static std::vector<std::string> split(std::string s, std::string delimiter);

private:
    static void eval_expr(result& r, std::string content);
    static void eval_list(result& r, std::string content);
    static void eval_init(result& r, std::string content);

    static const std::regex amount_regex;
    static const std::regex comment_regex;
    static const std::regex dice_regex;
    static const std::regex list_regex;
    static const std::regex initiative_regex;
};

#endif
