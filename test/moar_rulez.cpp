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
using moar_rulez::sequence;

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
    CHECK( fail    == execute(running && fail) );
    CHECK( running == execute(success && running) );
    CHECK( running == execute(running && success) );
    CHECK( running == execute(running && running) );
  }
  GIVEN("operator||") {
    CHECK( success == execute(success || success) );
    CHECK( success == execute(success || fail) );
    CHECK( success == execute(fail || success) );
    CHECK( success == execute(success || running) );
    CHECK( success == execute(running || success) );
    CHECK( fail    == execute(fail || fail) );
    CHECK( running == execute(fail || running) );
    CHECK( running == execute(running || fail) );
    CHECK( running == execute(running || running) );
  }

  GIVEN("operator!") {
    CHECK( success == execute(~fail) );
    CHECK( fail    == execute(~success) );
    CHECK( running == execute(~running) );
  }
  
  GIVEN("eq") {
    auto eqvar = 5;
    CHECK( success == execute(eq(eqvar,5)) );
    CHECK( fail    == execute(eq(eqvar,7)) );    
  }
  GIVEN("ne") {
    auto nevar = 4;
    CHECK( fail    == execute(ne(nevar,4)) );
    CHECK( success == execute(ne(nevar,2)) );
    CHECK( success == execute(~eq(nevar,2)) ); // same as above
  }
  GIVEN("gt") {
    auto gtvar = 4;
    CHECK( fail    == execute(gt(gtvar,4)) );
    CHECK( fail    == execute(gt(gtvar,5)) );
    CHECK( success == execute(gt(gtvar,2)) );
  }
  GIVEN("lt") {
    auto ltvar = 4;
    CHECK( fail    == execute(lt(ltvar,4)) );
    CHECK( success == execute(lt(ltvar,5)) );
    CHECK( fail    == execute(lt(ltvar,2)) );
  }  
  GIVEN("sequence") {
    CHECK( success == execute(sequence(success, success, success)) );
    CHECK( fail    == execute(sequence(success, success, fail)) );
    CHECK( running == execute(sequence(success, success, running)) );
    CHECK( fail    == execute(sequence(success, fail, success)) );
    CHECK( fail    == execute(sequence(success, fail, fail)) );
    CHECK( fail    == execute(sequence(success, fail, running)) );
    CHECK( running == execute(sequence(success, running, success)) );
    CHECK( running == execute(sequence(success, running, fail)) );
    CHECK( running == execute(sequence(success, running, running)) );
    CHECK( fail    == execute(sequence(fail, success, success)) );
    CHECK( fail    == execute(sequence(fail, success, fail)) );
    CHECK( fail    == execute(sequence(fail, success, running)) );
    CHECK( fail    == execute(sequence(fail, fail, success)) );
    CHECK( fail    == execute(sequence(fail, fail, fail)) );
    CHECK( fail    == execute(sequence(fail, fail, running)) );
    CHECK( fail    == execute(sequence(fail, running, success)) );
    CHECK( fail    == execute(sequence(fail, running, fail)) );
    CHECK( fail    == execute(sequence(fail, running, running)) );
    CHECK( running == execute(sequence(running, success, success)) );
    CHECK( running == execute(sequence(running, success, fail)) );
    CHECK( running == execute(sequence(running, success, running)) );
    CHECK( running == execute(sequence(running, fail, success)) );
    CHECK( running == execute(sequence(running, fail, fail)) );
    CHECK( running == execute(sequence(running, fail, running)) );
    CHECK( running == execute(sequence(running, running, success)) );
    CHECK( running == execute(sequence(running, running, fail)) );
    CHECK( running == execute(sequence(running, running, running)) );
  }
}

SCENARIO("Example: A dude tries to open a door") {
  // positions [ dude, door ]
  auto door_open     = false;
  auto door_locked   = true;
  auto dude_position = 0;
  constexpr auto door_position = 1;

  const auto move_dude = [&dude_position]{
    dude_position += 1;
    return State::Success;
  };

  //success && move_dude && move_dude;
}
/*
#include <iostream>
enum class Door { Closed, Open };
enum class Position { BeforeDoor, AtDoor, BehindDoor };
Door door;
Position position;
bool walking;
bool path_blocked;
bool door_blocked;

SCENARIO("A simple example: A character that wants to go through a door.") {

  const auto walk_up_to_door = [&]{
      if(position == Position::AtDoor) {
	return State::Success;
      }
      if((position == Position::BeforeDoor) && !path_blocked) {
	if(!walking) {
	  walking = true;
	  return State::Running;
	} else {
	  walking = false;
	  position = Position::AtDoor;
	  return State::Success;
	}
      }
      return State::Fail;
  };

  const auto open_door = [&]{
      if(door == Door::Open || !door_blocked) {
	door = Door::Open;
	return State::Success;
      }
      return State::Fail;
  };

  const auto close_door = [&]{
      if(door == Door::Closed || !door_blocked) {
	door = Door::Closed;
	return State::Success;
      }
      return State::Fail;
  };

  const auto walk_away = [&]{
      position = Position::BehindDoor;
      return State::Success;
  };

  GIVEN("door is not blocked") {
    door         = Door::Closed;
    position     = Position::BeforeDoor;
    walking      = false;
    path_blocked = false;
    door_blocked = false;
  
    const auto r = sequence(
        walk_up_to_door,
	open_door,
	close_door,
	walk_away
    );
  
    CHECK( State::Running == execute(r) );
    CHECK( State::Success == execute(r) );
  
    CHECK( position == Position::BehindDoor );
    CHECK( door == Door::Closed );
    CHECK( walking == false );
  }

  GIVEN("door is blocked") {
    door         = Door::Closed;
    position     = Position::BeforeDoor;
    walking      = false;
    path_blocked = false;
    door_blocked = true;
  
    const auto r = sequence(
        walk_up_to_door,
	open_door,
	close_door,
	walk_away
    );
  
    CHECK( State::Running == execute(r) );
    CHECK( State::Fail == execute(r) );    
  
    CHECK( position == Position::AtDoor );
    CHECK( door == Door::Closed );
    CHECK( walking == false );
  }
  
}
*/
