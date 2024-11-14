CC ?= cc
CFLAGS := -std=c99 -Wall -Wextra -Wno-unused-result
LDFLAGS :=
LIBS :=


SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(SRC_FILES:.c=.o)
BIN := bfi


ifeq ($(STATIC), 1)
	CC := musl-clang
	LDFLAGS := -static
endif

ifeq ($(OPTIMIZE), 1)
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS += -s -fno-asynchronous-unwind-tables -fcf-protection=none
else
	CFLAGS += -Og -ggdb
endif

ifeq ($(SAFE), 1)
	CFLAGS += -DSAFE_BFI=1
endif

$(BIN): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ_FILES)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(SRC_DIR)/*.o $(BIN)
