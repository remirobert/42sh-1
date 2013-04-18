/*
** FILE for FILE in /home/moriss_h/projet
**
** Made by hugues morisset
** Login   <moriss_h@epitech.net>
**
** Started on  Mon Oct  8 09:34:29 2012 hugues morisset
** Last update Wed Apr 10 10:26:02 2013 remi robert
*/

#include "42sh.h"

void	aff_special_return_val(t_cmd *cmd)
{
  char	*tmp;
  int	signal;

  if (WIFSIGNALED(cmd->return_value))
    {
      signal = WTERMSIG(cmd->return_value);
      my_putstr(cmd->argv[0], 2, -1);
      my_putstr(" terminated by signal: 0x", 2, -1);
      tmp = my_uint_strbase(signal, "0123456789ABCDEF");
      my_putstr(tmp, 2, -1);
      free(tmp);
      my_putstr("\n", 2, -1);
      if (signal == 0xB)
        my_putstr("Segmentation fault\n", 2, -1);
    }
  if (WIFEXITED(cmd->return_value))
    {
      cmd->return_value = WEXITSTATUS(cmd->return_value);
      cmd->pid.pid = -1 ;
    }
}

t_cmd	*cmd_f_pid(int pid, t_sh *shell)
{
  int	i;
  int	j;
  t_grp	*tmpgrp;
  t_cmd	*tmpcmd;

  i = 0;
  if ((shell == NULL) || (shell->process_group == NULL) || (pid == -1))
    return (NULL);
  while ((tmpgrp = shell->process_group[i]) != NULL)
    {
      if (tmpgrp->cmds != NULL)
        {
          j = 0;
          while ((tmpcmd = tmpgrp->cmds[j]) != NULL)
            {
              if (tmpcmd->pid.pid == pid)
                return (tmpcmd);
              j++;
            }
        }
      i++;
    }
  return (NULL);
}

int	wait_son(t_grp *grp)
{
  int	i;
  t_cmd	*cmd;
  int	out;

  i = 0;
  if (grp->cmds == NULL)
    return (0);
  while ((cmd = grp->cmds[i]) != NULL)
    {
      if ((cmd->return_value == 0) && (cmd->pid.pid != -1))
        {
          out = waitpid(cmd->pid.pid, &(cmd->return_value), WNOHANG);
          if (out == 0)
            return (1);
          else
            aff_special_return_val(cmd);
        }
      i++;
    }
  return (0);
}

void	wait_all_jobs(t_sh *shell, t_grp **jobtab)
{
  int	i;
  t_grp	*forground_grp;

  i = 0;
  if (jobtab == NULL)
    return ;
  forground_grp = get_forground_grp(shell);
  while (jobtab[i] != NULL)
    {
      if (wait_son(jobtab[i]) == 0)
        {
          SETFLAG(jobtab[i]->flags, FGRP_TERMINATED);
          if (jobtab[i] == forground_grp)
            set_forground_pgrp(shell->pid.pgid);
          else
            {
              if (jobtab[i]->pid.pgid != -1) //this is a tempory solution
                {
                  my_putstr(jobtab[i]->line, 2, -1);
                  my_putstr(" -> Done\n", 2, -1);
                }
            }
          ///debug
          int j = 0;
          printf("grp return value is [");
          while (jobtab[i]->cmds[j] != NULL)
            {
              printf("%d,", jobtab[i]->cmds[j]->return_value);
              j++;
            }
          printf("\n");
          ///
          UNSETFLAG(jobtab[i]->flags, FLAGPOS(FGRP_FORGROUND));
          rm_ptr_f_tab((void**)shell->process_group, (void*)jobtab[i]);
          //delete it ? it's seem yes, don't know if it's a good ieda
        }
      i++;
    }
}

void	wait_no_fg_grp(t_sh* shell)
{
  t_grp	*fg;
  int	tmp;
  int	sig;
  t_cmd	*cmd;

  while ((fg = get_forground_grp(shell)) != NULL)
    {
      sig = 0;
      if ((cmd = cmd_f_pid(waitpid(-1, &tmp, WUNTRACED), shell)) != NULL)
        {
          if (WIFEXITED(tmp))
            {
              cmd->return_value = tmp;
              aff_special_return_val(cmd);
            }
        }
      if (WIFSIGNALED(tmp))
        sig = WTERMSIG(tmp);
      if (WIFSTOPPED(tmp))
        sig = WSTOPSIG(tmp);
      if (WIFCONTINUED(tmp))
        sig = SIGCONT;
      call_signal_func(shell, sig);
    }
  set_forground_pgrp(shell->pid.pgid);
}
