CXXFLAGS:=-std=c++17
LDFLAGS:=-lsdl2 -lSDL2_image

tv_studio: main.o
	$(CXX) main.o -o tv_studio $(LDFLAGS)

main.o: ../src/main.cpp
	$(CXX) ../src/main.cpp -c -o main.o $(CXXFLAGS)
