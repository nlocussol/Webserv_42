CXX=c++
NAME = webserv
SRCS = Request.cpp Server.cpp Socket.cpp Epoll.cpp\
main.cpp
HEADER = Server.hpp 
OBJS = $(SRCS:.cpp=.o)
CXXFLAGS = -Wall -Wextra --std=c++98 -g

.cpp.o	:
			${CXX} ${CXXFLAGS} -c $< -o ${<:.cpp=.o}

all		:	${NAME}

${NAME}	:	${OBJS} ${HEADER}
			${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS}

fclean: clean
	rm -f $(NAME)

clean:
	rm -f $(OBJS)

run: all
	./$(NAME)

run2: all
	valgrind --show-leak-kinds=all --leak-check=full ./$(NAME)

re: fclean all

.PHONY: all fclean clean run run2 re
