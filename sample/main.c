#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

int _milliseconds()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

typedef enum {
  kNoDirection,
  kLeft,
  kRight
} Direction;

typedef enum {
  kNoResult,
  kGoodFast,
  kGoodSlow,
  kBad
} Result;

typedef struct _game_state {
  int start_time_;
  int count_;
  int misses_;
  int score_;
  int multiplier_;
  Direction direction_;
  int consecutives_;
} GameState;

void state_init_multiplier(GameState *state)
{
  state->multiplier_ = 1;
}

void state_init(GameState *state)
{
  state->start_time_ = _milliseconds();
  state->count_ = 0;
  state->misses_ = 0;
  state->score_ = 0;
  state_init_multiplier(state);
  state->direction_ = kNoDirection;
  state->consecutives_ = 0;
}

int state_duration(const GameState *state)
{
  return _milliseconds() - state->start_time_;
}

static const int MULTIPLIER_MIN = 1;
static const int MULTIPLIER_MAX = 5;
static const int MULTIPLIER_MILLISECONDS = 1000;
static const int MULTIPLIER_DECAY_MILLISECONDS = 3000;
static const int GAME_DURATION_MILLISECONDS = 15000;

void state_increment_multiplier(GameState *state)
{
  if (state->multiplier_ < MULTIPLIER_MAX) {
    state->multiplier_ += 1;
  } else {
    // We have to handle the 'decay' multiplier here
  }
}

void state_decrement_multiplier(GameState *state)
{
  if (state->multiplier_ == 5 || state->multiplier_ == 4) {
    state->multiplier_ = 3;
  } else if (state->multiplier_ == 3 || state->multiplier_ == 2) {
    state->multiplier_ = 1;
  }
}

Direction _random_direction() 
{
  if ((rand() % 2) == 0) {
    return kLeft;
  } else {
    return kRight;
  }
}

typedef struct _turn {
  int milliseconds_;
  int speed_;
  Direction direction_;
  Result result_;
} Turn;

void turn_init(Turn *turn) 
{
  turn->milliseconds_ = _milliseconds();
  turn->speed_ = 0;
  turn->direction_ = kNoDirection;
  turn->result_ = kNoResult;
}

void turn_copy(const Turn *from, Turn *to)
{
  *to = *from;
}

void _setup_random() 
{
  srand(time(0));
}

int state_counts[5] = { 0, 3, 6, 9, 12 };

void _handle_turn(
    GameState *state, Turn *this_turn, const Turn *last_turn) 
{
  this_turn->speed_ = _milliseconds() - this_turn->milliseconds_;
  if (state->direction_ == this_turn->direction_) {
    this_turn->result_ = kGoodFast;
    state->score_ += state->multiplier_;
    if (this_turn->speed_ < MULTIPLIER_MILLISECONDS) {
      state->consecutives_ += 1;
      if (state_counts[state->multiplier_] == state->consecutives_) {
        state_increment_multiplier(state);
        state->consecutives_ = 0;
      }
    } else if (this_turn->speed_ >= MULTIPLIER_DECAY_MILLISECONDS) {
      this_turn->result_ = kGoodSlow;
      state_decrement_multiplier(state);
      state->consecutives_ = 0;
    }
  } else {
    this_turn->result_ = kBad;
    state_decrement_multiplier(state);
    state->consecutives_ = 0;
    state->misses_ += 1;
  }
}

void _game_header(const GameState *state, const Turn *last_turn)
{
  printw("'q' to quit. 'r' to reset. left arrow / right arrow to play\n");
  printw("-----------------------------------------------------------\n");
  printw("\n");
  printw("time   : %d\n", (GAME_DURATION_MILLISECONDS - state_duration(state)) / 1000);
  printw("score  : %d\n", state->score_);
  printw("multi  : %d\n", state->multiplier_);
  printw("misses : %d\n", state->misses_);
  printw("speed  : %.3f\n", ((float)(last_turn->speed_)) / 1000);
  printw("\n");

  if (state->direction_ == kLeft) {
    printw("   <===  ", state->count_);
  } else {
    printw("   ===>  ", state->count_);
  }
  if (last_turn->result_ != kNoResult) {
    const char *str = "+";
    if (last_turn->result_ == kGoodFast) {
      str = "+";
    } else if (last_turn->result_ == kGoodSlow) {
      str = "-";
    } else if (last_turn->result_ == kBad) {
      str = "XXX";
    }
    printw("   %s", str);
  }
}

int main()
{
  _setup_random();

  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  int keep_going = 1;
  while (keep_going == 1) {
    GameState state;
    state_init(&state);
    Turn this_turn;
    Turn last_turn;
    turn_init(&this_turn);
    turn_init(&last_turn);
    for (;;) {
      if (state_duration(&state) >= GAME_DURATION_MILLISECONDS) {
        keep_going = 0;
        break;
      }
      state.direction_ = _random_direction();
      turn_copy(&this_turn, &last_turn);
      turn_init(&this_turn);

      _game_header(&state, &last_turn);
      int v = getch();
      clear();
      state.count_ += 1;
      refresh();
      if (v == KEY_LEFT) {
        this_turn.direction_ = kLeft;
      } else if (v == KEY_RIGHT) {
        this_turn.direction_ = kRight;
      } else if (v == 'q' || v == 'Q') {
        printw("thanks for playing\n");
        keep_going = 1;
        break;
      } else if (v == 'r' || v == 'R') {
        keep_going = 0;
        break;
      }
      _handle_turn(&state, &this_turn, &last_turn);
    }
  }
  endwin();
  return 0;
}
