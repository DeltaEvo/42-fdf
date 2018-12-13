/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/12 09:12:12 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/13 09:37:47 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include "ft/mem.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static size_t	count_words(const char *s, char c)
{
	size_t	len;

	len = !(*s == c);
	while (*++s)
		if (s[-1] == c && *s != c)
			len++;
	return (len);
}

static int32_t	ft_atoip(char **str)
{
	char		*o;
	int32_t		result;
	char		mod;

	o = *str;
	while (**str == ' ' || (**str >= '\t' && **str <= '\r'))
		(*str)++;
	result = 0;
	mod = **str == '-' || **str == '+' ? *(*str)++ == '-' : 0;
	if (**str >= '0' && **str <= '9')
		while (**str >= '0' && **str <= '9')
			result = result * 10 + *(*str)++ - '0';
	else
	{
		*str = o;
		return (0);
	}
	return (mod ? -result : result);
}

static uint32_t	ft_abs(int32_t a)
{
	return (a > 0 ? a : -a);
}

static void		*free_ret(void *addr)
{
	free(addr);
	return (NULL);
}

int32_t			*parse_file(const int fd, size_t *curr_size, size_t *size,
		uint32_t *max_height)
{
	char		*line[2];
	int32_t		i;
	int32_t		*arr;

	*size = 0;
	*curr_size = 0;
	arr = NULL;
	while (get_next_line(fd, line) > 0)
	{
		if (!*size)
			*size = count_words(*line, ' ');
		line[1] = line[0];
		arr = ft_realloc(arr, *curr_size * 4, (*curr_size + *size) * 4);
		i = -1;
		while ((uint32_t)++i < *size)
			if (!**line)
				return (free_ret(arr));
			else if (ft_abs(arr[*curr_size + i] = ft_atoip(line)) > *max_height)
				*max_height = ft_abs(arr[*curr_size + i]);
		if (**line)
			return (free_ret(arr));
		*curr_size += *size;
		free(line[1]);
	}
	return (arr);
}
