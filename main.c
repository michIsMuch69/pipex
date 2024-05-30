/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedusser <jedusser@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 08:09:43 by jedusser          #+#    #+#             */
/*   Updated: 2024/05/30 16:16:39 by jedusser         ###   ########.fr       */
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
#include <fcntl.h>

// Custom exec

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
	while (entity!= NULL)
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
	//pas trouve ?? cherche dans builtin ?? oui > exec builtin , non > pas trouve > command not found;
	free_array(path_list);
	return (result);
}

char	*ft_concat_path(char *directory, char *prompt)
{
	size_t	total_length;
	char *exec_path;

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
	while(cmds[i])
	{
		printf("commande %d = [%s]\n",i + 1, cmds[i]);
		i++;
	}
}
int	my_exec(int i, char **cmds, char **argv, char **envp)
{
	char	*directory;
	char	*cmd_path;

	directory = check_all_dirs(cmds[i]);
	if (!directory)
		return (1);
	cmd_path = ft_concat_path(directory, cmds[i]);
	printf("path to exec : %s\n", cmd_path);
	if (!cmd_path)
		return (free(directory), 1);
	print_cmds(cmds);
	if(execve(cmd_path, argv, envp) == -1)
	{
		perror("execve failed");
		free(cmd_path);
		return (1);
	}
	free(cmd_path);
	return (0);
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
	while(i < argc && argv[i] != NULL)
	{
		cmds[j] = malloc((ft_strlen(argv[i]) + 1) * sizeof(char));
		if(!cmds[j])
			return (NULL);
		cmds[j] = strdup(argv[i]);
		j++;
		i++;
	}
	cmds[j] = NULL;
	return (cmds);
}

//FOR TEST
int	main(int argc, char **argv, char **envp)
{
	int		cmd_count;
	cmd_count = argc - 1;
	int		fds[2];
	pid_t	pids[cmd_count];
	int		i;
	char	**cmds;
	
	cmds = distribute_cmds(argc, argv);
	//test :
	print_cmds(cmds);
	
	i = 0;
	pipe(fds);
	while (i < cmd_count) 
	{
		// pipe following fork
		pids[i] = fork();
		printf("%d\n", pids[i]);
		if (pids[i] == -1) 
		{
			perror("fork failed");
			exit(EXIT_FAILURE);
		} 
		else if (pids[i] == 0) 
		{
			close(fds[0]);
			dup2(fds[1], STDOUT_FILENO);
			my_exec(i, cmds, argv, envp);
			exit(0); 
		}
		else if (pids[i] > 0)
		{
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);
			my_exec(i, cmds, argv, envp);
		}
		i++;
	}
	i = 0;
	while (i < cmd_count) 
	{
		waitpid(pids[i], NULL, 0);
		i++;
	}
	free_array(cmds);
}
