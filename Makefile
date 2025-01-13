CC ?= cc
CFLAGS += -std=c99 -Wall -Wextra -Wshadow -Wno-unused-result

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
	CFLAGS += -O3 -flto -DNDEBUG -march=native
	LDFLAGS += -fuse-ld=lld -fno-asynchronous-unwind-tables -fcf-protection=none -Wl,-O3,-q
else
	CFLAGS += -Og -ggdb
endif

ifeq ($(BRANCHLESS), 1)
	CFLAGS += -DBFI_BRANCHLESS=1
endif

ifeq ($(SAFE), 1)
	CFLAGS += -DSAFE_BFI=1
endif

$(BIN): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ_FILES) $(LIBS)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(SRC_DIR)/*.o $(BIN)
