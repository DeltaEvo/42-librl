/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/29 14:31:43 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/02/12 11:17:02 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include "utils.h"

#define CSI "\33["

void			rl_echo(struct s_rl_state *state, char *part, size_t size)
{
	(void)state;
	write(STDOUT_FILENO, part, size);
}

static void		clear_lines(struct s_rl_state *state)
{
	size_t	i;

	i = state->current_tty_line;
	while (i < state->tty_lines)
	{
		write(1, CSI "B", sizeof(CSI));
		i++;
	}
	i = 1;
	while (i < state->tty_lines)
	{
		write(1, CSI "2K", sizeof(CSI) + 1);
		write(1, CSI "A", sizeof(CSI));
		i++;
	}
	write(1, CSI "2K" CSI "0G", sizeof(CSI) * 2 + 2);
}

static void		print_line(struct s_rl_state *state, char *line,
		size_t line_len, bool has_prompt)
{
	size_t	i;

	i = state->tty_columns - state->prompt_len;
	if (!has_prompt)
		write(STDOUT_FILENO, "                           ", state->prompt_len);
	state->echo_hook(state, line, line_len);
	if ((line_len + state->prompt_len) % state->tty_columns == 0)
		write(STDOUT_FILENO, "\n", 1);
}

static size_t	print_lines(struct s_rl_state *state, size_t y)
{
	char	*line;
	char	*end;
	size_t	len;
	size_t	up_count;

	line = state->buffer;
	up_count = 0;
	while (line <= state->buffer + state->index)
	{
		len = state->index - (line - state->buffer);
		if ((end = rl_memchr(line, '\n', len)))
			len = end - line;
		print_line(state, line, len, line == state->buffer);
		y += 1 + (state->prompt_len + len) / state->tty_columns;
		if ((size_t)(line - state->buffer) <= state->y_offset)
			up_count = y;
		if (!end)
			break ;
		write(STDOUT_FILENO, "\\\n\r", 3);
		line = end + 1;
	}
	up_count -= (state->prompt_len + state->x_len) / state->tty_columns
				- (state->prompt_len + state->x_pos) / state->tty_columns;
	state->current_tty_line = up_count;
	return (y);
}

void			rl_render(struct s_rl_state *state)
{
	size_t	i;

	clear_lines(state);
	write(STDOUT_FILENO, state->prompt, state->prompt_size);
	state->tty_lines = print_lines(state, 0);
	write(STDOUT_FILENO, CSI, sizeof(CSI) - 1);
	rl_putnbr_fd(STDOUT_FILENO, (state->prompt_len + state->x_pos)
			% state->tty_columns + 1);
	write(STDOUT_FILENO, "G", 1);
	i = state->tty_lines;
	while (i > state->current_tty_line)
	{
		write(1, CSI "A", sizeof(CSI));
		i--;
	}
}
