BINARY = adidasi
LIBS = -lpistache -lpthread
CXX = g++
CXX_VERSION = c++17


build:
	$(CXX) -std=$(CXX_VERSION) adidasi.cpp -o $(BINARY) $(LIBS)

run: build
	chmod +x $(BINARY)
	./$(BINARY)

clean:
	rm $(BINARY)
