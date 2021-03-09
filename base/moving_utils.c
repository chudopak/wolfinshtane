/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moving_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmarash <pmarash@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/08 20:42:08 by pmarash           #+#    #+#             */
/*   Updated: 2021/03/08 20:42:36 by pmarash          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/overall.h"

int		check_barier(t_all *all, int i, int j)
{
	if (all->parsed.map[i][j] != '0' && all->parsed.map[i][j] != '.'
			&& all->parsed.map[i][j] != '2')
		return (0);
	return (1);
}

void	check_near_wall(t_all *all)
{
	if (!check_barier(all, (int)(all->player.i - STOP_BF_WALL / 1.01),
			(int)all->player.j))
		all->player.i = (int)(all->player.i - STOP_BF_WALL / 1.01 + 1)
				+ STOP_BF_WALL / 1.01;
	if (!check_barier(all, (int)(all->player.i + STOP_BF_WALL / 1.01),
			(int)all->player.j))
		all->player.i = (int)(all->player.i + STOP_BF_WALL / 1.01)
				- STOP_BF_WALL / 1.01;
	if (!check_barier(all, (int)all->player.i,
			(int)(all->player.j - STOP_BF_WALL / 1.01)))
		all->player.j = (int)(all->player.j - STOP_BF_WALL / 1.01 + 1)
				+ STOP_BF_WALL / 1.01;
	if (!check_barier(all, (int)all->player.i,
			(int)(all->player.j + STOP_BF_WALL / 1.01)))
		all->player.j = (int)(all->player.j + STOP_BF_WALL / 1.01)
				- STOP_BF_WALL / 1.01;
}

void	axis_i_movement(t_all *all)
{
	float	limit_i;

	if (all->player.move.step_i < 0)
	{
		limit_i = -STOP_BF_WALL / 1.01;
		if (!check_barier(all, (int)(all->player.i + limit_i),
				(int)(all->player.j)))
			all->player.i = (int)(all->player.i + limit_i + 1) - limit_i;
	}
	else if (all->player.move.step_i > 0)
	{
		limit_i = STOP_BF_WALL / 1.01;
		if (!check_barier(all, (int)(all->player.i + limit_i),
				(int)(all->player.j)))
			all->player.i = (int)(all->player.i + limit_i) - limit_i;
	}
}

void	axis_j_movement(t_all *all)
{
	float	limit_j;

	if (all->player.move.step_j < 0)
	{
		limit_j = -STOP_BF_WALL / 1.01;
		if (!check_barier(all, (int)(all->player.i),
				(int)(all->player.j + limit_j)))
			all->player.j = (int)(all->player.j + limit_j + 1) - limit_j;
	}
	else if (all->player.move.step_j > 0)
	{
		limit_j = STOP_BF_WALL / 1.01;
		if (!check_barier(all, (int)(all->player.i),
				(int)(all->player.j + limit_j)))
			all->player.j = (int)(all->player.j + limit_j) - limit_j;
	}
}
