/*********************************************************************************************\
 *
 *  Example code for opening and fetching compressed profiles produced by FastK
 *
 *  Author:  Gene Myers
 *  Date  :  October, 2020
 *
 *********************************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>

#include "libfastk.h"

static char *Usage = "<source_root>[.prof] <read:int> ...";

/****************************************************************************************
 *
 *  Test Stub
 *
 *****************************************************************************************/

int main(int argc, char *argv[])
{ char          *name;
  int            kmer, nthreads;
  Profile_Index *P;

  { int    i, j, k;
    int    flags[128];

    (void) flags;

    ARG_INIT("Profex");

    j = 1;
    for (i = 1; i < argc; i++)
      if (argv[i][0] == '-')
        switch (argv[i][1])
        { default:
            ARG_FLAGS("")
            break;
        }
      else
        argv[j++] = argv[i];
    argc = j;

    if (argc < 3)
      { fprintf(stderr,"Usage: %s %s\n",Prog_Name,Usage);
        exit (1);
      }
  }

  { FILE *f;
    char *dir, *root;

    dir  = PathTo(argv[1]);
    root = Root(argv[1],".prof");
    name = Strdup(Catenate(dir,"/.",root,""),NULL);
    f = fopen(Catenate(dir,"/",root,".prof"),"r");
    if (f == NULL)
      { fprintf(stderr,"%s: Cannot open %s for reading\n",Prog_Name,Catenate(dir,"/",root,".prof"));
        exit (1);
      }
    fread(&kmer,sizeof(int),1,f);
    fread(&nthreads,sizeof(int),1,f);
    fclose(f);
    free(root);
    free(dir);
  }

  P = Open_Profiles(name,kmer,nthreads);

  { int     c, id;
    char   *eptr;
    uint16 *profile;
    int     plen, tlen;

    plen    = 20000;
    profile = Malloc(plen*sizeof(uint16),"Profile array");

    for (c = 2; c < argc; c++)
      { id = strtol(argv[c],&eptr,10);
        if (*eptr != '\0' || argv[c][0] == '\0')
          { fprintf(stderr,"%s: argument '%s' is not an integer\n",Prog_Name,argv[c]);
             exit (1);
          }
        if (id <= 0 || id > P->nbase[P->nparts-1])
          { fprintf(stderr,"%s: Id %d is out of range\n",Prog_Name,id);
            exit (1);
          }
        tlen = Fetch_Profile(P,(int64) id-1,plen,profile);
        if (tlen > plen)
          { plen    = 1.2*tlen + 1000;
            profile = Realloc(profile,plen*sizeof(uint16),"Profile array");
            Fetch_Profile(P,(int64) id-1,plen,profile);
          }
        printf("\nRead %d:\n",id);
        for (int i = 0; i < tlen; i++)
          printf(" %5d: %5d\n",i,profile[i]);
      }
    free(profile);
  }

  Free_Profiles(P);

  free(name);

  Catenate(NULL,NULL,NULL,NULL);
  Numbered_Suffix(NULL,0,NULL);
  free(Prog_Name);

  exit (0);
}
