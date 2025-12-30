CC ?= cc
CFLAGS += -std=gnu99 -Wall -Wextra -Wshadow -Wno-unused-result -Wpadded

LD := ld.lld
LDFLAGS :=
LIBS :=


SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(SRC_FILES:.c=.o)
BIN := bfi


ifeq ($(STATIC), 1)
	CC := musl-clang
	LDFLAGS += -static
endif

ifeq ($(OPTIMIZE), 1)
	CFLAGS += -O3 -DNDEBUG -march=native
	LDFLAGS += -fuse-ld=lld -flto -fno-asynchronous-unwind-tables -fcf-protection=none -Wl,-O3,-q
else
	CFLAGS += -Og -g3
endif

ifeq ($(SAFE), 1)
	CFLAGS += -DSAFE_BFI=1
endif

.PHONY: all build clean

all: build

build: $(BIN)

$(BIN): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ_FILES) $(LIBS)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(SRC_DIR)/*.o $(BIN)
