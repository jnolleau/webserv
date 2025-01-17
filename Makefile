	
NAME = webserv

# -------------- COLORS -------------- #

RED			= \033[1;31m
GREEN		= \033[1;32m
YELLOW		= \033[1;33m
END			= \033[1;0m

# ---------------- CC ---------------- #

CC = clang++

CFLAGS		+=	-Wall
CFLAGS		+=	-Wextra

ifneq ($(err), no)
	CFLAGS +=	-Werror
endif

EXTRAFLAGS	=	-std=c++98

ifeq ($(leak), yes)
	SANITIZE	=	-g3 -fsanitize=address,undefined
else
	SANITIZE	=	-g
endif



# -------------- INCLUDES ------------ #

INCLUDES 		=	./includes/
SERVER_DIR 		=	$(SRCS_DIR)/Server/
REQUEST_DIR 	=	$(SRCS_DIR)/Request/
RESPONSE_DIR 	=	$(SRCS_DIR)/Response/
CGI_HAND_DIR 	=	$(SRCS_DIR)/CGI/
CONF_DIR 		=	$(SRCS_DIR)/Config/
UTILS_DIR 		=	$(SRCS_DIR)/Utils/
AUTO_DIR 		=	$(SRCS_DIR)/Autoindex/

ALL_INC			+= 	-I$(INCLUDES)
ALL_INC			+= 	-I$(SERVER_DIR)
ALL_INC			+= 	-I$(REQUEST_DIR)
ALL_INC			+= 	-I$(CGI_HAND_DIR)
ALL_INC			+= 	-I$(CONF_DIR)
ALL_INC			+= 	-I$(UTILS_DIR)
ALL_INC			+= 	-I$(RESPONSE_DIR)
ALL_INC			+= 	-I$(AUTO_DIR)
# ALL_INC			+= 	-I$(LIBFT_DIR)includes/


# ---------------- SRC --------------- #
# Add files to compile here

SRCS_DIR = ./srcs

# main sources
SRCS += webserv.cpp

# core sources
SRCS += Server.cpp
SRCS += Response.cpp
SRCS += Response_utils.cpp
SRCS += Response_headers.cpp
SRCS += request_class.cpp
SRCS += request_class_store.cpp
SRCS += request_class_utils.cpp
SRCS += CgiHandler.cpp

# Config Parser sources
SRCS += ConfParser.cpp
SRCS += HttpBlock.cpp
SRCS += ServerBlock.cpp
SRCS += LocationBlock.cpp

# Utils sources
SRCS += ip_manipulation.cpp
SRCS += utils.cpp
SRCS += Autoindex.cpp


# Add directories containing files to compile here
vpath %.cpp $(SRCS_DIR)/
vpath %.cpp $(SERVER_DIR)
vpath %.cpp $(REQUEST_DIR)
vpath %.cpp $(CGI_HAND_DIR)
vpath %.cpp $(UTILS_DIR)
vpath %.cpp $(CONF_DIR)
vpath %.cpp $(AUTO_DIR)
vpath %.cpp $(RESPONSE_DIR)


# ---------------- OBJ --------------- #

DIR_OBJS	= ./objs/
OBJS		= $(patsubst %.cpp, $(DIR_OBJS)%.o, $(SRCS))

# ---------------- LIB --------------- #

LIBFT_DIR 	= ./libft/
LIBFT 		= $(LIBFT_DIR)libft.a
LIB_LINK	= -L$(LIBFT_DIR) -lft


all: $(NAME)

fast:
	$(MAKE) re -j8

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(EXTRAFLAGS) $(SANITIZE) $(OBJS) $(ALL_INC) -o $@
			echo "$(GREEN)==> $@ linked$(END)"

$(OBJS): 	$(DIR_OBJS)%.o: %.cpp | $(DIR_OBJS)
			$(CC) $(CFLAGS) $(EXTRAFLAGS) $(SANITIZE) $(ALL_INC) -c $< -o $@
			echo "$(YELLOW)--> $@ made$(END)"

$(DIR_OBJS):
	mkdir -p $@

$(LIBFT):
	echo "$(YELLOW)--> Creating $@...$(END)"
	$(MAKE) -C $(LIBFT_DIR)
	echo "$(GREEN)==> $@ linked$(END)"

clean:
# 	$(MAKE) clean -C $(LIBFT_DIR)
	$(RM) -R $(DIR_OBJS)
# 	echo "$(RED)-> $@ $(NAME) made$(END)"

fclean: clean
# 	$(MAKE) fclean -C $(LIBFT_DIR)
	$(RM) $(NAME)
# 	echo "$(RED)-> $@ $(NAME) made$(END)"

re: fclean
	$(MAKE)

.PHONY:		all fast clean fclean re
.SILENT:	$(NAME) $(OBJS) $(DIR_OBJS) $(LIBFT) # clean fclean
