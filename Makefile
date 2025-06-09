CC = gcc
CFLAGS = -Wall -Wextra -g -I./src -I./include -I/opt/homebrew/opt/openssl@3/include
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(filter-out $(SRC_DIR)/test_security.c, $(SRCS))
TEST_OBJS = $(TEST_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_TARGET = $(BIN_DIR)/test_security

MAIN_SRCS = $(filter-out $(SRC_DIR)/test_security.c, $(SRCS))
MAIN_OBJS = $(MAIN_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN_TARGET = $(BIN_DIR)/medblockchain

.PHONY: all clean test

all: $(MAIN_TARGET)

test: $(TEST_TARGET)

$(MAIN_TARGET): $(MAIN_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(MAIN_OBJS) -o $@ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS) $(OBJ_DIR)/test_security.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(TEST_OBJS) $(OBJ_DIR)/test_security.o -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) 