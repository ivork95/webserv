VPATH		:= src
NAME		:= webserv
LIBS		:= -I./include -I./spdlog/include
CXXFLAGS	?= -Wall -Wextra -Werror -std=c++20
LDFLAGS		?=
OBJECTS		:=	obj/main.o \
				obj/Client.o \
				obj/MultiplexerIO.o \
				obj/TcpServer.o \
				obj/Socket.o \
				obj/HttpMessage.o \
				obj/HttpRequest.o \
				obj/HttpResponse.o
HEADERS		:=	include/TcpServer.hpp \
				include/Client.hpp \
				include/MultiplexerIO.hpp \
				include/Socket.hpp \
				include/HttpMessage.hpp \
				include/HttpRequest.hpp \
				include/HttpResponse.hpp 
CONTAINER	:= webserv-container
IMAGE		:= ubuntu-c-plus

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

obj/%.o : %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(LIBS) -o $@ $<

clean :
	$(RM) -r obj

fclean : clean
	$(RM) $(NAME)

re : fclean all

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
	-e CXXFLAGS="-Wall -Wextra -std=c++20 -O1 -g -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer" \
	-e LDFLAGS="-g -fsanitize=address -fsanitize=leak" \
	$(IMAGE) sh -c "cd /pwd; bash"

docker-pwd-val:
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

.PHONY	: clean fclean re docker-pwd docker-pwd-val docker-build
