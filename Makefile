# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jedusser <jedusser@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/27 12:03:52 by jedusser          #+#    #+#              #
#    Updated: 2024/06/03 10:09:29 by jedusser         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = pipex

SRCS_DIR = .

OBJS_PATH = .obj

SRCS = exec.c main.c utils.c

HDR = pipex.h

LIBFT_PATH = libft/
LIBFT_HDR_PATH = libft/hdr/

LIBFT = $(LIBFT_PATH)libft.a

FLAGS = -g3 -g -I$(LIBFT_HDR_PATH) -Iheader #-Wall -Wextra -Werror
LDFLAGS = -L$(LIBFT_PATH) -lft -lreadline

OBJS = $(SRCS:%.c=$(OBJS_PATH)/%.o)

default : all

all : $(LIBFT) $(NAME)

$(NAME) : $(OBJS) $(LIBFT)
	$(CC) $(FLAGS) $^ -o $@ $(LDFLAGS)
	
$(LIBFT) :
	$(MAKE) -C $(LIBFT_PATH) all 	

$(OBJS_PATH)/%.o : $(SRCS_DIR)/%.c $(HDR)
	mkdir -p $(OBJS_PATH)
	$(CC) $(FLAGS) -c $< -o $@

clean    :
	@rm -rf $(OBJS_PATH)
	$(MAKE) -C $(LIBFT_PATH) clean

fclean    : 
	$(MAKE) -C $(LIBFT_PATH) fclean
	@rm -rf $(OBJS_PATH) $(NAME)

re        : fclean all

.PHONY    : clean fclean re default all
