/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/06 14:18:46 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/07 16:22:19 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/mlx.h"
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ft/io.h"
#include "ft/mem.h"
#include "ft/str.h"
#include "ft/math.h"
#include "get_next_line.h"

#define WINDOW_SIZE (1000)

typedef	struct	s_fdf {
	void		*mlx;
	void		*win;
	t_mat4		mat;
	int			*arr;
	size_t		len;
	size_t		width;
	size_t		window_size;
}				t_fdf;

void	draw_line(t_fdf *fdf, int32_t x, int32_t y, int32_t xTo, int32_t yTo, int color)
{
	int32_t	xInc;
	int32_t	yInc;
	int32_t	dx;
	int32_t	dy;
	int32_t	err;
	int32_t	e2;

	xInc = x < xTo ? 1 : -1;
	yInc = y < yTo ? 1 : -1;
	dx = xTo > x ? xTo - x : x - xTo;
	dy = yTo > y ? yTo - y : y - yTo; 
	err = dx > dy ? dx/2 : -dy/2;
	while (x != xTo || y != yTo)
	{
		mlx_pixel_put(fdf->mlx, fdf->win, x, y, color);
		e2 = err;
		if (e2 >-dx)
		{
			err -= dy;
			x += xInc;
		}
		if (e2 < dy)
		{
			err += dx;
			y += yInc;
		}
	}
}

void render(t_fdf *fdf)
{
	float	factor;
	float	yfactor;
	int32_t	oldx;
	int32_t	oldy;

	factor = fdf->width;
	yfactor = 10 * factor;
	mlx_clear_window(fdf->mlx, fdf->win);
	oldx = -1;
	oldy = -1;
	for (size_t i = 0; i < fdf->len; i++)
	{
		t_vec3 vec;

		vec.d.x = (i % fdf->width)/factor - 0.5;
		vec.d.y = (i / fdf->width)/factor - 0.5;
		vec.d.z = fdf->arr[i] / yfactor - 0.5;
		vec = mat4_mult_vec3(fdf->mat, vec);
		int32_t x = (int32_t)(vec.d.x * WINDOW_SIZE); 
		int32_t y = (int32_t)(vec.d.y * WINDOW_SIZE); 
		if (i % fdf->width)
			draw_line(fdf, oldx, oldy, x, y, 0x00FF00 * fdf->arr[i + fdf->width] * 10 + 0x0000FF);
		if (i < fdf->len - fdf->width)
		{
			vec.d.x = (i % fdf->width)/factor - 0.5;
			vec.d.y = ((i / fdf->width) + 1)/factor - 0.5;
			vec.d.z = fdf->arr[i + fdf->width] / yfactor - 0.5;
			vec = mat4_mult_vec3(fdf->mat, vec);
			int32_t x2 = (int32_t)(vec.d.x * WINDOW_SIZE); 
			int32_t y2 = (int32_t)(vec.d.y * WINDOW_SIZE); 
			draw_line(fdf, x, y, x2, y2, 0x00FF00 * fdf->arr[i + fdf->width] * 10 + 0x0000FF);
		}
		oldx = x;
		oldy = y;
	}
}

int	key_hook(int keycode, t_fdf *fdf)
{
	if (keycode == X_KEY_NUM_1)
		fdf->mat = mat4_mult(fdf->mat, mat4_rotate_x(M_PI/8));
	else if (keycode == X_KEY_NUM_2)
		fdf->mat = mat4_mult(fdf->mat, mat4_rotate_y(M_PI/8));
	else if (keycode == X_KEY_NUM_3)
		fdf->mat = mat4_mult(fdf->mat, mat4_rotate_z(M_PI/8));
	else if (keycode == X_KEY_PLUS)
		fdf->mat = mat4_mult(fdf->mat, mat4_scale(1.1, 1.1, 1.1));
	else if (keycode == X_KEY_MINUS)
		fdf->mat = mat4_mult(fdf->mat, mat4_scale(0.9, 0.9, 0.9));
	else if (keycode == X_KEY_LEFT)
		fdf->mat = mat4_mult(mat4_translate(-0.1, 0, 0), fdf->mat);
	else if (keycode == X_KEY_RIGHT)
		fdf->mat = mat4_mult(mat4_translate(0.1, 0, 0), fdf->mat);
	else if (keycode == X_KEY_DOWN)
		fdf->mat = mat4_mult(mat4_translate(0, 0.1, 0), fdf->mat);
	else if (keycode == X_KEY_UP)
		fdf->mat = mat4_mult(mat4_translate(0, -0.1, 0), fdf->mat);
	else if (keycode == X_KEY_ESC)
		exit(0);
	render(fdf);
	return (0);
}

size_t	count_words(const char *s, char c)
{
	size_t	len;

	len = !(*s == c);
	while (*++s)
		if (s[-1] == c && *s != c)
			len++;
	return (len);
}

#include <stdlib.h>

int	*parse_file(const int fd, size_t *curr_size, size_t *size)
{
	char	*line;
	char	*line_o;
	size_t	i;
	t_readable	rd;
	int	*arr;

	if (get_next_line(fd, &line) != 1)
		return (NULL);
	line_o = line;
	*size = count_words(line, ' ');
	arr = malloc(*size * sizeof(int));
	i = 0;
	while (i < *size)
		arr[i++] = ft_atoip(&line);
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
			arr[*curr_size + i++] = ft_atoi_rd(&rd);
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

int	main(int argc, char *argv[])
{
	t_fdf		fdf;

	if (argc >= 2)
	{
		if (!(fdf.arr = parse_file(open(argv[1], O_RDONLY), &fdf.len, &fdf.width)))
		{
			ft_putf("Error\n");
			return (1);
		}
	}
	fdf.mlx = mlx_init();
	fdf.win = mlx_new_window(fdf.mlx, WINDOW_SIZE, WINDOW_SIZE, "FdF");
	fdf.mat = mat4_translate(0.5, 0.5, 0.5);
	fdf.mat = mat4_mult(fdf.mat, mat4_rotate_x(M_PI/4));
	fdf.mat = mat4_mult(fdf.mat, mat4_scale(0.8, 0.8, 0.8));
	render(&fdf);
	mlx_hook(fdf.win, X_KEYPRESS, X_KEYPRESSMASK, key_hook, &fdf);
	mlx_loop(fdf.mlx);
}
