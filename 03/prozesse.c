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

  if ((child_A = fork()) == 0)      // Child A starten -- bin ich A?
    child_A_proc();                 // dann child_A_proc() starten
  else if ((child_B = fork()) == 0) // ... sonst B starten -- bin ich B?
    child_B_proc();                 // dann child_B_proc() starten
  else                              // ... sonst bin ich der Parent,
    parent_proc();                  // also parent_proc() starten

  return 0;
}
