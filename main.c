/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jedusser <jedusser@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 08:09:43 by jedusser          #+#    #+#             */
/*   Updated: 2024/06/03 12:16:12 by jedusser         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
// Custom exec functions

char	**distribute_cmds(int argc, char **argv)
{
	char	**cmds;
	int		i;
	int		j;

	j = 0;
	i = 1;
	cmds = malloc((argc) * sizeof(char *));
	if (!cmds)
		return (NULL);
	while (i < argc && argv[i] != NULL)
	{
		cmds[j] = ft_strdup(argv[i]);
		if (!cmds[j])
			return (free_array(cmds), NULL);
		j++;
		i++;
	}
	cmds[j] = NULL;
	return (cmds);
}

void	handle_child(int i, int fds[2], int cmd_count, int prev_fd, char **cmds, char **envp)
{
	if (i == 0)
	{
		int	input_fd = open("file1.txt", O_RDONLY);
		dup2(input_fd, STDIN_FILENO);
	}
	else // (i > 0) //not first cmd.
	{
		dup2(prev_fd, STDIN_FILENO); // --> proteger appel a pid, apres refacto);
		close(prev_fd);
	}
	if (i < cmd_count - 1) // not last cmd.
	{
		dup2(fds[1], STDOUT_FILENO);
		close(fds[1]);
	}
	else
	{
		int	output_fd = open("file2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
		dup2(output_fd, STDOUT_FILENO);
	}
	if (my_exec(cmds[i], envp) == -1)
		free_array(cmds);
	close(fds[0]);
}

void	handle_parent(int i, int fds[2], int prev_fd, int cmd_count)
{
	if (i > 0)
		close(prev_fd);
	if (i < cmd_count - 1)
		close(fds[1]);
}

void	wait_all(int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count)
	{
		waitpid(-1, NULL, 0);
		i++;
	}
}

int	pipex(int cmd_count, char **cmds, char **envp)
{
	int		i;
	int		prev_fd;
	int		fds[2];
	pid_t	pid;

	prev_fd = 0;
	i = 0;
	while (i < cmd_count)
	{
		if (pipe(fds) == -1)
			return (perror("pipe failed"), free_array(cmds), -1);
		pid = fork();
		if (pid == -1)
			return (perror("fork failed"), free_array(cmds), -1);
		else if (pid == 0)
			handle_child(i, fds, cmd_count, prev_fd, cmds, envp); // retour d'erreur ?
		else
		{
			handle_parent(i, fds, prev_fd, cmd_count); //retour d'erreur ?
			prev_fd = fds[0];
		}
		i++;
	}
	wait_all(cmd_count);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	int		cmd_count;
	char	**cmds;
	//
	//

	cmd_count = argc - 1;
	if (argc < 2)
		return (ft_printf("Usage: ./pipex cmd1 cmd2 ... cmdN\n"), 1);
	cmds = distribute_cmds(argc, argv);
	if (!cmds)
		return (2);
	if (pipex(cmd_count, cmds, envp) == -1)
		return (3);
	free_array(cmds);
	return (0);
}

// tests : 
// ./pipex  rev sort "cat -e" "tr 'o' 'x'" 