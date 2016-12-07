CPP = g++
CFLAGS = -Wall -std=c++11

all: pagerank

pagerank: ./src/PageRank.cpp
	$(CPP) $(CFLAGS) ./src/PageRank.cpp -o ./bin/pagerank

clean:
	rm ./bin/pagerank

test:
	./bin/pagerank ./data/test.data

