/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rl.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/28 10:24:58 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/02/12 10:58:07 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIB_RL_H
# define LIB_RL_H

# include <stddef.h>
# include <unistd.h>
# include <stdbool.h>

enum		e_rl_tok {
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
	RL_END,
	RL_TOKEN_LEN
};

struct s_rl_state;

typedef void	(*t_rl_hook)(struct s_rl_state *state);

struct			s_rl_state {
	char		*prompt;
	size_t		prompt_size;
	size_t		prompt_len;

	char		*buffer;
	size_t		buffer_size;

	size_t		index;
	size_t		last_index;
	size_t		len;
	size_t		x_pos;
	size_t		x_len;
	size_t		y_offset;

	size_t		tty_columns;
	size_t		tty_lines;
	size_t		current_tty_line;

	bool		end;

	t_rl_hook	hooks[RL_TOKEN_LEN];
	void		(*echo_hook)(struct s_rl_state *state, char *part, size_t size);

	void		*user_data;
};

ssize_t			readline(struct s_rl_state *state);

void			rl_render(struct s_rl_state *state);
bool			rl_right(struct s_rl_state *state);
bool			rl_left(struct s_rl_state *state);
bool			rl_down(struct s_rl_state *state);
bool			rl_up(struct s_rl_state *state);
void			rl_ctrl_d(struct s_rl_state *state);
void			rl_ctrl_c(struct s_rl_state *state);
void			rl_enter(struct s_rl_state *state);
void			rl_delete(struct s_rl_state *state);
void			rl_echo(struct s_rl_state *state, char *part, size_t size);

/*
** Internals
*/

void			rl_delete_token_from_buffer (enum e_rl_tok token,
						struct s_rl_state *state);
size_t			rl_token(enum e_rl_tok *tok, char *part, size_t size);

#endif
