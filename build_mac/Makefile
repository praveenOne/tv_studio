CXXFLAGS:=-std=c++17
LDFLAGS:=-lsdl2 -lSDL2_image

../tv_studio: main.o
	$(CXX) main.o -o ../tv_studio $(LDFLAGS)

main.o: ../src/main.cpp ../src/scenes/forest.h ../src/scenes/ice.h
	$(CXX) ../src/main.cpp -c -o main.o $(CXXFLAGS)

run: ../tv_studio
	cd .. && ./tv_studio

clean:
	rm ../tv_studio *.o
