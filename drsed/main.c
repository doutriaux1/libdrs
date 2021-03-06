				/* Main DRSED routine */
/* main.c,v 2.1 1993/07/16 00:33:51 drach Exp
   main.c,v
 * Revision 2.1  1993/07/16  00:33:51  drach
 * Declared global variables, modified ifdefs.
 *
 * Revision 2.0  1992/03/02  23:14:59  drach
 * Entered into RCS
 *
*/

#include "drsed.h"
#ifdef mac
#include <stdio.h>
#include <stdlib.h>
#endif

extern DRS_FILE *DRSEDopenfile();
extern int DRSEDreaddicfile();

int interact;        /* interactive flag */
char stringbuf[MAXSTRING];  /* most recently read string */
DRS_FILE *currentfile;      /* most recently referenced DRS file */
DRS_VARIABLE *currentvar;   /* most recently referenced DRS variable */
char *progname;		/* program name */
int lineno;		/* current line number */
FILE *_drsed_outfile;			     /* output stream */
#ifdef sun
int MAIN_;			/* Kluge to fix SunOS 4.1.3 bug */
#endif

main(argc,argv)
	int argc;
	char *argv[];
{
	char *s;
	int ierr;
#ifdef sun
#ifndef mac
	f_init();
#endif
#endif
	Seterr(0,IDRS_WARNING);
	_drsed_outfile = stdout;
	
	progname = argv[0];
	lineno=1;
	interact=1;		/* Default to interactive mode */
				/* Decode arguments */
	while (--argc>0 && (*++argv)[0]=='-')
	{
		s=argv[0]+1;
		switch (*s)
		{
		  case 'b':
			interact=0;
				/* Turn off output buffering in batch mode */
			(void) setbuf(stdout,(char *)0);
			break;
		  default:
			DRSEDerror("Usage: drsed [-b] [-f inputfile]\n");
			exit(1);
		}
	}

	if(argc>0)
	{
		if((currentfile=DRSEDopenfile(*argv))==NULL_FILE)
			DRSEDerror("Error opening file %s, try OPEN command",*argv);
		else
			if(DRSEDreaddicfile(currentfile))
				DRSEDerror("Error reading file %s, try OPEN command",*argv);
		
	}

	ierr=yyparse();
#ifdef sun
#ifndef mac
	f_exit();
#endif
#endif
	exit(ierr);
}
				/* Allocate mem, or reallocate if ptr is nonnull */
char *DRSEDalloc(size,ptr)
	int size;
	char *ptr;
{
	char *s;
/*	if((s=(ptr==NULL ? malloc(size) : realloc(ptr,size)))==NULL) */
#ifdef mac
	if((s=(ptr==NULL ? (char *)malloc(size) : (free(ptr),(char *)malloc(size))))==NULL)
#else
	if((s=(ptr==NULL ? (char *)calloc(size,1) : (cfree(ptr),(char *)calloc(size,1))))==NULL)
#endif
		DRSEDerror("Error allocating space of size %d",size);
	return(s);
}


