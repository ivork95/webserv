VPATH		:=	src:
NAME		:=	webserv
CXXFLAGS	?=	-Wall -Wextra -Werror -std=c++20
LDFLAGS		?=
OBJECTS		:=	obj/main.o \
				obj/Client.o \
				obj/Helper.o \
				obj/Message.o \
				obj/Request.o \
				obj/Response.o \
				obj/Multiplexer.o \
				obj/Socket.o \
				obj/Timer.o \
				obj/Server.o \
				obj/InitConfig.o \
				obj/config/Token.o \
				obj/config/Lexer.o \
				obj/config/Parser.o \
				obj/config/ServerParser.o \
				obj/config/LocationParser.o \
				obj/config/Configuration.o \
				obj/config/ServerConfig.o \
				obj/config/LocationConfig.o \
				obj/config/ErrorPageConfig.o \
				obj/utils/config/hasConversionUnit.o \
				obj/utils/config/isAlphaNum.o \
				obj/utils/config/isNumber.o \
				obj/utils/config/isValidCgiInterpreter.o \
				obj/utils/config/isValidCgiScript.o \
				obj/utils/config/isValidErrorCode.o \
				obj/utils/config/isValidHttpMethod.o \
				obj/utils/config/isValidIndexExtension.o \
				obj/utils/config/isValidIpv4.o \
				obj/utils/config/isValidPath.o \
				obj/utils/config/isValidPortNumber.o \
				obj/utils/config/isValidServerName.o \
				obj/utils/config/isValidUri.o \
				obj/utils/config/hasRequiredPermissions.o \
				obj/utils/config/ParserUtils.o \
				obj/utils/request/RequestUtils.o \
				obj/request/DirectoryListing.o \
				obj/request/RequestDelete.o \
				obj/request/RequestChunk.o \
				obj/request/RequestGet.o \
				obj/request/RequestPost.o \
				obj/cgi/CGIPipeIn.o \
				obj/cgi/CGIPipeOut.o
HEADERS		:=	include/CGIPipeIn.hpp \
				include/CGIPipeOut.hpp \
				include/Client.hpp \
				include/Helper.hpp \
				include/Message.hpp \
				include/Request.hpp \
				include/Response.hpp \
				include/Multiplexer.hpp \
				include/Socket.hpp \
				include/StatusCodes.hpp \
				include/Server.hpp \
				include/Timer.hpp \
				include/Token.hpp \
				include/Lexer.hpp \
				include/Parser.hpp \
				include/Configuration.hpp \
				include/ServerConfig.hpp \
				include/LocationConfig.hpp \
				include/ErrorPageConfig.hpp \
				include/UtilsConfig.hpp \
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

docker-pwd-san:
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
	-e CXXFLAGS="-Wall -Wextra -std=c++20 -O1 -g -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer" \
	-e LDFLAGS="-g3 -fsanitize=address -fsanitize=leak" \
	$(IMAGE) sh -c "cd /pwd; bash"

docker-pwd:
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
	-e CXXFLAGS="-Wall -Wextra -std=c++20 -g -gdwarf-4 -gstrict-dwarf" \
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
	-e CXXFLAGS="-Wall -Wextra -std=c++20" \
	-e LDFLAGS="" \
	$(IMAGE) sh -c "cd /pwd; bash"

docker-build:
	docker build -t $(IMAGE) .

docker-exec:
	docker exec -it $(CONTAINER) sh -c "cd /pwd; bash"

basic: all
	./webserv config-files/valid/basic.conf

py: all
	./webserv config-files/py.conf

test: all
	./webserv config-files/valid/multiple-servers.conf

run: all
	./webserv config-files/valid/complexe-server.conf

db:
	lldb webserv -- config-files/valid/multiple-servers.conf

.PHONY	: clean fclean re docker-pwd docker-pwd-val docker-build