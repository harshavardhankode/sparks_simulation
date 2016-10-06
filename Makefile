CC=g++

OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib 
CPPFLAGS=-I/usr/local/include -I./

BIN=scene
SRCS=scene.cpp gl_framework.cpp shader_util.cpp object.cpp texture.cpp spark.cpp
INCLUDES=gl_framework.hpp shader_util.hpp scene.hpp object.hpp texture.hpp spark.hpp

all: $(BIN)

$(BIN): $(SRCS) $(INCLUDES)
	g++  $(CPPFLAGS) $(SRCS) -o  $(BIN) $(LDFLAGS) $(LIBS) -g -pg

clean:
	rm -f *~ *.o $(BIN)
