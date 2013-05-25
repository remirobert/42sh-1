/*
** gere_keyboard.c for gere_èkeyboard in /home/remi/Dropbox/Projet/edition_ligne_termcap
**
** Made by remi robert
** Login   <robert_r@epitech.net>
**
** Started on  Sun May  5 16:20:31 2013 remi robert
** Last update Fri May 24 21:53:03 2013 Adrien Della Maggiora
*/

#include "42sh.h"

void	gere_posright(char *cmd, t_param *param)
{
  if (param->pos + 1 <= my_strlen(cmd))
    {
      if (param->x + 1 >= return_x())
	{
	  apply_termcap(param->termcap.str_do, 0, param->fd_tty);
	  param->x = 0;
	  param->y += 1;
	}
      else
	{
	  apply_termcap(param->termcap.str_ri, 0, param->fd_tty);
	  param->x += 1;
	}
      param->pos += 1;
    }
}

void	gere_posleft(char *cmd, t_param *param)
{
  if (param->pos - 1 >= 0)
    {
      if (param->x <= 0)
	{
	  apply_termcap(param->termcap.str_up, 0, param->fd_tty);
	  param->x = return_x() - 1;
	  param->y -= 1;
	  curseur(param->x, param->y);
	}
      else
	{
	  apply_termcap(param->termcap.str_le, 0, param->fd_tty);
	  param->x -= 1;
	}
      param->pos -= 1;
    }
}

int	gere_keyboard(char *buff, char *cmd, t_param *param,
		      t_history **history)
{
  if (buff[0] == END)
    return (1);
  if (str_cmp(buff, STR_RIGHT) == 1)
    gere_posright(cmd, param);
  else if (str_cmp(buff, STR_LEFT) == 1)
    gere_posleft(cmd, param);
  else if (history != NULL && (str_cmp(buff, STR_UP) == 1 ||
			       str_cmp(buff, STR_DOWN) == 1))
    gere_history(cmd, param, *history, buff);
  else if ((buff[0] == DEL && buff[1] == END) ||
	   (buff[0] == ESC && buff[1] == CRO && buff[2] == SUPPR
	    && buff[3] == END))
    gere_delete(cmd, param, buff);
  else
    return (gere_control(cmd, param, buff));
  return (0);
}
