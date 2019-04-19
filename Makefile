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
	GLAD_FLAGS = -ldl -lglfw
endif

FILES = src/$(wildcard *.cpp) src/$(wildcard *.c)
OUTPUT_NAME = game

all: main
	mkdir ../run_
	cp game ../

main: $(FILES)
	$(COMPILER) -I../deps/OpenGL/headers $(FLAGS) $(FILES) -o $(OUTPUT_NAME) $(GL_FLAGS) $(GLAD_FLAGS)

.PHONY: clean run
clean:
	rm game

run: game
	./game
