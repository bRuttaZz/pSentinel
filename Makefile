CC = gcc
CFLAGS = -Wall -Iinclude
SRC = $(wildcard src/*.c)
TARGET = pSentinel

BUILD_DIR = build


help:	## Show all Makefile targets.
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' Makefile | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[33m%-30s\033[0m %s\n", $$1, $$2}'

_pre_build:
	@mkdir -p $(BUILD_DIR)

format:	## Format source code
	@pre-commit

build-prod: _pre_build ## Production build
	@echo "Todo"

build-dev:	_pre_build ## Debug build
	@echo "Todo"

clean: ## Clean build dirs
	@rm -rf $(BUILD_DIR)

.PHONY: help _pre_build build-prod build-dev
