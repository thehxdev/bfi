CC := cc
CFLAGS := -Wall -Wextra -Wno-unused-result
LDFLAGS :=
LIBS :=


SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(SRC_FILES:.c=.o)
BIN := build/bfi

ifeq ($(STATIC), 1)
	CC := musl-clang
	LDFLAGS := -static -s
endif

ifeq ($(OPTIMIZE), 1)
	CFLAGS += -O2 -DNDEBUG
	LDFLAGS += -s
else
	CFLAGS += -Og -ggdb
endif

ifeq ($(SAFE_BFI), 1)
	CFLAGS += -DSAFE_BFI=1
endif

ifeq ($(NON_STD), 1)
	CFLAGS += -DNON_STD_CMDS=1
endif


$(BIN): $(OBJ_FILES)
	@mkdir -p build
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ_FILES)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(SRC_DIR)/*.o build
