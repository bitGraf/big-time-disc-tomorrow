UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	COMPILER = g++
	FLAGS = -std=c++1y -pedantic -Wall
	GL_FLAGS = -lGL -lglfw
endif

ifeq ($(UNAME_S), Darwin)
	COMPILER = clang++
	FLAGS = -std=c++14 -stdlib=libc++ -pedantic -Wall
	GL_FLAGS = -lGLEW -framework OpenGL -lm
	GLAD_FLAGS = -ldl
endif

FILES = $(wildcard *.cpp)
OUTPUT_NAME = dindin

all: main

main: $(FILES)
	$(COMPILER) -I../deps/OpenGL/headers $(FLAGS) $(FILES) glad.c -o $(OUTPUT_NAME) $(GL_FLAGS) $(GLAD_FLAGS)

.PHONY: clean run
clean:
	rm dindin

run: dindin
	./dindin