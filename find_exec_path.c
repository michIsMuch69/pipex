/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_exec_path.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedusser <jedusser@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 08:51:23 by jedusser          #+#    #+#             */
/*   Updated: 2024/05/30 08:51:55 by jedusser         ###   ########.fr       */
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

	//printf("%s\n", exec_path);

	free(directory);
	return (exec_path);
}
int	my_exec(char *cmd, char **argv, char **envp)
{
	char	*directory;
	char	*cmd_path;
	
	directory = check_all_dirs(cmd);
	printf("%s\n", cmd);
	if (!directory)
		return (1);
	cmd_path = ft_concat_path(directory, cmd);
	if(!cmd_path)
		return (1);
	free(directory);
	execve(cmd_path, argv, envp);
	free (cmd_path);
	return (0);
}