/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/28 10:34:25 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/29 17:53:26 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <string.h>
#include <assert.h>

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

static void	apply_state(struct s_rl_state *state, size_t *last_index)
{
	if (*last_index > state->index)
		*last_index = state->index;
	move_in_buffer(state->buffer, state->y_offset + state->x_pos, *last_index,
			state->index - *last_index);
	state->x_len += state->index - *last_index;
	state->x_pos += state->index - *last_index;
	rl_render(state);
	*last_index = state->index;
}

static int	get_columns(int fd)
{
	struct winsize ws;

	if (ioctl(fd, TIOCGWINSZ, &ws) == -1)
		return (0);
	else
		return (ws.ws_col);
}

static void	init_state(struct s_rl_state *state, struct termios *orig)
{
	switch_to_raw(STDIN_FILENO, orig);
	state->tty_columns = get_columns(STDIN_FILENO) + 30;
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
	if (!state->echo_hook)
		state->echo_hook = rl_echo;
}

ssize_t		readline(struct s_rl_state *s)
{
	struct termios	orig_termios;
	size_t			last_index;
	ssize_t			r;
	enum e_rl_tok	token;

	init_state(s + (last_index = 0), &orig_termios);
	while (!s->end)
	{
		apply_state(s, &last_index);
		while (!(r = rl_token(&token, s->buffer + s->index, s->len - s->index)))
			if (s->len >= s->buffer_size || (r = read(STDIN_FILENO,
						s->buffer + s->len, s->buffer_size - s->len)) < 0)
			{
				tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
				return (r < 0 ? r : s->index);
			}
			else
				s->len += r;
		s->index += r;
		rl_delete_token_from_buffer(token, s);
		if (s->hooks[token])
			s->hooks[token](s);
	}
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	return (s->index);
}
