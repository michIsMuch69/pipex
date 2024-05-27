/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedusser <jedusser@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 13:16:15 by jedusser          #+#    #+#             */
/*   Updated: 2024/05/27 16:00:30 by jedusser         ###   ########.fr       */
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
// this function checks if exec is found within a directory.

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
// this function checks if exec is found in all concerned directories 
// and returns the directory where it has been found.

char	*check_all_dirs(char **envp, char *exec_searched)
{
	const char	*paths = getenv("PATH");
	char		**path_list;
	int			i;

	path_list = ft_split(paths, ':');
	if (!path_list)
		return (NULL);
	i = 0;
	while (path_list[i])
	{
		if (exec_found(path_list[i], exec_searched) == 1)
		{
			return (path_list[i]);
			//printf("%s\n", path_list[i]);
			break ;
		}
		i++;
	}
	free_array(path_list);
	return (NULL);
}

char	*ft_concat_path(char **envp, char *prompt)
{
	char	*exec_path;
	
	exec_path = ft_strcat(check_all_dirs(envp, prompt), "/"); 
	exec_path = ft_strcat(exec_path, prompt);
	printf("%s\n", exec_path);
	return (exec_path);
}


int main(int argc, char **argv, char **envp)
{
	if (argc < 2)
		return 1;
	
	char	*cmd1_path;
	char	*cmd2_path;
	char	*directory;
	//int filedes[2];
	pid_t	pid;
	int file = open ("cmd_result.txt", O_WRONLY | O_CREAT, 0777);
	int file2 = dup2(file, STDOUT_FILENO);
	//close(file);
	//printf("hello");
	directory = check_all_dirs(envp, argv[1]);
	if (!directory)
		return 1;
		
	cmd1_path = ft_concat_path(&directory, argv[1]);
	if (!cmd1_path)
		return 1;
		
	free(directory);

	pid = fork();
	if(pid == 0)
	{
		printf("Executing: %s\n", cmd1_path);
		execve(cmd1_path, &argv[1], envp);
		free(cmd1_path);	
	}
	if(pid > 0)
		waitpid(pid, NULL, 0);
	
	// if(!argv[2])
	// 	return (1);
	// else if(argv[2][0] != '-')
	// {
	// 	directory = check_all_dirs(envp, argv[2]);
	// 	cmd2_path = ft_concat_path(&directory, argv[2]);
	// }
	// else
	// {
	// 	directory = check_all_dirs(envp, argv[3]);
	// 	cmd2_path = ft_concat_path(&directory, argv[3]);
	// }

	return 0;
}
