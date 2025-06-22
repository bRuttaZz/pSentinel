VERSION = $(shell cat ./VERSION)

CC = gcc

CFLAGS = -Wall -Iinclude
SRC = $(wildcard src/*.c)
BUILD_DIR = build

DEBUG_CFLAGS = -g3 -O0 -Wextra -Wpedantic -Wshadow -Wconversion -Wformat=2 -Werror

TARGET = pSentinel-v$(VERSION)


help:	## Show all Makefile targets.
	@echo -e "$(TARGET) \n"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' Makefile | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[33m%-30s\033[0m %s\n", $$1, $$2}'

_pre_build:
	@mkdir -p $(BUILD_DIR)

test: _pre_build ## Run test
	- $(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o test-$(TARGET) test.c $(SRC)
	- @echo -e "\n\nRunning test.."
	- ./test-$(TARGET)

build-prod: _pre_build ## Production build
	$(CC) $(CFLAGS) -o test-$(TARGET) main.c $(SRC)

build-dev:	_pre_build ## Debug build
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o dev-$(TARGET) main.c $(SRC)

pre-commit:	.pre-commit-config.yaml ## Pre commit hooks
	@pre-commit

clean: ## Clean build dirs
	@rm -rf $(BUILD_DIR)

.PHONY: help _pre_build build-prod build-dev
