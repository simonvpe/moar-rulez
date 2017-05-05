// vim: tabstop=3 expandtab shiftwidth=4 softtabstop=4

#include <catch.hpp>
#include <moar_rulez.hpp>

using moar_rulez::execute;
using moar_rulez::success;
using moar_rulez::fail;
using moar_rulez::running;
using moar_rulez::eq;
using moar_rulez::ne;
using moar_rulez::gt;
using moar_rulez::lt;
using moar_rulez::State;
using moar_rulez::make_rule;
using moar_rulez::set;

SCENARIO("All rules should work independently") {
  
  GIVEN("constants") {
    CHECK( success == execute(success) );
    CHECK( fail	   == execute(fail) );
    CHECK( running == execute(running) );
  }
  GIVEN("operator&&") {
    CHECK( success == execute(success && success) );
    CHECK( fail    == execute(success && fail) );
    CHECK( fail    == execute(fail && success) );
    CHECK( fail    == execute(fail && fail) );
    CHECK( fail    == execute(fail && running) );
    CHECK( running == execute(running && fail) );
    CHECK( running == execute(success && running) );
    CHECK( running == execute(running && success) );
    CHECK( running == execute(running && running) );
  }
  GIVEN("operator||") {
    CHECK( success == execute(success || success) );
    CHECK( success == execute(success || fail) );
    CHECK( success == execute(fail || success) );
    CHECK( success == execute(success || running) );
    CHECK( running == execute(running || success) );
    CHECK( fail    == execute(fail || fail) );
    CHECK( running == execute(fail || running) );
    CHECK( running == execute(running || fail) );
    CHECK( running == execute(running || running) );
  }

  GIVEN("operator~") {
    CHECK( success == execute(~fail) );
    CHECK( fail    == execute(~success) );
    CHECK( running == execute(~running) );
  }
  
  GIVEN("eq") {
    const auto eqvar = 5;
    CHECK( success == execute(eq(eqvar,5)) );
    CHECK( fail    == execute(eq(eqvar,7)) ); 
    CHECK( success == execute(eq<5>(eqvar)) );
    CHECK( fail    == execute(eq<7>(eqvar)) );
  }
  GIVEN("ne") {
    const auto nevar = 4;
    CHECK( success == execute(ne(nevar,2)) );
    CHECK( fail    == execute(ne(nevar,4)) );
    CHECK( success == execute(ne<2>(nevar)) );
    CHECK( fail    == execute(ne<4>(nevar)) );
  }
  GIVEN("gt") {
    const auto gtvar = 4;
    CHECK( success == execute(gt(gtvar,2)) );
    CHECK( fail    == execute(gt(gtvar,4)) );
    CHECK( fail    == execute(gt(gtvar,5)) );
    CHECK( success == execute(gt<2>(gtvar)) );
    CHECK( fail    == execute(gt<4>(gtvar)) );
    CHECK( fail    == execute(gt<5>(gtvar)) );
  }
  GIVEN("lt") {
    const auto ltvar = 4;
    CHECK( success == execute(lt(ltvar,5)) );
    CHECK( fail    == execute(lt(ltvar,4)) );
    CHECK( fail    == execute(lt(ltvar,2)) );
    CHECK( success == execute(lt<5>(ltvar)) );
    CHECK( fail    == execute(lt<4>(ltvar)) );
    CHECK( fail    == execute(lt<2>(ltvar)) );
  }
  GIVEN("set") {
    auto setvar = 0;
    CHECK( success == execute(set(setvar,42)) );
    CHECK( 42 == setvar );
    CHECK( success == execute(set<43>(setvar)) );
    CHECK( 43 == setvar );
  }
}

SCENARIO("Complex sequences") {
  CHECK( success == execute(success && fail    || success) );
  CHECK( fail    == execute(success && fail    || fail)    );
  CHECK( running == execute(success && running || success) );
}

SCENARIO("Example: A dude tries to open a door") {
  auto door_open     = false;
  auto door_locked   = true;
  auto failed        = false;
  auto dude_has_key  = true;
  auto dude_position = 0;
  constexpr auto door_position = 100;

  const auto move_dude = make_rule([&]{
    dude_position += 1;
    return State::Success;
  });

  const auto unlock_door = make_rule([&]{
      if(dude_position != door_position || !dude_has_key) return State::Fail;
      door_locked = false;
      return State::Success;
  });

  const auto open_door = make_rule([&]{
      if(dude_position != door_position || door_locked) return State::Fail;
      door_open = true;
      return State::Success;
  });

  const auto try_open =
    eq(dude_position, door_position)
    && (unlock_door || set(failed,true))
    && open_door;
  
  const auto rules =
    try_open
    || move_dude;

  const auto run = [&]{
    while( door_locked && !failed ) execute(rules);
  };
  
  GIVEN("the dude has no key") {
    dude_has_key = false;
    run();
    CHECK( false == door_open );
    CHECK( true  == failed );
  }
  GIVEN("the dude has a key") {
    dude_has_key = true;
    run();
    CHECK( true  == door_open );
    CHECK( false == failed );
  }
}
