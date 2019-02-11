/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/29 17:09:59 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/02/11 13:12:46 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rl.h"
#include "utils.h"

/*
** Try to match a CSI Sequence: ESC[char
** If not a CSI Sequence tok = ESC
*/

static void	rl_token_escape(enum e_rl_tok *tok, char *part, size_t size,
		size_t *i)
{
	size_t	j;

	j = *i;
	if (++j >= size)
		return ;
	if (part[j] != '[')
	{
		*tok = RL_ESC;
		*i = j;
		return ;
	}
	if (++j >= size)
		return ;
	if (part[j] == 'A' || part[j] == 'B')
		*tok = part[j] == 'A' ? RL_UP : RL_DOWN;
	else if (part[j] == 'C' || part[j] == 'D')
		*tok = part[j] == 'C' ? RL_RIGHT : RL_LEFT;
	else if (part[j] == 'H')
		*tok = RL_HOME;
	else if (part[j] == 'F')
		*tok = RL_END;
	*i = j + 1;
}

/*
** Char 1-26 = Ctrl+Letter ()
** Char 27 = ESC
** Char 127 = DEL
*/

size_t		rl_token(enum e_rl_tok *tok, char *part, size_t size)
{
	size_t	i;

	i = 0;
	*tok = RL_NONE;
	while (i < size)
	{
		if (part[i] <= 0x1f || part[i] == 127)
		{
			if (part[i] >= 1 && part[i] <= 26)
				*tok = part[i++];
			else if (part[i] == 27)
				rl_token_escape(tok, part, size, &i);
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

void		rl_delete_token_from_buffer(enum e_rl_tok token,
		struct s_rl_state *state)
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
		return ;

	ft_memmove(state->buffer + state->index - size, state->buffer + state->index,
			state->len - state->index);
	state->index -= size;
	state->len -= size;
}
