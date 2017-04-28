#pragma once

namespace moar_rulez {
  enum class State { Fail, Success, Running };

  // Rule factory
  
  template<typename TExecute> struct Rule { TExecute execute; };

  constexpr inline auto make_rule(auto f) {
    return Rule<decltype(f)>{f};
  }
  
  // Logic operators
  
  constexpr inline auto operator~(Rule<auto> rule) {
    return make_rule([rule]{
      const auto result = rule.execute();
      return result == State::Fail ? State::Success :
             result == State::Success ? State::Fail :
             State::Running;
    });
  }

  constexpr inline auto operator&&(Rule<auto> left, Rule<auto> right) {
    return make_rule([left, right]{
      const auto lresult = left.execute();
      if(lresult == State::Fail || lresult == State::Running) return lresult;
      const auto rresult = right.execute();
      if(rresult == State::Fail || rresult == State::Running) return rresult;
      return State::Success;
    });
  }

  constexpr inline auto operator||(Rule<auto> left, Rule<auto> right) {
    return make_rule([left, right]{
      const auto lresult = left.execute();
      if(lresult == State::Success || lresult == State::Running) return lresult;
      const auto rresult = right.execute();
      if(rresult == State::Success || rresult == State::Running) return rresult;
      return State::Fail;
    });
  }

  // Rule factories
  
  constexpr inline auto eq(const auto& ref, auto value) {
    return make_rule([&ref,value]{
      return (value == ref) ? State::Success : State::Fail;
    });
  }

  constexpr inline auto ne(const auto& ref, auto value) {
    return make_rule([&ref,value]{
      return (value != ref) ? State::Success : State::Fail;
    });
  }

  constexpr inline auto gt(const auto& ref, auto value) {
    return make_rule([&ref,value]{
      return (value < ref) ? State::Success : State::Fail;
    });
  }

  constexpr inline auto lt(const auto& ref, auto value) {
    return make_rule([&ref,value]{
      return (value > ref) ? State::Success : State::Fail;
    });
  }

  // Manipulation of state

  constexpr inline auto set(auto& ref, auto value) {
    return make_rule([&ref,value]{
	ref = value;
	return State::Success;
    });
  }
  
  // Constant rules
  
  static constexpr auto success = make_rule([]{ return State::Success; });
  static constexpr auto fail    = make_rule([]{ return State::Fail;    });
  static constexpr auto running = make_rule([]{ return State::Running; });

  // Convenience, makes less typing
  
  constexpr inline bool operator==(Rule<auto> lhs, State rhs) {
    return lhs.execute() == rhs;
  }

  // Execution
  
  inline State execute(Rule<auto> rule) {
    return rule.execute();
  }
} // namespace moar_rulez
