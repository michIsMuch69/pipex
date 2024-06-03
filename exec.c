/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedusser <jedusser@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 10:00:59 by jedusser          #+#    #+#             */
/*   Updated: 2024/06/03 10:10:48 by jedusser         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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

	total_length = ft_strlen(directory) + ft_strlen(prompt) + 2;
	exec_path = malloc(total_length);
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

int	my_exec(char *cmd, char **envp)
{
	char	**args;
	char	*directory;
	char	*cmd_path;

	args = ft_split(cmd, ' ');
	if (!args)
		return (-1);
	directory = check_all_dirs(args[0]);
	if (!directory)
		return (free_array(args), -1);
	cmd_path = ft_concat_path(directory, args[0]);
	if (!cmd_path)
		return (free(directory), free_array(args), -1);
	if (execve(cmd_path, args, envp) == -1)
		return (perror("execve failed"), free_array(args), free(cmd_path), -1);
	return (0);
}
