/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/28 10:34:25 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/29 15:43:03 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <string.h>

static bool	switch_to_raw(int fd, struct termios *orig_termios)
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
			break ;
		}
		i++;
	}
	return (i);
}

#include <assert.h>

static void	delete_from_buffer (enum e_rl_tok token, struct rl_state *state)
{
	size_t	size;

	if (token == RL_NONE)
		return ;
	else if ((token >= RL_CTRL_A && token <= RL_CTRL_Z) || token == RL_ESC
			|| token == RL_DELETE)
		size = 1;
	else if (token == RL_UP || token == RL_DOWN || token == RL_RIGHT
			|| token == RL_LEFT || token == RL_HOME || token == RL_END)
		size = 3;
	else
		assert(false);
	assert(state->index >= size);
	memmove(state->buffer + state->index - size, state->buffer + state->index,
			state->len - state->index);
	state->index -= size;
	state->len -= size;
}

static void	move_in_buffer(char *buffer, size_t to, size_t start, size_t len)
{
	char	c;
	size_t	i;

	if (start == to || len == 0)
		return ;
	assert(start > to);
	i = 0;
	while (i < len)
	{
		c = buffer[start + i];
		memmove(buffer + to + i + 1, buffer + to + i, start - to);
		buffer[to + i] = c;
		i++;
	}
}

/*
** Apply state to tty, last_index can be wrong if we remove a char
*/

static void	apply_state(struct rl_state *state, size_t last_index)
{
	if (last_index > state->index)
		last_index = state->index;
	move_in_buffer(state->buffer, state->y_offset + state->x_pos, last_index,
			state->index - last_index);
	state->x_len += state->index - last_index;
	state->x_pos += state->index - last_index;
	rl_render(state);
}

static void	init_default_hooks(struct rl_state *state)
{
	if (!state->hooks[RL_LEFT])
		state->hooks[RL_LEFT] = (t_rl_hook)rl_left;
	if (!state->hooks[RL_RIGHT])
		state->hooks[RL_RIGHT] = (t_rl_hook)rl_right;
	if (!state->hooks[RL_UP])
		state->hooks[RL_UP] = (t_rl_hook)rl_up;
	if (!state->hooks[RL_DOWN])
		state->hooks[RL_DOWN] = (t_rl_hook)rl_down;
	if (!state->hooks[RL_ENTER_CTRL_M])
		state->hooks[RL_ENTER_CTRL_M] = rl_enter;
	if (!state->hooks[RL_CTRL_C])
		state->hooks[RL_CTRL_C] = rl_ctrl_c;
	if (!state->hooks[RL_CTRL_D])
		state->hooks[RL_CTRL_D] = rl_ctrl_d;
	if (!state->hooks[RL_DELETE])
		state->hooks[RL_DELETE] = rl_delete;
}

static int	get_columns(int fd)
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
	state->tty_columns = get_columns(STDIN_FILENO);
	init_default_hooks(state);
	while (!state->end)
	{
		while ((r = readline_tok(&token, state->buffer + state->index,
						state->len - state->index)) == 0)
		{
			apply_state(state, last_index);
			last_index = state->index;
			if (state->len >= state->buffer_size)
			{
				state->end = true;
				break ;
			}
			if ((r = read(STDIN_FILENO, state->buffer + state->len,
							state->buffer_size - state->len)) < 0)
			{
				state->index = r;
				break ;
			}
			state->len += r;
		}
		state->index += r;
		delete_from_buffer(token, state);
		if (state->hooks[token])
			state->hooks[token](state);
	}
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	return (state->index);
}
