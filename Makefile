NAME = webserv

# SRC = $(wildcard src/*.cpp)
SRC =	src/main2.cpp\
		src/requestHandler.cpp\
		src/serverSocket.cpp

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