/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   concerning_light.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarcia- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/10 13:06:14 by mgarcia-          #+#    #+#             */
/*   Updated: 2020/03/12 20:01:04 by mgarcia-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

void				add_coeficient(double (*rgb)[3], double coef, int color)
{
	unsigned int	mask;

	mask = 255 << 16;
	(*rgb)[0] += coef * ((color & mask) >> 16) / 255;
	mask >>= 8;
	(*rgb)[1] += coef * ((color & mask) >> 8) / 255;
	mask >>= 8;
	(*rgb)[2] += coef * (color & mask) / 255;
}

t_p3		reflect_ray(t_p3 ray, t_p3 normal)
{
    return (vsubstract(scal_x_vec(2 * dot(normal, ray), normal), ray));
}

void				compute_light(t_p3 o, int *color, t_p3 p, t_p3 normal, t_scene data, t_figures *lst, double (*fun_ptr[NUM_FIGS])(t_p3, t_p3, t_figures *))
{
	double			light;
	double			rgb[3];
	t_p3			direction;
	t_p3			p_to_cam;
	t_p3			reflected;

	light = 0.0;
	rgb[0] = 0.0;
	rgb[1] = 0.0;
	rgb[2] = 0.0;
	add_coeficient(&rgb, data.ambient_light, data.al_color);
	while (data.l)
	{
		direction = vsubstract(data.l->o, p);
		if (is_lit(p, direction, lst, fun_ptr) && dot(normal, direction) > 0)
		{
			light = data.l->br * vcos(normal, direction);
			add_coeficient(&rgb, light, data.l->color);
		}
		if (lst->specular)
		{
			p_to_cam = vsubstract(o, p);
			//reflected = reflect_ray(direction, normal);
			reflected = vsubstract(scal_x_vec(2 * dot(normal, direction), normal), direction);
			if (dot(reflected, p_to_cam) > 0)
			{
				light = data.l->br * pow(vcos(reflected, p_to_cam), lst->specular);
				add_coeficient(&rgb, light, data.l->color);
			}
		}	
		data.l = data.l->next;
	}
	*color = color_x_light(*color, rgb);
}

void				calc_normal(t_p3 p, t_p3 d, t_p3 *normal, t_figures lst)
{
	if ((lst.flag == PL) || (lst.flag == SQ) || (lst.flag == TR) || (lst.flag == CY))
		*normal = vcos(d, lst.normal) > 0 ? scal_x_vec(-1, lst.normal) : lst.normal;
		//*normal = lst.normal;
	else if (lst.flag == SP)
		*normal = normalize(vsubstract(p, lst.fig.sp.c));
	//else if (lst.flag == CY)
	//	*normal = lst.normal;
	//else if (lst.flag == SQ)
	//	*normal = lst.normal;
}

int					is_lit(t_p3 o, t_p3 d, t_figures *lst, double (*fun_ptr[NUM_FIGS])(t_p3, t_p3, t_figures *))
{
	double			in;

	while (lst)
	{
		in = (fun_ptr[lst->flag])(o, d, lst);
		if (in > 0.0001 && in < 1)
			return (0);
		lst = lst->next;
	}
	return (1);
}

int					color_x_light(int color, double rgb[3])
{
	unsigned int	mask;
	unsigned int	r;
	unsigned int	g;
	unsigned int	b;

	mask = 255 << 16;
	r = rgb[0] * ((color & mask) >> 16);
	mask >>= 8;
	g = rgb[1] * ((color & mask) >> 8);
	mask >>= 8;
	b = rgb[2] * (color & mask);
	r = r > 255 ? 255 : r;
	g = g > 255 ? 255 : g;
	b = b > 255 ? 255 : b;
	return ((r << 16) | (g << 8) | b);
}