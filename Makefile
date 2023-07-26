NAME		:= webserv
CXXFLAGS	?= -Wall -Wextra -Werror -std=c++20
LDFLAGS		?=
OBJECTS		:=	main.o
HEADERS		:=
CONTAINER	:= webserv-container
IMAGE		:= ubuntu-c-plus

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o : %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean :
	$(RM) $(OBJECTS)

fclean : clean
	$(RM) $(NAME)

re : fclean all

docker-pwd:
	docker run \
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

docker-new:
	docker exec -it $(CONTAINER) sh -c "cd /pwd; bash"

.PHONY	: clean fclean re docker-pwd docker-pwd-val docker-build
