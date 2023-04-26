CXX		=	c++
NAME 	= 	webserv
SRCS 	=	srcs/Request.cpp srcs/Server.cpp \
			srcs/Socket.cpp srcs/Epoll.cpp srcs/FdManager.cpp \
			srcs/parsing/fill_location.cpp  srcs/parsing/pars_conf.cpp \
			srcs/parsing/pars_multimap.cpp  srcs/parsing/pars_struct.cpp\
	  		srcs/main.cpp 
HEADER	=	inc/Server.hpp inc/Request.hpp \
			inc/Socket.hpp inc/Epoll.hpp \
			inc/FdManager.hpp inc/parsing.hpp 
OBJS	=	$(SRCS:.cpp=.o)
CXXFLAGS=	-Wall -Wextra --std=c++98 -g

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
	./$(NAME) conf/default.conf

run2: all
	valgrind --show-leak-kinds=all --leak-check=full ./$(NAME) conf/default.conf

re: fclean all

.PHONY: all fclean clean run run2 re
