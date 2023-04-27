CXX		=	c++
NAME 	= 	webserv
SRC 	=	Request.cpp Server.cpp \
			Socket.cpp Epoll.cpp FdManager.cpp \
			parsing/fill_location.cpp  parsing/pars_conf.cpp \
			parsing/pars_multimap.cpp  parsing/pars_struct.cpp\
	  		main.cpp handle_cgi.cpp
HEADER	=	inc/Server.hpp inc/Request.hpp \
			inc/Socket.hpp inc/Epoll.hpp \
			inc/FdManager.hpp inc/parsing.hpp 
OBJS	=	$(addprefix obj/, $(SRC:.cpp=.o))
CXXFLAGS=	-Wall -Wextra --std=c++98 -g



all		:	${NAME}

${NAME}	:	${OBJS} ${HEADER}
			${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS}

obj/%.o: srcs/%.cpp $(HEADER)
	@mkdir -p $(@D)
	${CXX} ${CXXFLAGS} -c $< -o $@

fclean: clean
	rm -f $(NAME)

clean:
	rm -rf obj/

run: all
	./$(NAME) conf/default.conf

run2: all
	valgrind --show-leak-kinds=all --leak-check=full ./$(NAME) conf/default.conf

re: fclean all

.PHONY: all fclean clean run run2 re
