NAME        = btc

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98

SRCS        = main.cpp BitcoinExchange.cpp


RM          = rm -f

all: $(NAME)

$(NAME): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(NAME)

clean: fclean

fclean:
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re