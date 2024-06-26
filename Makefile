VPATH		:=	src:
NAME		:=	webserv
CXXFLAGS	?=	-Wall -Wextra -Werror -std=c++20
LDFLAGS		?=
OBJECTS		:=	obj/main.o \
				obj/Signal.o \
				obj/cgi/CGIPipeIn.o \
				obj/cgi/CGIPipeOut.o \
				obj/Client.o \
				obj/Helper.o \
				obj/Message.o \
				obj/Request.o \
				obj/Response.o \
				obj/Multiplexer.o \
				obj/ASocket.o \
				obj/Timer.o \
				obj/Server.o \
				obj/ConfigInit.o \
				obj/config/Token.o \
				obj/config/Lexer.o \
				obj/config/Parser.o \
				obj/config/ParserServer.o \
				obj/config/ParserLocation.o \
				obj/config/Configuration.o \
				obj/config/ConfigServer.o \
				obj/config/ConfigLocation.o \
				obj/config/ConfigErrorPage.o \
				obj/config/ParserHelper.o \
				obj/request/DirectoryListing.o \
				obj/request/RequestDelete.o \
				obj/request/RequestChunk.o \
				obj/request/RequestGet.o \
				obj/request/RequestPost.o
HEADERS		:=	include/CGIPipeIn.hpp \
				include/Signal.hpp \
				include/CGIPipeOut.hpp \
				include/Client.hpp \
				include/Helper.hpp \
				include/Message.hpp \
				include/Request.hpp \
				include/Response.hpp \
				include/Multiplexer.hpp \
				include/ASocket.hpp \
				include/StatusCodeException.hpp \
				include/Server.hpp \
				include/Timer.hpp \
				include/Token.hpp \
				include/Lexer.hpp \
				include/Parser.hpp \
				include/Configuration.hpp \
				include/ConfigServer.hpp \
				include/ConfigLocation.hpp \
				include/ConfigErrorPage.hpp \
				include/ParserHelper.hpp \
				include/Permissions.hpp
SPDLOGLIB	:=	./spdlog/build/libspdlog.a
SPDLOGINCL	:=	-DSPDLOG_COMPILED_LIB -I./spdlog/include
CONTAINER	:=	webserv-container
IMAGE		:=	ubuntu-c-plus
INCLUDE		:= -I./include

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(SPDLOGLIB)

obj/%.o : %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $(SPDLOGINCL) -o $@ $<

clean :
	$(RM) -r obj

fclean : clean
	$(RM) $(NAME)
	$(RM) -r ./results-parster

re : fclean all

docker-pwd:
	docker run \
	-p 8081:8081 \
	-p 8082:8082 \
	-p 8083:8083 \
	--name $(CONTAINER) \
	-it \
	--rm \
	--init \
	-v "$$PWD:/pwd" \
	--cap-add=SYS_PTRACE \
	--security-opt seccomp=unconfined \
	-e CXX="clang++" \
	-e CXXFLAGS="-Wall -Wextra -Werror -std=c++20 -g -gdwarf-4 -gstrict-dwarf" \
	-e LDFLAGS="-g -gdwarf-4 -gstrict-dwarf" \
	$(IMAGE) sh -c "cd /pwd; bash"

docker-clean:
	docker run \
	-p 8081:8081 \
	--name $(CONTAINER) \
	-it \
	--rm \
	--init \
	-v "$$PWD:/pwd" \
	--cap-add=SYS_PTRACE \
	--security-opt seccomp=unconfined \
	-e CXX="clang++" \
	-e CXXFLAGS="-Wall -Wextra -Werror -std=c++20" \
	-e LDFLAGS="" \
	$(IMAGE) sh -c "cd /pwd; bash"

docker-build:
	docker build -t $(IMAGE) .

docker-exec:
	docker exec -it $(CONTAINER) sh -c "cd /pwd; bash"

basic: all
	./webserv config-files/valid/basic.conf

test: all
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./webserv config-files/test.conf

run: all
	./webserv config-files/valid/complexe-server.conf

pytest: all
	./webserv config-files/test.conf

db:
	lldb webserv -- config-files/valid/multiple-servers.conf

.PHONY	: clean fclean re docker-pwd docker-build docker-exec basic test run db