 /********************************************************************************************
 *
 *  FastK: a rapid disk-based k-mer counter for high-fidelity shotgun data sets.
 *     Uses a novel minimizer-based distribution scheme that permits problems of
 *     arbitrary size, and a two-staged "super-mer then weighted k-mer" sort to acheive
 *     greater speed when error rates are low (1% or less).  Directly produces sequence
 *     profiles.
 *
 *  Author:  Gene Myers
 *  Date  :  October, 2020
 *
 *********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

#include "gene_core.h"

static char *Usage = "[-i] <source> ...";

int main(int argc, char **argv)
{ int QUERY;

  { int    i, j, k;
    int    flags[128];

    ARG_INIT("Fastrm")

    j = 1;
    for (i = 1; i < argc; i++)
      if (argv[i][0] == '-')
        switch (argv[i][1])
        { default:
            ARG_FLAGS("i")
            break;
        }
      else
        argv[j++] = argv[i];
    argc = j;

    QUERY = flags['i'];

    if (argc < 2)
      { fprintf(stderr,"\nUsage: %s %s\n",Prog_Name,Usage);
        fprintf(stderr,"\n");
        fprintf(stderr,"      -i: prompt for each (stub) deletion\n");
        exit (1);
      }
  }

  { int   c, len, yes, a;
    char *dir, *root;
    char *command;
    struct stat B;

    len = 0;
    for (c = 1; c < argc; c++)
      if ((int) strlen(argv[c]) > len)
        len = strlen(argv[c]);

    command = Malloc(3*len+50,"Allocating command buffer");

    for (c = 1; c < argc; c++)
      { dir  = PathTo(argv[c]);
        root = Root(argv[c],"");
        if (stat(Catenate(dir,"/",root,".hist"),&B) == 0)
          { yes = 1;
            if (QUERY)
              { printf("remove %s/%s.hist? ",dir,root);
                yes = 0;
                while ((a = getc(stdin)) != '\n')
                  if (a == 'y' || a == 'Y')
                    yes = 1;
                  else if (a == 'n' || a == 'N')
                    yes = 0;
              }
            if (yes)
              unlink(Catenate(dir,"/",root,".hist"));
          }
        if (stat(Catenate(dir,"/",root,".ktab"),&B) == 0)
          { yes = 1;
            if (QUERY)
              { printf("remove %s/%s.ktab & hidden parts? ",dir,root);
                yes = 0;
                while ((a = getc(stdin)) != '\n')
                  if (a == 'y' || a == 'Y')
                    yes = 1;
                  else if (a == 'n' || a == 'N')
                    yes = 0;
              }
            if (yes)
              { sprintf(command,"rm -f %s/%s.ktab %s/.%s.ktab.*",dir,root,dir,root);
                system(command);
              }
          }
        if (stat(Catenate(dir,"/",root,".prof"),&B) == 0)
          { yes = 1;
            if (QUERY)
              { printf("remove %s/%s.prof & hidden parts? ",dir,root);
                yes = 0;
                while ((a = getc(stdin)) != '\n')
                  if (a == 'y' || a == 'Y')
                    yes = 1;
                  else if (a == 'n' || a == 'N')
                    yes = 0;
              }
            if (yes)
              { sprintf(command,"rm -f %s/%s.prof %s/.%s.pidx.* %s/.%s.prof.*",
                                dir,root,dir,root,dir,root);
                system(command);
              }
          }
        free(root);
        free(dir);
      }

    free(command);
  }

  exit (0);
}
