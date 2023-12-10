#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_TOUCHSCREEN_TIMER_PERIOD 10.0E-3
#define CONFIG_GAME_TIMER_PERIOD 45.0E-3

#define CONFIG_MAX_LASERS 7

#define CONFIG_COLOR_LASER DISPLAY_RED
#define CONFIG_COLOR_PLAYER DISPLAY_GREEN
#define CONFIG_BACKGROUND_COLOR DISPLAY_BLACK

#define CONFIG_COLOR_ANSWERKEY DISPLAY_CYAN
#define CONFIG_RADIUS_ANSWERKEY 10

#define CONFIG_WINNING_SCORE 10

// Speed of lasers
#define CONFIG_LASER_DISTANCE_PER_SECOND 35
#define CONFIG_LASER_DISTANCE_PER_TICK                                 \
  (CONFIG_LASER_DISTANCE_PER_SECOND * CONFIG_GAME_TIMER_PERIOD)

// Speed of player
#define CONFIG_PLAYER_MOVEMENT_PER_SECOND 50
#define CONFIG_PLAYER_MOVEMENT_PER_TICK                                \
  (CONFIG_PLAYER_MOVEMENT_PER_SECOND * CONFIG_GAME_TIMER_PERIOD)

#endif /* CONFIG_H */