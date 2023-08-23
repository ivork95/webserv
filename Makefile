VPATH		:= src
NAME		:= webserv
CXXFLAGS	?= -Wall -Wextra -Werror -std=c++20
LDFLAGS		?=
OBJECTS		:=	obj/main.o \
				obj/ClientSocket.o \
				obj/ServerIO.o \
				obj/TcpServer.o \
				obj/InitConfig.o \
				obj/config-parsing/Lexer.o \
				obj/config-parsing/Parser.o \
				obj/config-parsing/Configuration.o \
				obj/config-parsing/Token.o \
				obj/logger/Logger.o \
				obj/config-parsing/Server.o \
				obj/config-parsing/Route.o \
				obj/config-parsing/ErrorPage.o \

# HEADERS		:=	TcpServer.hpp \
# 				ClientSocket.hpp \
# 				ServerIO.hpp \
# 				config-parsing/Lexer.hpp \
# 				config-parsing/Parser.hpp \
# 				config-parsing/Configuration.hpp \
# 				config-parsing/Token.hpp \
# 				logger/Logger.hpp \

INCL_DIR	:=	includes/

CONTAINER	:= webserv-container
IMAGE		:= ubuntu-c-plus

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

obj/%.o : %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $< -I$(INCL_DIR)

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
	-e LDFLAGS="-g3 -fsanitize=address -fsanitize=leak" \
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

test: all
	./webserv config-files/valid/multiple-servers.conf

db:
	lldb webserv -- config-files/valid/multiple-servers.conf

.PHONY	: clean fclean re docker-pwd docker-pwd-val docker-build
