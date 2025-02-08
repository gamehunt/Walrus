#ifndef __ROLL_ENGINE_H
#define __ROLL_ENGINE_H 1

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

    virtual std::string serialize(const T& _result) const = 0;
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
};

#endif
