/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_frame_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmarash <pmarash@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/09 15:59:18 by chudapak          #+#    #+#             */
/*   Updated: 2021/03/04 22:34:10 by pmarash          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/overall.h"

t_pl	get_ray_info(t_all *all)
{
	//all->player.start_agl = all->player.dir - VIEV_ANGLE / 2;
	all->player.colom = -1;
	if (all->player.dir > 2 * M_PI)
		all->player.dir -= 2 * M_PI;
	else if (all->player.dir < 0)
		all->player.dir += 2 * M_PI;
	return (all->player);
}

void	way_pl_looking(t_pl *ray)
{
	ray->cos_i = cos(ray->start_agl);
	ray->sin_j = sin(ray->start_agl);
	ray->step_len = 0;
	if (ray->cos_i < 0)
		ray->step_i = 1;
	else
		ray->step_i = 0;
	if (ray->sin_j < 0)
		ray->step_j = 1;
	else
		ray->step_j = 0;
}

void	get_final_coordinate(t_all *all, t_pl *ray)
{
	double	len_i;
	double	len_j;
	double	one_i;
	double	one_j;
	double	ratio;

	len_i = ray->cos_i * ray->step_len - ray->cos_i * (ray->step_len - STEP_TO_WALL);
	len_j = ray->sin_j * ray->step_len - ray->sin_j * (ray->step_len - STEP_TO_WALL);
	if ((int)ray->i != (int)(ray->i - len_i))
	{
		ratio = fabs(((int)ray->i - all->player.i + ray->step_i) / ray->i - all->player.i - len_i);
		ray->j /= ratio;
		ray->i = (int)ray->i + ray->step_i;
	}
	else if ((int)ray->j != (int)(ray->j - len_j))
	{
		ray->i /= fabs(((int)ray->j - all->player.j + ray->step_j) / ray->j - all->player.j - len_j);
		ray->j = (int)ray->j + ray->step_j;
	}
	else
	{
		ratio = fabs(((int)ray->i - all->player.i + ray->step_i) / ray->i - all->player.i - len_i);
		one_j = ray->j / ratio;
		ratio = fabs(((int)ray->j - all->player.j + ray->step_j) / ray->j - all->player.j - len_j);
		one_i = ray->i / ratio;
		if (one_i < one_j)
		{
			ray->j /= fabs(((int)ray->i - all->player.i + ray->step_i) / ray->i - all->player.i - len_i);
			ray->i = (int)ray->i + ray->step_i;
		}
		else
		{
			ray->i /= fabs(((int)ray->j - all->player.j + ray->step_j) / ray->j - all->player.j - len_j);
			ray->j = (int)ray->j + ray->step_j;
		}
	}
}

void 	get_ray_len(t_all *all, t_pl *ray)
{
	way_pl_looking(ray);
	while (all->parsed.map[(int)ray->i][(int)ray->j] != '1'
				&& all->parsed.map[(int)ray->i][(int)ray->j] != '2'
				&& all->parsed.map[(int)ray->i][(int)ray->j] != ' '
				&& all->parsed.map[(int)ray->i][(int)ray->j] != '\0')
	{
		ray->step_len += STEP_TO_WALL;
		ray->i += all->player.i + ray->cos_i * ray->step_len;
		ray->j += all->player.j + ray->sin_j * ray->step_len;
	}
	get_final_coordinate(all, ray);
	ray->ray_len = sqrt(pow(ray->i - all->player.i, 2) + pow(ray->j - all->player.j, 2));
}

int		draw_new_frame(t_all *all)
{
	t_pl	ray;
 
	ray = get_ray_info(all);
	while (++ray.colom < (int)all->parsed.res.width)
	{
		ray.dir = 0;
		ray.start_agl = (ray.dir - VIEV_ANGLE / 2) + ((float)ray.colom / (float)all->parsed.res.width) * VIEV_ANGLE;
		get_ray_len(all, &ray);
		ray.ceiling = (float)(all->parsed.res.height / 2) - all->parsed.res.height / (ray.ray_len);
		ray.floor = all->parsed.res.height - ray.ceiling;
		ray.string = -1;
		while (++ray.string < (int)all->parsed.res.height)
		{
			if (ray.string < ray.ceiling)
				pixel_put(&all->img, ray.string, ray.colom, CEILING);
			else if (ray.string >= ray.ceiling && ray.string <= ray.floor)
				pixel_put(&all->img, ray.string, ray.colom, WALL);
			else
				pixel_put(&all->img, ray.string, ray.colom, FLOOR);
		}
	}
	mlx_put_image_to_window(all->vars.mlx, all->vars.win, all->img.img, 0, 0);
	return (0);
}

#include "../headers/overall.h"

void	draw_wall(t_all *all, t_pl *ray)
{
	if (ray->ray_hit_wall == 'N')
	{
		pixel_put(&all->img, ray->string, ray->colom, WALL_N);
	}
	else if (ray->ray_hit_wall == 'S')
		pixel_put(&all->img, ray->string, ray->colom, WALL_S);
	else if (ray->ray_hit_wall == 'W')
		pixel_put(&all->img, ray->string, ray->colom, WALL_W);
	else if (ray->ray_hit_wall == 'E')
		pixel_put(&all->img, ray->string, ray->colom, WALL_E);
}

static int	get_nb_texture(t_pl *ray)
{
	if (ray->ray_hit_wall == 'N')
		return (0);
	else if (ray->ray_hit_wall == 'S')
		return (1);
	else if (ray->ray_hit_wall == 'W')
		return (2);
	else if (ray->ray_hit_wall == 'E')
		return (3);
	return (-1);
}

void		fill_picture(t_all *all, t_pl *ray)
{
	ray->ceiling = (int)((float)(all->parsed.res.height / 2) - all->parsed.res.height / (ray->ray_len * 3));
	ray->floor = all->parsed.res.height - ray->ceiling;
	ray->wallH = ray->floor - ray->ceiling;
	if ((ray->nb_tex = get_nb_texture(ray)) == -1)
		exit(0);
	ray->step_of_height = all->texture[ray->nb_tex].tH / ray->wallH;
	ray->ray_of_tex = 0;
	ray->colom_of_tex = (int)fabs((ray->ray_cross_j - (int)ray->ray_cross_j) * all->texture[ray->nb_tex].tW);
	ray->string = -1;
	while (++ray->string < (int)all->parsed.res.height)
	{
		if (ray->string < ray->ceiling)
			pixel_put(&all->img, ray->string, ray->colom, all->color.clg);
		else if (ray->string >= ray->ceiling && ray->string <= ray->floor)
		{
			//draw_wall(all, ray);
			pixel_put(&all->img, ray->string, ray->colom, *(unsigned int*)(all->texture[ray->nb_tex].ptr + 
															((int)ray->ray_of_tex * all->texture[ray->nb_tex].size_line
															+ (ray->colom_of_tex * (all->texture[ray->nb_tex].bpp / 8)))));
			ray->ray_of_tex += ray->step_of_height;
		}
		else
			pixel_put(&all->img, ray->string, ray->colom, all->color.flr);
	}
}


void	draw_wall_no_tex(t_all *all, t_pl *ray)
{
	if (ray->ray_hit_wall == 'N')
	{
		pixel_put(&all->img, ray->string, ray->colom, WALL_N);
	}
	else if (ray->ray_hit_wall == 'S')
		pixel_put(&all->img, ray->string, ray->colom, WALL_S);
	else if (ray->ray_hit_wall == 'W')
		pixel_put(&all->img, ray->string, ray->colom, WALL_W);
	else if (ray->ray_hit_wall == 'E')
		pixel_put(&all->img, ray->string, ray->colom, WALL_E);
}

static void	mv_left_right(t_all *all, int way_direction)
{
	double	step_i;
	double	step_j;

	step_i = STEP_L_R * cos(all->player.dir) * way_direction;
	step_j = STEP_L_R * sin(all->player.dir) * way_direction;
	//all->player.i += step_i;
	//all->player.j += step_j;
	if (//all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] == '0'
			(all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] == '1'
			|| all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != '2'
			|| all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != ' '
			|| all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != '\0')
			&& ((int)(all->player.i + step_i) > 0 && (int)(all->player.i + step_i) < all->parsed.p_coord.rows))
		all->player.i += step_i;
	if  (//all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] == '0'
			(all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] == '1'
			|| all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] != '2'
			|| all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] != ' '
			|| all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] != '\0')
			&& ((int)(all->player.j + step_j) > 0 && (int)(all->player.j + step_j) < all->parsed.p_coord.coloms))
		all->player.j += step_j;
}

static void	mv_foward_back(t_all *all, int way_direction)
{
	double	step_i;
	double	step_j;

	step_i = STEP_TO_WALL * (-sin(all->player.dir)) * way_direction;
	step_j = STEP_TO_WALL * cos(all->player.dir) * way_direction;
	//all->player.i += step_i;
	//all->player.j += step_j;
	if (//all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] == '0'
			(all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != '1'
			|| all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != '2'
			|| all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != ' '
			|| all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != '\0')
			&& ((int)(all->player.i + step_i) > 0 && (int)(all->player.i + step_i) < all->parsed.p_coord.rows))
		all->player.i += step_i;
	if  (//all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] == '0'
			(all->parsed.map[(int)(all->player.i + step_i)][(int)(all->player.j)] != '1'
			|| all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] != '2'
			|| all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] != ' '
			|| all->parsed.map[(int)(all->player.i)][(int)(all->player.j + step_j)] != '\0')
			&& ((int)(all->player.j + step_j) > 0 && (int)(all->player.j + step_j) < all->parsed.p_coord.coloms))
		all->player.j += step_j;
}
+ STOP_BF_WALL * way_direction

static int	get_dist_seen_spr(t_all *all, t_pl *ray, t_spr **visible_spr)
{
	int		i;
	int		j;
	//double	at1;
	//double	at2;

	i = -1;
	j = 0;
	while (++i < all->parsed.amt_spr)
	{
		/*all->sprite[i].vec_i = all->sprite[i].i * ray->i + all->sprite[i].j * ray->j;
		all->sprite[i].vec_j = sqrt(all->sprite[i].i * all->sprite[i].i + all->sprite[i].j * all->sprite[i].j)
								+ sqrt(ray->i * ray->i + ray->j * ray->j);
		all->sprite[i].ang_btw_pl_sp = fabs(acos(all->sprite[i].vec_i / all->sprite[i].vec_j));*/
	//	printf("i - %f, j - %f\n", all->sprite[i].i, all->sprite[i].j);
		all->sprite[i].vec_i = all->sprite[i].i - ray->i;
		all->sprite[i].vec_j = all->sprite[i].j - ray->j;
		//all->sprite[i].eye_i = cos(ray->dir);
		//all->sprite[i].eye_j = sin(ray->dir);
		all->sprite[i].sprite_angle = atan2(all->sprite[i].vec_i, all->sprite[i].vec_j);
		//at2 = atan2(all->sprite[i].eye_i, all->sprite[i].eye_j);
		//all->sprite[i].sprite_angle = at1 - at2;
		if (all->sprite[i].sprite_angle < 0)
			all->sprite[i].sprite_angle *= -1;
		else if (all->sprite[i].sprite_angle >= 0)
			all->sprite[i].sprite_angle = 2.0 * M_PI - all->sprite[i].sprite_angle;
	//	printf("sprite_agl - %f\n", all->sprite[i].sprite_angle);
		all->sprite[i].ang_btw_pl_sp = (all->sprite[i].sprite_angle - ray->dir) * -1;
	//	printf("agl all - %f\n", all->sprite[i].ang_btw_pl_sp);
		if (fabs(all->sprite[i].ang_btw_pl_sp) < VIEV_ANGLE / 2.0)
		{
			visible_spr[j]->ang_btw_pl_sp = all->sprite[i].ang_btw_pl_sp;
			visible_spr[j]->len_till_pl = sqrt(all->sprite[i].vec_i * all->sprite[i].vec_i
												+ all->sprite[i].vec_j * all->sprite[i].vec_j);
			//printf("agl - %f, len - %f\n", visible_spr[j]->ang_btw_pl_sp, visible_spr[j]->len_till_pl);
			j++;
			
		}
	//	printf("\n\n");
	}
	return (j);
	
	
static void			paint_sprite(t_all *all, t_spr *visible_spr, int i, float *rays)
{
	t_spr_limit	data;

	if (fabs(visible_spr[i].ang_btw_pl_sp) < (VIEV_ANGLE + 0.15) / 2.0)
	{
		data = calculate_sprite_parametres(all, visible_spr, i);
		data.img_colom = (int)(data.mdl_spr + data.colom_tex - (data.spr_width / 2.0));
		if (data.img_colom < 0)
		{
			data.img_colom = 0;
			data.colom_tex = -1;
		}
		while (++data.colom_tex < data.spr_width)
		{
			data.row_tex = -1;
			if (data.img_colom >= 0 && data.img_colom < (int)all->parsed.res.width)
			{
				printf("%d\n", data.img_colom);
				if (rays[data.img_colom] >= visible_spr[i].len_till_pl)
					while (++data.row_tex < data.spr_height)
						if (check_for_empty_pixel(all, data))
							draw_sprite_colom(all, data);
				data.img_colom++;
			}
		}
	}
}


all: start_game

start_game: compile_all

compile_all: $(NAME) $(LIBFT)
	$(CC) $(FLAGS) -o $(NAME) $(LIBFT) $(MLX) $(UTILS) $(BASE) $(PARSER)

2dwindow: compile_window

$(NAME):
	$(MAKE) -C minilibx_mms_20200219; \
	cp minilibx_mms_20200219/libmlx.dylib libmlx.dylib;

$(LIBFT):
	cd libft; \
	make; \

compile_window: $(NAME) $(LIBFT)
	$(CC) $(FLAGS) -o $(NAME) $(LIBFT) $(MLX) $(UTILS) $(CUB2D) $(PARSER)

clean_libft:
	cd libft; \
	make fclean;

clean_all: clean_libft
	cd minilibx_mms_20200219; \
	make clean; \
	rm -rf $(NAME)