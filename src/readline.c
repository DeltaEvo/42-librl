/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/28 10:34:25 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/28 16:47:46 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/ioctl.h>

#define CSI "\33["

static bool switch_to_raw(int fd, struct termios *orig_termios)
{
    struct termios	raw;

    if (tcgetattr(fd, orig_termios) == -1)
		return (false);
    raw = *orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    if (tcsetattr(fd, TCSAFLUSH, &raw) < 0)
		return (false);
	return (true);
}


size_t	readline_tok(enum e_rl_tok *tok, char *part, size_t size)
{
	size_t	i;
	size_t	j;

	i = 0;
	*tok = RL_NONE;
	while (i < size)
	{
		if (iscntrl(part[i]))
		{
			if (part[i] >= 1 && part[i] <= 26)
				*tok = part[i++];
			else if (part[i] == 27)
			{
				j = i;
				if (++j < size)
				{
					if (part[j] == '[')
					{
						if (++j < size)
						{
							if (part[j] == 'A')
								*tok = RL_UP;
							else if (part[j] == 'B')
								*tok = RL_DOWN;
							else if (part[j] == 'C')
								*tok = RL_RIGHT;
							else if (part[j] == 'D')
								*tok = RL_LEFT;
							else if (part[j] == 'H')
								*tok = RL_HOME;
							else if (part[j] == 'F')
								*tok = RL_END;
							i = j + 1;
						}
					}
					else
					{
						*tok = RL_ESC;
						i = j;
					}
				}
			}
			else if (part[i] == 127)
			{
				i++;
				*tok = RL_DELETE;
			}
			break;
		}
		i++;
	}
	return (i);

}

#include <assert.h>

static void delete_from_buffer (enum e_rl_tok token, struct rl_state *state)
{
	size_t	size;

	if (token == RL_NONE)
		return ;
	else if ((token >= RL_CTRL_A && token <= RL_CTRL_Z) || token == RL_ESC || token == RL_DELETE)
		size = 1;
	else if (token == RL_UP || token == RL_DOWN || token == RL_RIGHT || token == RL_LEFT || token == RL_HOME || token == RL_END)
		size = 3;
	else
		assert(false);
	assert(state->index >= size);
	memcpy(state->buffer + state->index - size, state->buffer + state->index, state->len - state->index);
	state->index -= size;
	state->len -= size;
}

static void	move_in_buffer(char *buffer, size_t to, size_t start, size_t len)
{
	char	c;
	size_t	i;

	if (start == to)
		return ;
	assert(start > to);
	i = 0;
	while (i < len)
	{
		c = buffer[start + i];
		memcpy(buffer + to + i + 1, buffer + to + i, start - to);
		buffer[to + i] = c;
		i++;
	}
}

static void	render(struct rl_state *state)
{
	size_t	i;
	char	*line;
	char	*end;
	size_t	y;
	size_t	columns = state->tty_columns - state->prompt_size;
	size_t	len;
	bool	first;

	i = state->y_pos;
	while (i > 0)
	{
		write(1, CSI "B", sizeof(CSI) + 1);
		i--;
	}
	i = 1;
	while (i < state->y_len)
	{
		write(1, CSI "2K", sizeof(CSI) + 1);
		write(1, CSI "A", sizeof(CSI));
		i++;
	}
	write(1, CSI "2K" CSI "0G", sizeof(CSI) * 2 + 2);
	write(STDOUT_FILENO, state->prompt, state->prompt_size);
	line = state->buffer;
	y = 0;
	first = true;
	while (true)
	{
		len = state->index - (line - state->buffer);
		if(!(end = memchr(line, '\n', len)))
		{
			i = 0;
			len = state->index - (line - state->buffer);
			while (i < len)
			{
				if (first)
					first = false;
				else
					write(STDOUT_FILENO, "                                ", state->prompt_size);
				write(STDOUT_FILENO, line + i, columns > len - i ? len - i : columns);
				i += columns;
				y++;
			}
			break ;
		}
		i = 0;
		len = end - line;
		while (i < len)
		{
			if (first)
				first = false;
			else
				write(STDOUT_FILENO, "                                ", state->prompt_size);
			write(STDOUT_FILENO, line + i, columns > len - i ? len - i : columns);
			write(STDOUT_FILENO, "\\\n", 3);
			i += columns;
			y++;
		}
		line = end + 1;
	}
	state->y_len = y;
	/*write(STDOUT_FILENO, CSI "K", sizeof(CSI));
	write(STDOUT_FILENO, state->buffer + state->x_pos, (state->x_len - state->x_pos) % (state->tty_columns - state->prompt_size));*/
	printf(CSI "%zuG", state->prompt_size + 1 + state->x_pos % columns);
	state->y_pos = len / columns - state->x_pos / columns; 
	i = state->y_pos;
	while (i > 0)
	{
		write(1, CSI "A", sizeof(CSI) + 1);
		i--;
	}
	fflush(stdout);
}

/*
** Apply state to tty, last_index can be wrong if we remove a char
*/
static void	apply_state(struct rl_state *state, size_t last_index)
{
	if (last_index > state->index)
		last_index = state->index;
	move_in_buffer(state->buffer, state->x_pos, last_index, state->index - last_index);
	state->x_len += state->index - last_index;
	state->x_pos += state->index - last_index;
	render(state);
}

static bool reduce_state(struct rl_state *state, enum e_rl_tok token)
{
	if (token == RL_LEFT && state->x_pos > 0)
		state->x_pos--;
	else if (token == RL_RIGHT && state->x_pos < state->x_len)
		state->x_pos++;
	else if (token == RL_ENTER_CTRL_M)
	{
		if (state->buffer[state->x_pos - 1] == '\\')
		{
			state->buffer[state->x_pos - 1] = '\n';
			state->x_pos = 0;
			state->x_len = 0;
			state->y_pos++;
			state->y_len++;
		}
		else
			return (true);
	}
	else if (token == RL_CTRL_C)
	{
		state->index = 0;
		return (true);
	}
	else if (token == RL_CTRL_D)
	{
		state->index = -1;
		return (true);
	}
	else if (token == RL_DELETE && state->x_pos > 0)
	{
		memcpy(state->buffer + state->x_pos - 1, state->buffer + state->x_pos, state->len - state->x_pos);
		state->index--;
		state->len--;
		state->x_pos--;
		state->x_len--;
		write(1, CSI "D", sizeof(CSI));
		state->dirty = true;
	}
	else if (token == RL_NONE)
		state->dirty = true;
	return (false);
}

static int get_columns(int fd)
{
	struct winsize ws;

	if (ioctl(fd, TIOCGWINSZ, &ws) == -1)
		return (0);
	else
		return (ws.ws_col);
}

ssize_t	readline(struct rl_state *state)
{
	struct termios	orig_termios;
	size_t			last_index;
	ssize_t			r;
	enum e_rl_tok	token;

	switch_to_raw(STDIN_FILENO, &orig_termios);
	last_index = 0;
	token = RL_NONE;
	state->tty_columns = get_columns(STDIN_FILENO);
	while (true)
	{
		if (reduce_state(state, token))
			break ;
		if (state->dirty)
		{
			apply_state(state, last_index);
			last_index = state->index;
			if (state->len >= state->buffer_size)
				break ;
			if ((r = read(STDIN_FILENO, state->buffer + state->len, state->buffer_size - state->len)) < 0)
			{
				state->index = r;
				break ;
			}
			state->len += r;
			state->dirty = false;
		}
		state->index += readline_tok(&token, state->buffer + state->index, state->len - state->index);
		delete_from_buffer(token, state);
	}
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	return (state->index);
}