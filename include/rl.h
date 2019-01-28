/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   librl.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/28 10:24:58 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/28 15:12:45 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRL
# define LIBRL

# include <stddef.h>
# include <unistd.h>
# include <stdbool.h>

struct		rl_state {
	char	*prompt;
	size_t	prompt_size;

	char	*buffer;
	size_t	buffer_size;

	size_t	index;
	size_t	len;
	size_t	x_pos;
	size_t	x_len;
	size_t	y_pos;
	size_t	y_len;
	bool	dirty;

	size_t	tty_columns;
};

enum e_rl_tok {
	RL_NONE,
	RL_CTRL_A = 1,
	RL_CTRL_B = 2,
	RL_CTRL_C = 3,
	RL_CTRL_D = 4,
	RL_CTRL_E = 5,
	RL_CTRL_F = 6,
	RL_CTRL_G = 7,
	RL_CTRL_H = 8,
	RL_TAB_CTRL_I = 9,
	RL_CTRL_J = 10,
	RL_CTRL_K = 11,
	RL_CTRL_L = 12,
	RL_ENTER_CTRL_M = 13,
	RL_CTRL_N = 14,
	RL_CTRL_O = 15,
	RL_CTRL_P = 16,
	RL_CTRL_Q = 17,
	RL_CTRL_R = 18,
	RL_CTRL_S = 19,
	RL_CTRL_T = 20,
	RL_CTRL_U = 21,
	RL_CTRL_V = 22,
	RL_CTRL_W = 23,
	RL_CTRL_X = 24,
	RL_CTRL_Y = 25,
	RL_CTRL_Z = 26,
	RL_ESC,
	RL_DELETE,
	RL_UP,
	RL_DOWN,
	RL_RIGHT,
	RL_LEFT,
	RL_HOME,
	RL_END
};

ssize_t	readline(struct rl_state *state);

#endif
