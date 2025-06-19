# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun<T> wrap(T arg) -> AnyCardMechanic:
  let to_return : AnyCardMechanic
  for field of to_return:
    using Type = type(field)
    if arg is Type:
      to_return = arg
      return to_return

  return to_return

act first_card_mechanic() -> FirstCard:
  act query_some_info1()
  while true:
    act query_some_info2()

act second_card_mechanic() -> SecondCard:
  act ask_for_something()

using AnyCardMechanic = FirstCard | SecondCard

enum GameCard:
  first_card:
    AnyCardMechanic mechanic = wrap(first_card_mechanic())
  second_card:
    AnyCardMechanic mechanic = wrap(second_card_mechanic())


act play(GameCard card) -> Game:
  frm current_state = card.mechanic()
  subaction* current_state

fun main() -> Int:
  let x = play(GameCard::second_card)
  x.ask_for_something()
  if x.is_done():
    return 0
  return 1

