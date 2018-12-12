/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/12 09:12:12 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/12 09:16:09 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include "ft/io.h"
#include "ft/mem.h"
#include "ft/str.h"
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

static uint32_t	ft_abs(int32_t a)
{
	return (a > 0 ? a : -a);
}

int				*parse_file(const int fd, size_t *curr_size, size_t *size,
		uint32_t *max_height)
{
	char		*line;
	char		*line_o;
	size_t		i;
	t_readable	rd;
	int			*arr;

	if (get_next_line(fd, &line) != 1)
		return (NULL);
	line_o = line;
	*size = count_words(line, ' ');
	arr = malloc(*size * sizeof(int));
	i = 0;
	while (i < *size)
	{
		arr[i++] = ft_atoip(&line);
		if (ft_abs(arr[i - 1]) > *max_height)
			*max_height = ft_abs(arr[i - 1]);
	}
	free(line_o);
	*curr_size = *size;
	rd = init_readable(fill_fd, (void *)(uintptr_t)fd);
	ft_memcpy(rd.buffer, get_next_line_buff(fd)->data, get_next_line_buff(fd)->len);
	rd.len += get_next_line_buff(fd)->len;
	while (42)
	{
		arr = ft_realloc(arr, *curr_size * sizeof(int), (*curr_size + *size) * sizeof(int));
		i = 0;
		while (i < *size)
		{
			arr[*curr_size + i++] = ft_atoi_rd(&rd);
			if (ft_abs(arr[*curr_size + i - 1]) > *max_height)
				*max_height = ft_abs(arr[*curr_size + i - 1]);
		}
		if (rd.len == 0)
			break;
		/*if (io_peek(&rd) != '\n')
		{
			free(arr);
			return (NULL);
		}
		else*/
			rd.index++;
		*curr_size += *size;
	}
	return (arr);
}
