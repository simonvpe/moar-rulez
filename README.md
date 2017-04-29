# MOAR RULEZ
Moar-rulez aims to be a very simple, very fast, yet very useful single header rules engine library. It uses operator everloads to allow chaining of rules with regular boolean operators and other helper functions.

The implementation is basically modeled as a behavior tree implemented with constexpr functions so that the boilerplate and glue code can be moved out of runtime and into compile time. Speed, clean code and simplicity has been the focus for every line of code.

## Getting Started ##

There are no dependencies but project has only been tested with gcc 7.0.1.

## Installing ##

Simply drop the `include/moar_rulez.hpp` file in your project and you're good to go.

## Example ##
This example actually gets reduced down to a single assembler instruction if you compile it with gcc 7.0.1 and `-O3` optimization. That is indeed blazingly fast!
```c++
#include <moar_rulez.hpp>
using moar_rulez::make_rule;
using moar_rulez::State;
using moar_rulez::eq;
using moar_rulez::set;
using moar_rulez::execute;

bool dude_can_open_the_door() {
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
    
  while( door_locked && !failed ) execute(rules);
  return failed;
}
```
## Running the tests ##
Make sure you have a C++17 compiler (preferably gcc 7.0.1) and cmake >= 3.8 installed, then
```
$ git clone https://github.com/simonvpe/moar-rulez.git
$ cd moar-rulez && mkdir build && cd build && cmake .. && make && ./test/tests
```
# Built With #
* gcc 7.0.1
* cmake 3.8

# Authors #
* Simon Pettersson

# License #
This project is licensed under the MIT License - see the LICENSE.md file for details
