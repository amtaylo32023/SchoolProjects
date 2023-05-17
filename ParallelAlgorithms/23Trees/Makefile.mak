FLAGS = -g
OBJS = Node.o Tree.o driver.o 
OUT = tree.exe
SRC_PATH = ./build/
SOURCES = $(OBJS:%.o=$(SRC_PATH)%.o)

all: build_directory $(OUT)

build_directory:
	mkdir -p $(SRC_PATH)

$(OUT): $(OBJS)
	g++ $(SOURCES) -o $@ 

%.o: %.cpp
	g++ -c $(FLAGS) $^ -o $(SRC_PATH)$@

clean:
	rm -f $(SRC_PATH)*.o *.exe *.txt