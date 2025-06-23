VERSION = $(shell cat ./VERSION)

CC = gcc

CFLAGS = -xc -Wall -Ibuild -DVERSION=\"$(VERSION)\" -DDATA_HEADER
SRC = $(wildcard src/*.c)
BUILD_DIR = build

DEBUG_CFLAGS = -g3 -O0 -Wextra -Wpedantic -Wshadow -Wconversion -Wformat=2 -Werror

TARGET = pSentinel-v$(VERSION)

DATA_HEADER = $(BUILD_DIR)/data.h
REPORT_DATA_FILE = include/report-skeleton.html

help:	## Show all Makefile targets.
	@echo -e "$(TARGET) \n"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' Makefile | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[33m%-30s\033[0m %s\n", $$1, $$2}'

_pre_build:
	@mkdir -p $(BUILD_DIR)
	@echo "Generating data header from $(REPORT_DATA_FILE).."
	@echo '#ifndef DATA_' > $(DATA_HEADER)
	@echo '#define DATA_' >> $(DATA_HEADER)
	@echo -n '#define REPORT_HTML {' >> $(DATA_HEADER)
	@hexdump -v -e '" 0x" 1/1 "%02x,"' $(REPORT_DATA_FILE) >> $(DATA_HEADER)
	@echo ' 0x00 }' >> $(DATA_HEADER)
	@echo '#endif // DATA_' >> $(DATA_HEADER)
	@echo "Data header generated: $(DATA_HEADER)"

test: _pre_build ## Run test
	# TODO: write unit testcases
	- $(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o test-$(TARGET) main.c $(SRC)
	- @echo -e "\n\nRunning test.."
	- ./test-$(TARGET) ls

build-prod: _pre_build ## Production build
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(SRC)

build-dev:	_pre_build ## Debug build
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o dev-$(TARGET) main.c $(SRC)

pre-commit:	.pre-commit-config.yaml ## Pre commit hooks
	@pre-commit

clean: ## Clean build dirs
	$(RM) -rf $(BUILD_DIR)
	$(RM) -rf ./test-$(TARGET) ./dev-$(TARGET) $(TARGET) ./report.html

.PHONY: help _pre_build build-prod build-dev
