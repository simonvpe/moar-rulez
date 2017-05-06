// Copyright (c) 2017 Simon Pettersson

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#ifndef NDEBUG
#  include <iostream>
#endif

namespace moar_rulez {
  enum class State { Fail, Success, Running };

  // Rule factory
  
  template<typename TExecute> struct Rule { TExecute execute; };


#ifndef NDEBUG
  static inline int indent = 0;
  
  constexpr inline auto make_rule_impl(auto f) {
    return Rule<decltype(f)>{f};
  }

  constexpr inline auto debug_wrapper(Rule<auto> rule, const char* name) {
    return make_rule_impl([rule,name]{
	for(auto i = 0 ; i < indent ; ++i) std::cerr << " ";
	std::cerr << ">> Running rule " << '<' << name << "> \n";
	++indent;
	const auto result = rule.execute();
	--indent;
	for(auto i = 0 ; i < indent ; ++i) std::cerr << " ";
	std::cerr << "<< " << (result == State::Success ? "Succes" :
		      result == State::Fail ? "Fail" :
		      "Running")
		  << ' ' << name << " \n";
	
	return result;
      });
  }

  constexpr inline auto make_rule(auto f, const char* name = "unknown") {
    const auto rule = make_rule_impl(f);
    return debug_wrapper(rule, name);
  }
#else
  constexpr inline auto make_rule(auto f, const char* name = nullptr) {
    return Rule<decltype(f)>{f};
  }  
#endif

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
  template<auto TValue>
  constexpr inline auto eq(const auto& ref) {
    return make_rule([&ref]{
      return (TValue == ref) ? State::Success : State::Fail;
    });
  }
	
  constexpr inline auto eq(const auto& ref, const auto& value) {
    return make_rule([&ref,&value]{
      return (value == ref) ? State::Success : State::Fail;
    });
  }

  template<auto TValue>
  constexpr inline auto ne(const auto& ref) {
    return make_rule([&ref]{
      return (TValue != ref) ? State::Success : State::Fail;
    });
  }
	
  constexpr inline auto ne(const auto& ref, const auto& value) {
    return make_rule([&ref,&value]{
      return (value != ref) ? State::Success : State::Fail;
    });
  }

  template<auto TValue>
  constexpr inline auto gt(const auto& ref) {
    return make_rule([&ref]{
      return (TValue < ref) ? State::Success : State::Fail;
    });
  }
	
  constexpr inline auto gt(const auto& ref, const auto& value) {
    return make_rule([&ref,&value]{
      return (value < ref) ? State::Success : State::Fail;
    });
  }

  template<auto TValue>
  constexpr inline auto lt(const auto& ref) {
    return make_rule([&ref]{
      return (TValue > ref) ? State::Success : State::Fail;
    });
  }
  
  constexpr inline auto lt(const auto& ref, const auto& value) {
    return make_rule([&ref,&value]{
      return (value > ref) ? State::Success : State::Fail;
    });
  }

  // Manipulation of state
  template<auto TValue>
  constexpr inline auto set(auto& ref) {
    return make_rule([&ref]{
	ref = TValue;
	return State::Success;
    });
  }
	
  constexpr inline auto set(auto& ref, const auto& value) {
    return make_rule([&ref,&value]{
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
