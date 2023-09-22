VPATH		:=	src:
NAME		:=	webserv
CXXFLAGS	?=	-Wall -Wextra -Werror -std=c++20
LDFLAGS		?=
OBJECTS		:=	obj/main.o \
				obj/ClientSocket.o \
				obj/ServerIO.o \
				obj/TcpServer.o \
				obj/InitConfig.o \
				obj/config-parsing/Token.o \
				obj/config-parsing/Lexer.o \
				obj/config-parsing/Parser.o \
				obj/config-parsing/Configuration.o \
				obj/config-parsing/ServerConfig.o \
				obj/config-parsing/LocationConfig.o \
				obj/config-parsing/ErrorPageConfig.o \
				obj/utils/config/hasConversionUnit.o \
				obj/utils/config/isAlphaNum.o \
				obj/utils/config/isNumber.o \
				obj/utils/config/isValidCgiExtension.o \
				obj/utils/config/isValidErrorCode.o \
				obj/utils/config/isValidHttpMethod.o \
				obj/utils/config/isValidIndexExtension.o \
				obj/utils/config/isValidIpv4.o \
				obj/utils/config/isValidPath.o \
				obj/utils/config/isValidPortNumber.o \
				obj/utils/config/isValidServerName.o \
				obj/utils/config/isValidUri.o \
				obj/logger/Logger.o \

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
OBJECTS		:=	obj/Client.o \
				obj/Cgi.o \
				obj/HttpMessage.o \
				obj/HttpRequest.o \
				obj/HttpResponse.o \
				obj/main.o \
				obj/MultiplexerIO.o \
				obj/Socket.o \
				obj/TcpServer.o \
				obj/Timer.o
HEADERS		:=	include/Client.hpp \
				include/Cgi.hpp \
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
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $(SPDLOGINCLUDE) -o $@ $< -I$(INCL_DIR)

clean :
	$(RM) -r obj

fclean : clean
	$(RM) $(NAME)
	$(RM) -r ./parster-results

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
	-e LDFLAGS="-g3 -fsanitize=address -fsanitize=leak" \
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

basic: all
	./webserv config-files/valid/basic.conf

test: all
	./webserv config-files/valid/multiple-servers.conf

run: all
	./webserv config-files/valid/complexe-server.conf

db:
	lldb webserv -- config-files/valid/multiple-servers.conf

.PHONY	: clean fclean re docker-pwd docker-pwd-val docker-build
