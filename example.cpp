#include <functional>
using std::function;

enum class State { Fail, Success, Running };

struct Rule {
    function<State(Rule&)> execute;
};

static Rule null_rule;

Rule operator!(Rule wrapme) {
    return {
        [=](Rule& r){
            const auto result = wrapme.execute(r);
            return result == State::Fail ? State::Success :
                   result == State::Success ? State::Fail :
                   State::Running;
        }
    };
}

Rule operator&&(Rule left, Rule right) {
    return {
        [=](Rule &r) {
            const auto lresult = left.execute(r);
            const auto rresult = right.execute(r);
            return (lresult == State::Fail || rresult == State::Fail) ? State::Fail :
                   (lresult == State::Success && rresult == State::Success) ? State::Success :
                   State::Running;
        }
    };
}

Rule always_true()    { return {[](Rule r){ return State::Success; }}; }
Rule always_false()   { return !always_true(); }
Rule always_running() { return {[](Rule r){ return State::Running; }}; }

/*************************************************************************/
/**                               TEST                                  **/
/*************************************************************************/

#include <iostream>
#include <sstream>
using std::cout;
using std::ostream;
ostream& operator<<(ostream& ost, State st);


int main() {
    const auto my_complicated_rule = !!!!!!!!always_true();
    
    std::cout << "true:             " << always_true().execute(null_rule) << '\n';
    std::cout << "false:            " << always_false().execute(null_rule) << '\n';
    std::cout << "it's complicated: " << my_complicated_rule.execute(null_rule) << '\n';
    std::cout << "false && false:   " << (always_false() && always_false()).execute(null_rule) << '\n';
    std::cout << "true && false:    " << (always_true() && always_false()).execute(null_rule) << '\n';
    std::cout << "true && true:     " << (always_true() && always_true()).execute(null_rule) << '\n';
    std::cout << "running && true:  " << (always_running() && always_true()).execute(null_rule) << '\n';
    return 0;
}

ostream& operator<<(ostream& ost, State st) {
    switch(st) {
    case State::Fail:
        ost << "Fail";
        break;
    case State::Success:
        ost << "Success";
        break;
    case State::Running:
        ost << "Running";
        break;
    default:
        ost << "ERROR";
        break;
    }
    return ost;
}
