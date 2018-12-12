/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/12 08:51:21 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/12 15:40:53 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

# include "ft/math.h"
# include <stdint.h>
# include <stddef.h>
# define WINDOW_SIZE (1000)
# define F_COLOR (0x0000FF)
# define T_COLOR (0xFF0000)

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

typedef struct	s_pixel {
	int32_t		x;
	int32_t		y;
	int32_t		color;
}				t_pixel;

int				*parse_file(const int fd, size_t *curr_size, size_t *size,
		uint32_t *max);

#endif
