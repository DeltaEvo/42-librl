/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/11 13:06:23 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/02/12 13:08:20 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRL_UTILS_H
# define LIBRL_UTILS_H

# include <stddef.h>

void	*rl_memmove(void *dst, const void *src, size_t len);
void	*rl_memchr(const void *s, int c, size_t n);
void	rl_putnbr_fd(int fd, int m);
int		get_columns(int fd);

#endif
