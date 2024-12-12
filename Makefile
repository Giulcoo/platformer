INPUT ?= input
OUTPUT ?= output
VERBOSE ?= 1

all: setup

setup: 
	@make --no-print-directory -C compiler

compile: dir run_compiler setup_game

dir:
	@mkdir -p build

run_compiler:
	@(echo "===Compile input===" && make --no-print-directory -C compiler run INPUT=../$(INPUT) VERBOSE=$(VERBOSE)) 

setup_game:
	@(echo "===Compile game===" && make --no-print-directory -C game OUTPUT=../build/$(OUTPUT))

run:
	@build/$(OUTPUT)