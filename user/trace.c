#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[argc];

  if(argc < 3){
    fprintf(2, "usage: trace mask command\n");
    exit(1);
  }

  if(trace(atoi(argv[1])) < 0){
    fprintf(2, "trace failed\n");
    exit(1);
  }
  //trace 32 grep hello README
  for(i=2;i<argc;i++){
    nargv[i-2]=argv[i];
  }
  
  nargv[argc-2] = 0;

  exec(nargv[0], nargv);
  fprintf(2, "exec %s failed\n", nargv[0]);
  exit(1);
}