/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedusser <jedusser@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 08:09:43 by jedusser          #+#    #+#             */
/*   Updated: 2024/05/30 10:53:51 by jedusser         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>
#include <readline/readline.h> 
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>


// ###### CUSTOM_EXEC ######

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
//this function checks if exec is found within a directory.

int	exec_found(const char *dirname, char *exec_searched)
{
	DIR				*dir;
	struct dirent	*entity;

	dir = opendir(dirname);
	if (!dir)
	{
		//perror("Failed to open directory");
		return (-1);
	}
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
//this function checks if exec is found in all concerned directories 
//and returns the directory where it has been found.

char	*check_all_dirs(char *exec_searched)
{
	const char	*paths;
	char		**path_list;
	char		*result;
	int			i;

	paths = getenv("PATH");
	if(!paths)
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
	directory = check_all_dirs(prompt);
	if (!directory)
		return (NULL);

	total_length = strlen(directory) + strlen(prompt) + 2; 
	char *exec_path = malloc(total_length);
	if (!exec_path)
	{
		free(directory);
		return (NULL);
	}
	ft_strcpy(exec_path, directory); 
	ft_strcat(exec_path, "/");      
	ft_strcat(exec_path, prompt);
	free(directory);
	return (exec_path);
}
int	my_exec(int i, char **cmds, char **argv, char **envp)
{
	char	*directory;
	char	*cmd_path;
	
	directory = check_all_dirs(cmds[i]);
	//printf("%s\n", cmds[i]);
	if (!directory)
		return (1);
	cmd_path = ft_concat_path(directory, cmds[i]);
	if(!cmd_path)
		return (1);
	free(directory);
	execve(cmd_path, argv, envp);
	free (cmd_path);
	return (0);
}

// ###### CUSTOM_EXEC ######

// ###### PIPES_HANDLING ######

void create_pipes(int fds[][2], int cmd_count) 
{
    int i = 0;
    while (i < cmd_count - 1) 
    {
        if (pipe(fds[i]) == -1) 
        {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
        i++;
    }
}

void close_fds(int fds[][2], int cmd_count)
{
    int i = 0;
    while (i < cmd_count - 1) 
    {
        close(fds[i][0]);
        close(fds[i][1]);
        i++;
    }
}

void child(int i, char **cmds, int fds[][2], char **argv, char **envp)
{
	printf("Child executing \n");
	if (*fds[i] >= 0)
	{
		dup2(*fds[i], STDOUT_FILENO /*1*/   ); // mettre les informations dans le fd[1]
		close(fds[i][0]);
	}
	if (i > 0)
        close(fds[i - 1][1]);
	// my_exec(args[0], args, envp);
	my_exec(i, cmds, argv, envp);
	exit(2);
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
	while(i < argc)
	{
		cmds[j] = malloc((ft_strlen(argv[i]) + 1) * sizeof(char));
		printf("size of cmd = %zu\n", ft_strlen(argv[i]));
		if(!cmds[j])
			return (NULL);
		cmds[j] = argv[i];
		j++;
		i++;
	}
	cmds[i] = NULL;
	return (cmds);
}

int	main(int argc, char **argv, char **envp)
{
	int		cmd_count;
	cmd_count = argc - 1;
	printf("%d\n", cmd_count);
	int		fds[cmd_count - 1][2];
	pid_t	pids[cmd_count];
	int		i;
	char	**cmds;
	
	cmds = distribute_cmds(argc, argv);
	printf("CMD1 : %s\n", cmds[0]);
	printf("CMD2 : %s\n", cmds[1]);
	printf("CMD3 : %s\n", cmds[2]);
	printf("CMD4 : %s\n", cmds[3]);
	printf("CMD5 : %s\n", cmds[4]);
	i = 0;
    while (i < cmd_count) 
    {
        pids[i] = fork();
        if (pids[i] == -1) 
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } 
        else if (pids[i] == 0) 
        {
        	child(i, cmds, fds, argv, envp);
        	exit(0); 
        }
        i++;
    }
    i = 0;
    while (i < cmd_count) 
    {
        waitpid(pids[i], NULL, 0);
        i++;
    }
    close_fds(fds, cmd_count);
}