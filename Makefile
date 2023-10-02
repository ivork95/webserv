NAME		:=	webserv

VPATH		:=	src:

OBJECTS		:=	obj/main.o \
				obj/Client.o \
				obj/Cgi.o \
				obj/HttpMessage.o \
				obj/HttpRequest.o \
				obj/HttpResponse.o \
				obj/MultiplexerIO.o \
				obj/Socket.o \
				obj/Timer.o \
				obj/TcpServer.o \
				obj/InitConfig.o \
				obj/config/Token.o \
				obj/config/Lexer.o \
				obj/config/Parser.o \
				obj/config/Configuration.o \
				obj/config/ServerConfig.o \
				obj/config/LocationConfig.o \
				obj/config/ErrorPageConfig.o \
				obj/utils/config/hasConversionUnit.o \
				obj/utils/config/isAlphaNum.o \
				obj/utils/config/isNumber.o \
				obj/utils/config/isValidCgiExtension.o \
				obj/utils/config/isValidConfigExtension.o \
				obj/utils/config/isValidErrorCode.o \
				obj/utils/config/isValidHttpMethod.o \
				obj/utils/config/isValidIndexExtension.o \
				obj/utils/config/isValidIpv4.o \
				obj/utils/config/isValidPath.o \
				obj/utils/config/isValidPortNumber.o \
				obj/utils/config/isValidServerName.o \
				obj/utils/config/isValidUri.o \
				obj/logger/Logger.o \

CXXFLAGS	?=	-Wall -Wextra -Werror -std=c++20
LDFLAGS		?=

ifeq ($(PARSTER),true)
    CXXFLAGS += -DPARSTER
endif

INCL_DIR	:=	includes/
SPDLOGLIB	:=	./spdlog/build/libspdlog.a
SPDLOGINCL	:=	-DSPDLOG_COMPILED_LIB -I./spdlog/include

CONTAINER	:=	webserv-container
IMAGE		:=	ubuntu-c-plus

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(SPDLOGLIB)

obj/%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(SPDLOGINCL) -o $@ $< -I$(INCL_DIR)

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

pflag: re
	./webserv config-files/valid/complexe-server.conf

db:
	lldb webserv -- config-files/valid/multiple-servers.conf

.PHONY	: clean fclean re docker-pwd docker-pwd-val docker-build
