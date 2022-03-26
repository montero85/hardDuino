SRC+=$(SRC_DIR)/test_project/test_project.cpp\
     $(SRC_DIR)/hal/timers/timer.c\
     $(SRC_DIR)/hal/watchdog/watchdog.c

PUBLIC_HEADERS+=$(SRC_DIR)/hal/timers/timer.h\
                $(SRC_DIR)/hal/interrupts/interrupts.h\
                $(SRC_DIR)/hal/watchdog/watchdog.h

