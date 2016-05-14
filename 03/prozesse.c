#include <stdio.h>
#include <unistd.h>

void child_B_proc()
{
  while (1) {
    fprintf(stdout, "%s", "C");
    fflush(stdout);
  }
}

void child_A_proc()
{
  while (1) {
    fprintf(stdout, "%s", "A");
    fflush(stdout);
  }
}

void parent_proc()
{
  while (1) {
    write(fileno(stdout), "B", 1);
  }
}

int main(void)
{
  int child_A, child_B;

//  child_A = fork();                 /* neuen Prozess starten                          */
//  Zuweisungen mit fork in den if Klammern, da sich sonst der erste child process mitforken würde
  if ((child_A = fork()) == 0)        /* Bin ich der Sohnprozess?                       */
    child_A_proc();                   /* ... dann child-Funktion ausfuehren             */
  else if ((child_B = fork()) == 0)
    child_B_proc();
  else
    parent_proc();                    /* ... sonst parent-Funktion ausfuehren           */

  return 0;
}
