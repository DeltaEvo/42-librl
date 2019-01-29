/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/29 14:31:43 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/29 15:31:39 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include <string.h>
#include <stdio.h>

#define CSI "\33["

static void		clear_lines(struct rl_state *state)
{
	size_t	i;

	i = state->current_tty_line;
	while (i < state->tty_lines)
	{
		write(1, CSI "B", sizeof(CSI) + 1);
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

static void		print_line(struct rl_state *state, char *line, size_t line_len,
		bool has_prompt)
{
	size_t	i;

	i = state->tty_columns - state->prompt_size;
	if (!has_prompt)
		write(STDOUT_FILENO, "                           ", state->prompt_size);
	write(STDOUT_FILENO, line, line_len > i ? i : line_len);
	while (i < line_len)
	{
		write(STDOUT_FILENO, line + i, line_len - i > state->tty_columns
									? state->tty_columns : line_len - i);
		i += state->tty_columns;
	}
	if (i == line_len)
		write(STDOUT_FILENO, "\n", 1);
}

static size_t	print_lines(struct rl_state *state, size_t y)
{
	char	*line;
	char	*end;
	size_t	len;
	size_t	up_count;

	line = state->buffer;
	while (true)
	{
		len = state->index - (line - state->buffer);
		if ((end = memchr(line, '\n', len)))
			len = end - line;
		print_line(state, line, len, line == state->buffer);
		y += 1 + (state->prompt_size + len) / state->tty_columns;
		if ((size_t)(line - state->buffer) <= state->y_offset)
			up_count = y;
		if (!end)
			break ;
		write(STDOUT_FILENO, "\\\n\r", 3);
		line = end + 1;
	}
	up_count -= (state->prompt_size + state->x_len) / state->tty_columns
				- (state->prompt_size + state->x_pos) / state->tty_columns;
	state->current_tty_line = up_count;
	return (y);
}

void			rl_render(struct rl_state *state)
{
	size_t	i;

	clear_lines(state);
	write(STDOUT_FILENO, state->prompt, state->prompt_size);
	state->tty_lines = print_lines(state, 0);
	printf(CSI "%zuG", (state->prompt_size + state->x_pos)
			% state->tty_columns + 1);
	fflush(stdout);
	i = state->tty_lines;
	while (i > state->current_tty_line)
	{
		write(1, CSI "A", sizeof(CSI) + 1);
		i--;
	}
}
