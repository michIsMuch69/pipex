/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jean-micheldusserre <jean-micheldusserr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 08:09:43 by jedusser          #+#    #+#             */
/*   Updated: 2024/05/30 17:04:59 by jean-michel      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

// Custom exec functions
int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return (s1[i] - s2[i]);
}

void	free_array(char **array)
{
	int	i;

	i = 0;
	while (array[i])
		free(array[i++]);
	free(array);
}

int	exec_found(const char *dirname, char *exec_searched)
{
	DIR				*dir;
	struct dirent	*entity;

	dir = opendir(dirname);
	if (!dir)
		return (-1);
	entity = readdir(dir);
	while (entity != NULL)
	{
		if (ft_strcmp(entity->d_name, exec_searched) == 0)
		{
			closedir(dir);
			return (1);
		}
		entity = readdir(dir);
	}
	closedir(dir);
	return (0);
}

char	*check_all_dirs(char *exec_searched)
{
	const char	*paths;
	char		**path_list;
	char		*result;
	int			i;

	paths = getenv("PATH");
	if (!paths)
		return (NULL);
	path_list = ft_split(paths, ':');
	result = NULL;
	if (!path_list)
		return (NULL);
	i = 0;
	while (path_list[i])
	{
		if (exec_found(path_list[i], exec_searched) == 1)
		{
			result = ft_strdup(path_list[i]);
			break ;
		}
		i++;
	}
	free_array(path_list);
	return (result);
}

char	*ft_concat_path(char *directory, char *prompt)
{
	size_t	total_length;
	char	*exec_path;

	total_length = strlen(directory) + strlen(prompt) + 2;
	exec_path = malloc(total_length);
	if (!exec_path)
	{
		free(directory);
		return (NULL);
	}
	strcpy(exec_path, directory);
	strcat(exec_path, "/");
	strcat(exec_path, prompt);
	free(directory);
	return (exec_path);
}

void	print_cmds(char **cmds)
{
	int i;
	i = 0;
	while (cmds[i])
	{
		printf("commande %d = [%s]\n", i + 1, cmds[i]);
		i++;
	}
}

int	my_exec(char *cmd, char **envp)
{
	char	**args;
	char	*directory;
	char	*cmd_path;

	args = ft_split(cmd, ' ');
	if (!args)
		return (1);
	directory = check_all_dirs(args[0]);
	if (!directory)
		return (free_array(args), 1);
	cmd_path = ft_concat_path(directory, args[0]);
	if (!cmd_path)
		return (free(directory), free_array(args), 1);
	execve(cmd_path, args, envp);
	perror("execve failed");
	free(cmd_path);
	free_array(args);
	return (1);
}

char	**distribute_cmds(int argc, char **argv)
{
	char	**cmds;
	int		i;
	int		j;

	j = 0;
	i = 1;
	cmds = malloc((argc - 1) * sizeof(char *));
	if (!cmds)
		return (NULL);
	while (i < argc && argv[i] != NULL)
	{
		cmds[j] = strdup(argv[i]);
		if (!cmds[j])
			return (free_array(cmds), NULL);
		j++;
		i++;
	}
	cmds[j] = NULL;
	return (cmds);
}

int	main(int argc, char **argv, char **envp)
{
	int		cmd_count;
	int		fds[2];
	pid_t	pid;
	int		i;
	char	**cmds;
	int		prev_fd;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s cmd1 cmd2 ... cmdN\n", argv[0]);
		return (1);
	}
	cmds = distribute_cmds(argc, argv);
	if (!cmds)
		return (1);

	cmd_count = argc - 1;
	i = 0;
	prev_fd = 0;

	while (i < cmd_count)
	{
		if (pipe(fds) == -1)
		{
			perror("pipe failed");
			return (free_array(cmds), 1);
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork failed");
			return (free_array(cmds), 1);
		}
		else if (pid == 0)
		{
			if (i > 0)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (i < cmd_count - 1)
			{
				dup2(fds[1], STDOUT_FILENO);
				close(fds[1]);
			}
			close(fds[0]);
			my_exec(cmds[i], envp);
			exit(EXIT_FAILURE);
		}
		else
		{
			if (i > 0)
				close(prev_fd);
			if (i < cmd_count - 1)
				close(fds[1]);
			prev_fd = fds[0];
		}
		i++;
	}
	i = 0;
	while (i < cmd_count)
	{
		waitpid(-1, NULL, 0);
		i++;
	}
	free_array(cmds);
	return (0);
}
