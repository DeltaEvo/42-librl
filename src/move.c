/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   move.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/29 11:27:56 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/29 16:30:16 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include <string.h>

bool	rl_up(struct s_rl_state *state)
{
	size_t	old_offset;

	if (state->y_offset > 0)
	{
		old_offset = state->y_offset;
		state->y_offset--;
		while (state->y_offset > 0
				&& state->buffer[state->y_offset - 1] != '\n')
			state->y_offset--;
		state->x_len = old_offset - state->y_offset - 1;
		if (state->x_pos > state->x_len)
			state->x_pos = state->x_len;
		return (true);
	}
	else
		return (false);
}

bool	rl_down(struct s_rl_state *state)
{
	char	*next;

	next = memchr(state->buffer + state->y_offset, '\n',
			state->len - state->y_offset);
	if (next)
	{
		state->y_offset = next - state->buffer + 1;
		state->x_len = 0;
		while (state->y_offset + state->x_len < state->len
				&& state->buffer[state->y_offset + state->x_len] != '\n')
			state->x_len++;
		if (state->x_pos > state->x_len)
			state->x_pos = state->x_len;
		return (true);
	}
	else
		return (false);
}

bool	rl_left(struct s_rl_state *state)
{
	if (state->x_pos > 0)
	{
		state->x_pos--;
		return (true);
	}
	else if (rl_up(state))
	{
		if (state->x_len)
			state->x_pos = state->x_len;
		return (true);
	}
	else
		return (false);
}

bool	rl_right(struct s_rl_state *state)
{
	if (state->x_pos < state->x_len)
	{
		state->x_pos++;
		return (true);
	}
	else if (rl_down(state))
	{
		state->x_pos = 0;
		return (true);
	}
	else
		return (false);
}
