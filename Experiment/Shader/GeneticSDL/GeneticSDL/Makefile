all: genetic

CXXFLAGS=-O3 `sdl-config --cflags`
LDFLAGS=`sdl-config --libs`
OBJECTS=genetic.o serializer.o streamer.o image.o

genetic: $(OBJECTS) point.h
	g++ -o $@ $(CXXFLAGS) $(OBJECTS) $(LDFLAGS)

clean:
	rm genetic $(OBJECTS)
	 
