CXX		=	c++
NAME 	= webserv
SRC 	=	Response.cpp Server.cpp \
			Socket.cpp Epoll.cpp Client.cpp \
			parsing/fill_location.cpp  parsing/pars_conf.cpp \
			parsing/pars_multimap.cpp  parsing/pars_struct.cpp\
	  	main.cpp handle_cgi.cpp Request.cpp dir_listing.cpp \
	  	ErrorPage.cpp utils.cpp
HEADER	=	inc/Server.hpp inc/Response.hpp \
			inc/Socket.hpp inc/Epoll.hpp \
			inc/Client.hpp inc/parsing.hpp 
OBJS	=	$(addprefix obj/, $(SRC:.cpp=.o))
CXXFLAGS=	-Wall -Wextra -Werror --std=c++98 -g



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
	valgrind ./$(NAME) conf/default.conf

re: fclean all

.PHONY: all fclean clean run run2 re
