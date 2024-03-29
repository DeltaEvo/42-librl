/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/29 11:34:33 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/02/12 13:05:55 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include "utils.h"

void	rl_delete(struct s_rl_state *state)
{
	char	c;

	if (rl_left(state))
	{
		c = state->buffer[state->y_offset + state->x_pos];
		if (state->y_offset + state->x_pos != state->len)
			rl_memmove(state->buffer + state->x_pos + state->y_offset,
				state->buffer + state->x_pos + state->y_offset + 1, state->len
				- (state->x_pos + state->y_offset + 1));
		state->index--;
		state->len--;
		if (c == '\n')
		{
			while (state->y_offset + state->x_len < state->len
					&& state->buffer[state->y_offset + state->x_len] != '\n')
				state->x_len++;
		}
		else
			state->x_len--;
	}
}

void	rl_enter(struct s_rl_state *state)
{
	if (state->x_pos > 0
	&& state->buffer[state->y_offset + state->x_pos - 1] == '\\')
	{
		state->buffer[state->y_offset + state->x_pos - 1] = '\n';
		state->y_offset += state->x_pos;
		state->x_len = state->x_len - state->x_pos;
		state->x_pos = 0;
	}
	else
		state->end = true;
}

void	rl_ctrl_c(struct s_rl_state *state)
{
	state->index = 0;
	state->end = true;
}

void	rl_ctrl_d(struct s_rl_state *state)
{
	state->index = -1;
	state->end = true;
}
