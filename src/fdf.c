/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/06 14:18:46 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/12 09:39:05 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"
#include "ft/mlx.h"
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <fcntl.h>
#include "ft/math.h"

int		gradient(int32_t color_from, int32_t color_to, double percent)
{
	return (((color_from >> 16) + (int)(percent * ((color_to >> 16)
				- (color_from >> 16)))) << 16
			| (((color_from >> 8) & 0xFF) + (int)(percent *
				(((color_to >> 8) & 0xFF) - ((color_from >> 8) & 0xFF)))) << 8
			| (((color_from) & 0xFF) + (int)(percent * (((color_to) & 0xFF)
				- ((color_from) & 0xFF)))));
}

int		draw_line(t_fdf *fdf, t_pixel from, t_pixel to)
{
	const int32_t	x_inc = from.x < to.x ? 1 : -1;
	const int32_t	y_inc = from.y < to.y ? 1 : -1;
	const int32_t	dx = to.x > from.x ? to.x - from.x : from.x - to.x;
	const int32_t	dy = to.y > from.y ? to.y - from.y : from.y - to.y;
	int32_t			e[2];

	e[0] = dx > dy ? dx / 2 : -dy / 2;
	while (from.x != to.x || from.y != to.y)
	{
		mlx_pixel_put(fdf->mlx, fdf->win, from.x, from.y,
		gradient(from.color, to.color, 1 - (dx > dy ? (to.x - from.x)
		* x_inc / (double)dx : (to.y - from.y) * y_inc / (double)dy)));
		if ((e[1] = e[0]) > -dx)
		{
			e[0] -= dy;
			from.x += x_inc;
		}
		if (e[1] < dy)
		{
			e[0] += dx;
			from.y += y_inc;
		}
	}
	return (0);
}

void	render(t_fdf *fdf)
{
	const float	f = fdf->width;
	const float	yf = fdf->max_height / fdf->yscale;
	uint32_t	i;
	t_pixel		p[2];
	t_vec3		vec;

	mlx_clear_window(fdf->mlx + (i = 0), fdf->win);
	while (i < fdf->len)
	{
		vec = mat4_mult_vec3(fdf->mat, (t_vec3)(t_vec3_data) { (i % fdf->width)
			/ f - 0.5, (i / fdf->width) / f - 0.5, fdf->arr[i] / yf - 0.5 });
		p[0] = (t_pixel) { vec.d.x * WINDOW_SIZE, vec.d.y * WINDOW_SIZE,
			gradient(F_COLOR, T_COLOR, fdf->arr[i] / (float)fdf->max_height) };
		((i % fdf->width ? draw_line(fdf, p[1], p[0]) : 0) || (p[1] = p[0]).x);
		if (i < fdf->len - fdf->width)
		{
			vec = mat4_mult_vec3(fdf->mat, (t_vec3)(t_vec3_data) { (i
				% fdf->width) / f - 0.5, ((i / fdf->width) + 1) / f - 0.5,
					fdf->arr[i + fdf->width] / yf - 0.5 });
			draw_line(fdf, p[1], p[0] = (t_pixel) { vec.d.x * WINDOW_SIZE,
				vec.d.y * WINDOW_SIZE, gradient(F_COLOR, T_COLOR,
						fdf->arr[i + fdf->width] / (float)fdf->max_height) });
		}
		i++;
	}
}

int		key_hook(int keycode, t_fdf *fdf)
{
	if (keycode == X_KEY_NUM_1)
		fdf->mat = mat4_mult(fdf->mat, mat4_rotate_x(M_PI / 8));
	else if (keycode == X_KEY_NUM_2)
		fdf->mat = mat4_mult(fdf->mat, mat4_rotate_y(M_PI / 8));
	else if (keycode == X_KEY_NUM_3)
		fdf->mat = mat4_mult(fdf->mat, mat4_rotate_z(M_PI / 8));
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
	else if (keycode == X_KEY_A || keycode == X_KEY_S)
		fdf->yscale -= keycode == X_KEY_A ? 0.3 : -0.3;
	else if (keycode == X_KEY_ESC)
		exit(mlx_destroy_window(fdf->mlx, fdf->win) & 0);
	render(fdf);
	return (0);
}

int		main(int argc, char *argv[])
{
	t_fdf		fdf;

	fdf.max_height = 0;
	fdf.yscale = 1;
	if (argc != 2 || !(fdf.arr = parse_file(open(argv[1], O_RDONLY), &fdf.len,
					&fdf.width, &fdf.max_height)))
	{
		write(2, "Error\n", 6);
		return (1);
	}
	fdf.mlx = mlx_init();
	if (!(fdf.win = mlx_new_window(fdf.mlx, WINDOW_SIZE, WINDOW_SIZE, "FdF")))
	{
		write(2, "Failed to create window\n", 24);
		return (1);
	}
	fdf.mat = mat4_translate(0.5, 0.5, 0.5);
	fdf.mat = mat4_mult(fdf.mat, mat4_rotate_x(M_PI / 4));
	fdf.mat = mat4_mult(fdf.mat, mat4_scale(0.8, 0.8, 0.8));
	mlx_hook(fdf.win, X_KEYPRESS, X_KEYPRESSMASK, key_hook, &fdf);
	mlx_expose_hook(fdf.win, (int (*)(t_fdf *))render, &fdf);
	mlx_loop(fdf.mlx);
}
