NAME = webserv

# SRC = $(wildcard src/*.cpp)
SRC =	src/main.cpp\
		src/requestHandler.cpp\
		src/serverSocket.cpp\
		src/CGIHandler.cpp\
		src/utils.cpp\
<<<<<<< HEAD
		src/config_parser.cpp
=======
		src/ConfigParser.cpp
>>>>>>> 8ce776a4430b0155f3c9ade98fc05f251d50a7e5

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes

OBJ = $(SRC:.cpp=.o)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME)