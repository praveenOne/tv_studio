LDFLAGS=`sdl2-config --libs` -lSDL2_image -lSDL2_ttf
CXXFLAGS=-fPIE -std=c++17

tv_studio: main.o
	$(CXX) main.o -o tv_studio $(LDFLAGS)

clean:
	rm *.o tv_studio

run: tv_studio
	./tv_studio
