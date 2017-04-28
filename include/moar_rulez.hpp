// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
#pragma once

#include <functional>
#include <type_traits>
#include <tuple>

namespace moar_rulez {
  enum class State { Fail, Success, Running };

  // Logic operators
  
  constexpr inline auto operator~(auto rule) {
    return [rule]{
      const auto result = rule();
      return result == State::Fail ? State::Success :
	result == State::Success ? State::Fail :
	State::Running;
    };
  }

  constexpr inline auto operator&&(auto left, auto right) {
    return [left, right]{
      const auto lresult = left();
      const auto rresult = right();
      return (lresult == State::Fail || rresult == State::Fail) ? State::Fail :
	(lresult == State::Success && rresult == State::Success) ? State::Success :
	State::Running;
    };
  }

  constexpr inline auto operator||(auto left, auto right) {
    return [left, right]{
      const auto lresult = left();
      const auto rresult = right();
      return (lresult == State::Success || rresult == State::Success) ? State::Success :
	(lresult == State::Fail && rresult == State::Fail) ? State::Fail :
	State::Running;
    };
  }

  // Rule factories
  
  constexpr inline auto eq(const auto& ref, auto value) {
    return [&ref,value]{
      return (value == ref) ? State::Success : State::Fail;
    };
  }

  constexpr inline auto ne(const auto& ref, auto value) {
    return [&ref,value]{
      return (value != ref) ? State::Success : State::Fail;
    };
  }

  constexpr inline auto gt(const auto& ref, auto value) {
    return [&ref,value]{
      return (value < ref) ? State::Success : State::Fail;
    };
  }

  constexpr inline auto lt(const auto& ref, auto value) {
    return [&ref,value]{
      return (value > ref) ? State::Success : State::Fail;
    };
  }

  constexpr inline auto sequence() {
    return []{ return State::Success; };
  }

  template <typename First, typename ... Rules>
  constexpr inline auto sequence(First first, Rules ... rules) {
    return [first, rules...](){
      const auto rfirst = first();
      if(rfirst == State::Fail || rfirst == State::Running) {
	return rfirst;
      }
      return sequence(rules...)();
    };
  }

  // Constant rules
  
  static constexpr auto success = []{ return State::Success; };
  static constexpr auto fail    = []{ return State::Fail;    };
  static constexpr auto running = []{ return State::Running; };

  // Convenience, makes less typing
  
  constexpr inline bool operator==(decltype(success) lhs, State rhs) {
    return lhs() == rhs;
  }

  constexpr inline bool operator==(decltype(fail) lhs, State rhs) {
    return lhs() == rhs;
  }

  constexpr inline bool operator==(decltype(running) lhs, State rhs) {
    return lhs() == rhs;
  }

  // Execution
  
  inline State execute(auto rule) {
    return rule();
  }
} // namespace moar_rulez
