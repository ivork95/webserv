VPATH		:=	src:
NAME		:=	webserv
CXXFLAGS	?=	-Wall -Wextra -Werror -std=c++20
LDFLAGS		?=
OBJECTS		:=	obj/Client.o \
				obj/Helper.o \
				obj/HttpMessage.o \
				obj/HttpRequest.o \
				obj/HttpResponse.o \
				obj/main.o \
				obj/MultiplexerIO.o \
				obj/Socket.o \
				obj/TcpServer.o \
				obj/Timer.o
HEADERS		:=	include/Client.hpp \
				include/Helper.hpp \
				include/HttpMessage.hpp \
				include/HttpRequest.hpp \
				include/HttpResponse.hpp \
				include/MultiplexerIO.hpp \
				include/Socket.hpp \
				include/StatusCodes.hpp \
				include/TcpServer.hpp \
				include/Timer.hpp
CONTAINER	:=	webserv-container
IMAGE		:=	ubuntu-c-plus
SPDLOGLIB	:=	./spdlog/build/libspdlog.a
SPDLOGINCLUDE	:= -DSPDLOG_COMPILED_LIB -I./spdlog/include
INCLUDE		:= -I./include

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(SPDLOGLIB)

obj/%.o : %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $(SPDLOGINCLUDE) -o $@ $<

clean :
	$(RM) -r obj

fclean : clean
	$(RM) $(NAME)

re : fclean all

docker-pwd-san:
	docker run \
	-p 12345:12345 \
	--name $(CONTAINER) \
	-it \
	--rm \
	--init \
	-v "$$PWD:/pwd" \
	--cap-add=SYS_PTRACE \
	--security-opt seccomp=unconfined \
	-e CXX="clang++" \
	-e CXXFLAGS="-Wall -Wextra -std=c++20 -O1 -g -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer" \
	-e LDFLAGS="-g -fsanitize=address -fsanitize=leak" \
	$(IMAGE) sh -c "cd /pwd; bash"

docker-pwd:
	docker run \
	-p 12345:12345 \
	--name $(CONTAINER) \
	-it \
	--rm \
	--init \
	-v "$$PWD:/pwd" \
	--cap-add=SYS_PTRACE \
	--security-opt seccomp=unconfined \
	-e CXX="clang++" \
	-e CXXFLAGS="-Wall -Wextra -std=c++20 -g -gdwarf-4 -gstrict-dwarf" \
	-e LDFLAGS="-g -gdwarf-4 -gstrict-dwarf" \
	$(IMAGE) sh -c "cd /pwd; bash"

docker-build:
	docker build -t $(IMAGE) .

docker-exec:
	docker exec -it $(CONTAINER) sh -c "cd /pwd; bash"

.PHONY	: clean fclean re docker-pwd-san docker-pwd-val docker-build