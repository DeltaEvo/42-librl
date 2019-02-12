/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/11 13:05:29 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/02/12 11:08:07 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include <unistd.h>

void	*rl_memmove(void *dst, const void *src, size_t len)
{
	const char	*c_src = src;
	char		*c_dst;

	c_dst = dst;
	if (dst > src)
		while (len--)
			c_dst[len] = c_src[len];
	else
		while (len--)
			*c_dst++ = *c_src++;
	return (dst);
}

void	*rl_memchr(const void *s, int c, size_t n)
{
	const int		*c_s = s;
	unsigned char	*b_s;
	int				repeated_c;
	int				mask;

	repeated_c = (c & 0xFF) | ((c & 0xFF) << 8);
	repeated_c |= repeated_c << 16;
	while (n >= 4)
	{
		mask = *c_s++ ^ repeated_c;
		if ((mask & 0xff) == 0)
			return ((void *)c_s - 4);
		if ((mask & 0xff00) == 0)
			return ((void *)c_s - 3);
		if ((mask & 0xff0000) == 0)
			return ((void *)c_s - 2);
		if ((mask & 0xff000000) == 0)
			return ((void *)c_s - 1);
		n -= 4;
	}
	b_s = (unsigned char *)c_s;
	while (n--)
		if (*b_s++ == (unsigned char)c)
			return ((void *)b_s - 1);
	return (NULL);
}

void		rl_putnbr_fd(int fd, int n)
{
	char		buf[11];
	int			i;
	const char	neg = n < 0;

	if (!neg)
		n = -n;
	i = sizeof(buf);
	if (!n)
		buf[--i] = '0';
	else
		while (n)
		{
			buf[--i] = -(n % 10) + '0';
			n /= 10;
		}
	if (neg)
		buf[--i] = '-';
	write(fd, buf + i, sizeof(buf) - i);
}
