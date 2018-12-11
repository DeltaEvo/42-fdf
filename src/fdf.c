/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/06 14:18:46 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/11 17:23:31 by dde-jesu         ###   ########.fr       */
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
	uint32_t	max_height;
	float		yscale;
}				t_fdf;

typedef struct s_pixel {
	int32_t		x;
	int32_t		y;
	int32_t		color;
}			t_pixel;

int	gradient(int32_t colorFrom, int32_t colorTo, double percent)
{
	return (((colorFrom >> 16) + (int)(percent * ((colorTo >> 16)
				- (colorFrom >> 16)))) << 16
			| (((colorFrom >> 8) & 0xFF) + (int)(percent *
				(((colorTo >> 8) & 0xFF) - ((colorFrom >> 8) & 0xFF)))) << 8
			| (((colorFrom) & 0xFF) + (int)(percent * (((colorTo) & 0xFF)
				- ((colorFrom) & 0xFF))))
			);
}

void	draw_line(t_fdf *fdf, t_pixel from, t_pixel to)
{
	int32_t	xInc;
	int32_t	yInc;
	int32_t	dx;
	int32_t	dy;
	int32_t	e[2];

	xInc = from.x < to.x ? 1 : -1;
	yInc = from.y < to.y ? 1 : -1;
	dx = to.x > from.x ? to.x - from.x : from.x - to.x;
	dy = to.y > from.y ? to.y - from.y : from.y - to.y;
	e[0] = dx > dy ? dx/2 : -dy/2;
	while (from.x != to.x || from.y != to.y)
	{
		mlx_pixel_put(fdf->mlx, fdf->win, from.x, from.y,
			gradient(from.color, to.color, 1 - (dx > dy ? (to.x-from.x)
			* xInc/(double)dx : (to.y-from.y) * yInc/(double)dy)));
		if ((e[1] = e[0]) >-dx)
		{
			e[0] -= dy;
			from.x += xInc;
		}
		if (e[1] < dy)
		{
			e[0] += dx;
			from.y += yInc;
		}
	}
}

#define F_COLOR (0x0000FF)
#define T_COLOR (0xFF0000)

void render(t_fdf *fdf)
{
	const float	f = fdf->width;
	const float	yf = fdf->max_height / fdf->yscale;
	t_pixel	old;
	t_pixel	curr;
	int32_t	i;
	t_vec3 vec;

	mlx_clear_window(fdf->mlx, fdf->win);
	i = -1;
	while (++i < fdf->len)
	{
		vec = mat4_mult_vec3(fdf->mat, (t_vec3)(t_vec3_data){ (i % fdf->width)/f - 0.5, (i / fdf->width)/f - 0.5, fdf->arr[i] / yf - 0.5 });
		curr = (t_pixel) { vec.d.x * WINDOW_SIZE, vec.d.y * WINDOW_SIZE,
			gradient(F_COLOR, T_COLOR, fdf->arr[i] / (float)fdf->max_height) };
		if (i % fdf->width)
			draw_line(fdf, old, curr);
		old = curr;
		if (i < fdf->len - fdf->width)
		{
			vec = mat4_mult_vec3(fdf->mat, (t_vec3)(t_vec3_data) { (i % fdf->width)/f - 0.5,
				((i / fdf->width) + 1)/f - 0.5, fdf->arr[i + fdf->width] / yf - 0.5 });
			draw_line(fdf, old, curr = (t_pixel) { vec.d.x * WINDOW_SIZE, vec.d.y * WINDOW_SIZE,
				gradient(F_COLOR, T_COLOR, fdf->arr[i + fdf->width] / (float)fdf->max_height) });
		}
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
	else if (keycode == X_KEY_UP)
		fdf->mat = mat4_mult(mat4_translate(0, 0.1, 0), fdf->mat);
	else if (keycode == X_KEY_DOWN)
		fdf->mat = mat4_mult(mat4_translate(0, -0.1, 0), fdf->mat);
	else if (keycode == X_KEY_A)
		fdf->yscale -= 0.3;
	else if (keycode == X_KEY_S)
		fdf->yscale += 0.3;
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

uint32_t ft_abs(int32_t a)
{
	return (a > 0 ? a : -a);
}

int	*parse_file(const int fd, size_t *curr_size, size_t *size, uint32_t *max_height)
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

int	main(int argc, char *argv[])
{
	t_fdf		fdf;

	fdf.max_height = 0;
	fdf.yscale = 1;
	if (argc != 2 || !(fdf.arr = parse_file(open(argv[1], O_RDONLY), &fdf.len, &fdf.width, &fdf.max_height)))
	{
		ft_putf("Error\n");
		return (1);
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
