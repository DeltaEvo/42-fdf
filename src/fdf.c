/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/06 14:18:46 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/06 16:34:28 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mlx.h>
#include <stddef.h>
#include <stdint.h>
#include "vertexdata.h"
#include <string.h>
#include <math.h>
#include "ft/io.h"
#include "ft/mem.h"
#include "ft/str.h"
#include "get_next_line.h"

typedef	struct	s_mat4_data {
	float	m11;
	float	m12;
	float	m13;
	float	m14;
	float	m21;
	float	m22;
	float	m23;
	float	m24;
	float	m31;
	float	m32;
	float	m33;
	float	m34;
	float	m41;
	float	m42;
	float	m43;
	float	m44;
}				t_mat4_data;

typedef	union	u_mat4 {
	float		a[4][4];
	t_mat4_data	d;
}				t_mat4;

typedef	struct	s_vec3 {
	float	d[3];
}				t_vec3;

t_mat4	mat4_identity() {
	return ((t_mat4)((t_mat4_data) {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	}));
}

t_mat4	mat4_scale(float scaleX, float scaleY, float scaleZ) {
	return ((t_mat4)((t_mat4_data) {
		scaleX, 0, 0, 0,
		0, scaleY, 0, 0,
		0, 0, scaleZ, 0,
		0, 0, 0, 1
	}));
}

t_mat4	mat4_translation(float x, float y, float z) {
	return ((t_mat4)((t_mat4_data) {
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	}));
}

t_mat4	mat4_rotation_x(float angle) {
	return ((t_mat4)((t_mat4_data) {
		1, 0, 0, 0,
		0, cos(angle), -sin(angle), 0,
		0, sin(angle), cos(angle), 0,
		0, 0, 0, 1
	}));
}

t_vec3	mat4_mult_vec3(t_vec3 src, t_mat4 m)
{
	t_vec3	res;
	float	x;
	float	y;
	float	z;
	float	w;
	uint8_t	i;

	x = src.d[0];
	y = src.d[1];
	z = src.d[2];
	w = x * m.a[3][0] + y * m.a[3][1] + z * m.a[3][2] + m.a[3][3]; 
	i = 0;
	while (i < 3)
	{
		res.d[i] = (x * m.a[i][0] + y * m.a[i][1] + z * m.a[i][2] + m.a[i][3])/w; 
		i++;
	}
	return (res);
}

t_mat4	mat4_projection(float fov, float near, float far)
{
	float	scale;
	float 	z_map1;
	float 	z_map2;
   
	scale = 1 / tan(fov * 0.5 * M_PI / 180); 
	z_map1 = -far/(far - near);
	z_map2 = -far * near/(far - near);

	return ((t_mat4)(t_mat4_data) {
		scale, 0    , 0    , 0,
		0    , scale, 0, 0,
		0    , 0    , z_map1, z_map2,
		0    , 0    , -1 , 0
	});
}

#define SQRT36 (0.70710678118)
#define SQRT26 (0.57735026919)

t_mat4	mat4_isometric()
{
	return ((t_mat4)(t_mat4_data) {
		SQRT36, 0, 0, -SQRT36,
		1     , 2, 1, 0,
		SQRT26, -SQRT26, SQRT26, 0,
		0    , 0    , -1 , 0
	});
}

uint32_t	min(uint32_t u1, uint32_t u2)
{
	return (u1 > u2 ? u2 : u1);
}

typedef	struct	s_fdf {
	void		*mlx;
	void		*win;
	t_mat4		projection;
	t_mat4		camera;
	float		scale;
	float		x;
	float		y;
	float		z;
	int		*arr;
	size_t		len;
	size_t		width;
}				t_fdf;

#include <stdio.h>

void render(t_fdf *fdf)
{
	uint32_t	imageSize = 500;
	t_mat4		scale;
	t_mat4		rotation;
	t_mat4		camera;

	mlx_clear_window(fdf->mlx, fdf->win);
	scale = mat4_scale(fdf->scale, fdf->scale, fdf->scale);
	camera = mat4_translation(fdf->x, fdf->y, fdf->z);
	rotation = mat4_rotation_x(M_PI/2);
	for (size_t i = 0; i < fdf->len; i++)
	{
		t_vec3 vec;

		vec.d[0] = i % fdf->width;
		vec.d[1] = (float)fdf->arr[i] / (float)10;
		vec.d[2] = i / fdf->width;
		vec = mat4_mult_vec3(vec, scale);
		vec = mat4_mult_vec3(vec, rotation);
		vec = mat4_mult_vec3(vec, camera);
		vec = mat4_mult_vec3(vec, fdf->projection);
		uint32_t x = (uint32_t)((vec.d[0] + 1) * 0.5 * imageSize); 
		uint32_t y = (uint32_t)((1 - (vec.d[1] + 1) * 0.5) * imageSize); 
		if (x < imageSize && y < imageSize)
			mlx_pixel_put(fdf->mlx, fdf->win, x, y, (0x00FF00 * fdf->arr[i] * 10) + 0x0000FF);
	}
}

int	key_hook(int keycode, t_fdf *fdf)
{
	ft_putf("Key: %d\n", keycode);
	if (keycode == 69)
		fdf->scale += 0.1;
	if (keycode == 78)
		fdf->scale -= 0.1;
	if (keycode == 123)
		fdf->x -= 1;
	if (keycode == 124)
		fdf->x += 1;
	if (keycode == 125)
		fdf->y -= 1;
	if (keycode == 126)
		fdf->y += 1;
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
	rd = init_readable(fill_fd, (void *)fd);
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
		if (io_peek(&rd) != '\n')
		{
			free(arr);
			return (NULL);
		}
		else
			rd.index++;
		*curr_size += *size;
	}
	return (arr);
}

#include <fcntl.h>

int	main(int argc, char *argv[])
{
	t_fdf		fdf;
	uint32_t	imageSize = 500;
	int		*arr;

	if (argc >= 2)
	{
		if (!(fdf.arr = parse_file(open(argv[1], O_RDONLY), &fdf.len, &fdf.width)))
		{
			ft_putf("Error\n");
			return (1);
		}
	}
	fdf.mlx = mlx_init();
	fdf.win = mlx_new_window(fdf.mlx, imageSize, imageSize, "Hello World");

	//fdf.projection = mat4_projection(120, 0.1, 100);
	//fdf.x = -20;
	//fdf.y = -10;
	//fdf.z = -30;
	//fdf.scale = 6;
	fdf.projection = mat4_isometric();
	fdf.x = 0;
	fdf.y = 0;
	fdf.z = 0;
	fdf.scale = 4;

	render(&fdf);

	mlx_hook(fdf.win, 2, 1, key_hook, &fdf);
	mlx_loop(fdf.mlx);
}
