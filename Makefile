FLAGS=-lncurses -lpthread -std=c++11 -g

all: model client

model: model_mainloop.cpp oo_model.cpp oo_model.hpp oo_server.cpp oo_server.hpp
	g++ -oserver model_mainloop.cpp oo_model.cpp oo_server.cpp $(FLAGS)

client: client.cpp oo_model.cpp oo_model.hpp oo_server.cpp oo_server.hpp
	g++ client.cpp oo_model.cpp -oclient oo_server.cpp $(FLAGS)

