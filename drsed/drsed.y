/* syntax for drsed */
/* drsed.y,v 2.3 1993/07/16 00:32:36 drach Exp
   drsed.y,v
 * Revision 2.3  1993/07/16  00:32:36  drach
 * Made dummy '(err=err)' entries in DRSEDCALL macros, for compatibility with HP.
 *
 * Revision 2.2  1992/10/14  23:41:21  drach
 * Added SET syntax.
 *
 * Revision 2.1  1992/06/24  17:14:37  drach
 * added SET statement syntax
 *
 * Revision 2.0  1992/03/02  23:14:59  drach
 * Entered into RCS
 *
*/

%{
#include "drsed.h"
#include <string.h>
#define DRSEDCALL(X,Y) err=X;\
	               if(!interact)\
		       	  DRSEDbatcherror(err);\
		       if(!err) Y;
#define DRSEDYYERROR if(!interact)\
		        DRSEDbatcherror(DRSED_GENERIC);\
                     YYERROR
static char *sink;
static char singleBlank[]=" ";
static int size;
extern DRS_FILE *DRSEDopenfile();
extern int DRSEDreaddicfile();
extern DRS_VARIABLE *DRSEDfindvar();
static int err;  /* error return */
static int err2;
static int ndim; /* number of dimensions */
extern FILE *_drsed_outfile;

void DRSEDhelp(int option);


char c;
%}
%union {
         double dval;
	 int ival;
	 char *sval;
	 DRS_FILE *fileval;
	 DRS_VARIABLE *varval;
       }

%token ALL BATCH BRIEF CLOSE CREATE DATA DESCRIBE FULL
%token HELP INDEX INTERACTIVE LENGTH
%token LIST MODE NAME ND NV OPEN PRINT PRINTI PRINTU QUIT
%token RANGE SET SHAPE SOURCE TIME TITLE TYPE UNITS WRITE WRITEU
%token <dval> DOUBLE_CONST
%token <ival> INTEGER_CONST
%token <sval> USERSTRING IDENT
%type <fileval> existingfile
%type <sval> file format setstring
%type <ival> listopt statementtype filefield varfield derivarfield dim
dimfield setfield

%type <varval> var var1
%type <dval> const
%%
statementlist:    /* nothing */ {if(interact) printf(">");}
		| statementlist '\n' {if(interact) printf(">");}
		| statementlist statement '\n' {if(interact) printf(">");}
		| statementlist error '\n' {yyerrok; if(interact) printf(">");}
		;
statement:	closestatement
		| createstatement
		| describestatement
		| getstatement
		| getdimstatement
		| helpstatement
		| liststatement
		| modestatement
		| openstatement
		| printstatement
		| printistatement
		| printustatement
		| setstatement
		| writestatement
		| writeustatement
		;
closestatement: CLOSE {DRSEDCALL(DRSEDcloseoutputfile(&_drsed_outfile),(err=err));
		      	_drsed_outfile=stdout;
			}
		;
file:		IDENT
		|USERSTRING
		;
createstatement: CREATE file { DRSEDCALL(DRSEDopenoutputfile($2,&_drsed_outfile),(err=err));
			       if(_drsed_outfile==NULL) _drsed_outfile=stdout;
			     }
		 ;
describestatement: DESCRIBE varspec {
					DRSEDCALL(currentvar==NULL_VARIABLE,
					 DRSEDdescribevar(currentvar))}
		   ;
var:		   var1 {if($1==NULL_VARIABLE)
				       {DRSEDerror("No variable current");
					DRSEDYYERROR;
				       }
			       else
				       $$=currentvar=$1;}

		   ;
var1:		/* nothing */ {$$=currentvar;}
		| IDENT	   {$$=DRSEDfindvar($1,currentfile);}
		| USERSTRING {$$=DRSEDfindvar($1,currentfile);}
		;
getstatement:	filefield {if(err=(currentfile==NULL_FILE))
			  	DRSEDerror("No file currently specified, use OPEN");
			   DRSEDCALL(err,
			    		DRSEDgetfilefield(currentfile,$1))}
		|varfield varspec {
			       	       DRSEDCALL(currentvar==NULL_VARIABLE,
			       		DRSEDgetvarfield(currentvar,$1))}
		|derivarfield varspec {DRSEDCALL(currentvar==NULL_VARIABLE,
				      	DRSEDgetvarfield(currentvar,$1))}
		;
filefield:	NV {$$=NV;} 
		;
varfield:	SOURCE {$$=SOURCE;} 
		|NAME {$$=NAME;} 
		|TITLE {$$=TITLE;} 
		|UNITS {$$=UNITS;} 
		|TIME {$$=TIME;} 
		|TYPE {$$=TYPE;} 
		|ND {$$=ND;}
		;
derivarfield:	LENGTH {$$=LENGTH;}
		|SHAPE {$$=SHAPE;}
		|INDEX {$$=INDEX;}
		;
getdimstatement: dimfield dim var { if(($3 != NULL_VARIABLE) && ($2 < 1 || $2 > $3->ndim))
				    {
				      DRSEDerror("Variable %s does not have a dimension %d",$3->name,$2); DRSEDYYERROR;}
				      DRSEDCALL($3==NULL_VARIABLE,
				  	DRSEDgetdimfield(FLUSH,&$3->dim[$2-1],$1))}
		 ;
dimfield:	SOURCE {$$=SOURCE;}
		|NAME {$$=NAME;}
		|TITLE {$$=TITLE;}
		|UNITS {$$=UNITS;}
		|RANGE {$$=RANGE;}
		|DATA {$$=DATA;}
		;
dim:		INTEGER_CONST {$$=$1;}
		;
helpstatement:  HELP statementtype {DRSEDCALL(0,DRSEDhelp($2))}
		;
statementtype:   {$$=ALL;} /* nothing */
		|ALL {$$=ALL;}
		|CLOSE {$$=CLOSE;}
		|CREATE {$$=CREATE;}
		|DATA {$$=DATA;}
		|DESCRIBE {$$=DESCRIBE;}
		|HELP {$$=HELP;}
		|LENGTH {$$=LENGTH;}
		|LIST {$$=LIST;}
		|MODE {$$=MODE;}
		|NAME {$$=NAME;}
		|ND {$$=ND;}
		|NV {$$=NV;}
		|OPEN {$$=OPEN;}
		|PRINT {$$=PRINT;}
		|PRINTI {$$=PRINTI;}
		|PRINTU {$$=PRINTU;}
		|QUIT {$$=QUIT;}
		|RANGE {$$=RANGE;}
		|SET {$$=SET;}
		|SHAPE {$$=SHAPE;}
		|SOURCE {$$=SOURCE;}
		|TIME {$$=TIME;}
		|TITLE {$$=TITLE;}
		|TYPE {$$=TYPE;}
		|UNITS {$$=UNITS;}
		;
liststatement:  LIST listopt {if(currentfile==NULL_FILE) {
	                          DRSEDerror("No file currently open");
				  DRSEDCALL(DRSED_GENERIC,(err=err));
			  } else if (currentfile->varlist==NULL_VARIABLE)
			  {
				  DRSEDerror("%s does not contain any variables!",currentfile->name);
				  DRSEDCALL(DRSED_GENERIC,(err=err));
			  } else
			  {
				  DRSEDCALL(0,DRSEDlist(currentfile,$2));
			  }
			  }
		;
listopt:	 /* nothing */ {$$=(interact ? FULL : BRIEF);}
		|BRIEF {$$=BRIEF;}
		|FULL {$$=FULL;}
		;
modestatement:	MODE modeopt {DRSEDCALL(0,(err=err));}
		;
modeopt:	INTERACTIVE {interact=1;}
		|BATCH {interact=0;}
		;
openstatement:	OPEN existingfile {if($2 != NULL_FILE) 
					   currentfile=$2;
				   DRSEDCALL(DRSEDreaddicfile($2),(err=err))}
		;
existingfile:	file {		/* Don't reopen the same file */
	                 if(currentfile != NULL_FILE && !strcmp($1,currentfile->name))
				 $$=currentfile;
			 else
			 {
				 if(currentfile != NULL_FILE)
				 {
					 
					 Cllun(currentfile->unit);
					 currentfile=NULL_FILE;
				 }
				 $$=DRSEDopenfile($1);
			 }
		 }

		;
printstatement: PRINT varspec {DRSEDCALL(DRSEDreadvardata(currentvar),
			      	DRSEDprintvardata(currentvar))}
		;
varspec:	var
		{if($1==NULL_VARIABLE)
			{
			DRSEDerror("Variable not found");
			DRSEDYYERROR;
			}
		 else
		 {currentvar=$1;
		  ndim=0;
		 }
		}
		dimspec
		{if (ndim!=currentvar->ndim)
		    {
		        DRSEDerror("Must specify %d dimensions for %s",currentvar->ndim, currentvar->name);
			DRSEDYYERROR;
			}
		}
		;
dimspec:	/* nothing */
		{
		for(ndim=0; ndim<currentvar->ndim; ndim++)
		  {
			  currentvar->dim[ndim].reqfirstel=currentvar->dim[ndim].firstel;
			  currentvar->dim[ndim].reqlastel=currentvar->dim[ndim].lastel;
		  }
		}
		|'(' dimlist ')'
		;
dimlist:	dimint
		| dimlist ',' dimint
		;
dimint:		{ if(ndim>=currentvar->ndim)
		  {
		  	  DRSEDerror("Too many dimensions specified for variable %s",currentvar->name);
			  DRSEDYYERROR;
		  }
		}
		dimint1
		;
dimint1: 	'*'
		  {
			  currentvar->dim[ndim].reqfirstel=currentvar->dim[ndim].firstel;
			  currentvar->dim[ndim].reqlastel=currentvar->dim[ndim].lastel;
			  ndim++;
		  }
		| const
		  {
			  currentvar->dim[ndim].reqfirstel=$1;
			  currentvar->dim[ndim].reqlastel=$1;
			  ndim++;
		  }
		| const ':' const
		  {
			  currentvar->dim[ndim].reqfirstel=$1;
			  currentvar->dim[ndim].reqlastel=$3;
			  ndim++;
		  }
		;
printistatement: PRINTI ivarspec {DRSEDCALL(DRSEDreadvardata(currentvar),
			      	DRSEDprintvardata(currentvar))}
		;
ivarspec:	var
		{if($1==NULL_VARIABLE)
			{
			DRSEDerror("Variable not found");
			DRSEDYYERROR;
			}
		 else
		 {currentvar=$1;
		  ndim=0;
		 }
		}
		idimspec
		{if (ndim!=currentvar->ndim)
		    {
		        DRSEDerror("Must specify %d dimensions for %s",currentvar->ndim, currentvar->name);
			DRSEDYYERROR;
			}
		}
		;
idimspec:	/* nothing */
		{
		for(ndim=0; ndim<currentvar->ndim; ndim++)
		  {
			  currentvar->dim[ndim].reqfirstel=currentvar->dim[ndim].firstel;
			  currentvar->dim[ndim].reqlastel=currentvar->dim[ndim].lastel;
		  }
		}
		|'(' idimlist ')'
		;
idimlist:	idimint
		| idimlist ',' idimint
		;
idimint:		{ if(ndim>=currentvar->ndim)
		  {
		  	  DRSEDerror("Too many dimensions specified for variable %s",currentvar->name);
			  DRSEDYYERROR;
		  }
		}
		idimint1
		;
idimint1: 	'*'
		  {
			  currentvar->dim[ndim].reqfirstel=currentvar->dim[ndim].firstel;
			  currentvar->dim[ndim].reqlastel=currentvar->dim[ndim].lastel;
			  ndim++;
		  }
		| INTEGER_CONST
		  {
		  	  DRSEDCALL(err2=DRSEDsetreqel(currentvar,ndim,$1,$1),(err=err));
			  if(err2) YYERROR;
			  ndim++;
		  }
		| INTEGER_CONST ':' INTEGER_CONST
		  {
		  	  DRSEDCALL(err2=DRSEDsetreqel(currentvar,ndim,$1,$3),(err=err));
			  if(err2) YYERROR;
			  ndim++;
		  }
		;
printustatement: PRINTU varspec {DRSEDCALL(DRSEDreadvardata(currentvar),
			      	DRSEDprintuvardata(currentvar))}
		 ;
writestatement:  WRITE varspec format {DRSEDCALL(DRSEDreadvardata(currentvar),
				      DRSEDwritevardata(currentvar,_drsed_outfile,$3))}
		;
format:		/* nothing */ {$$="";}
		| IDENT
		| USERSTRING
		;
writeustatement: WRITEU varspec {DRSEDCALL(DRSEDreadvardata(currentvar),
				DRSEDwriteuvardata(currentvar,_drsed_outfile))}
		;
const:		INTEGER_CONST {$$=$1;}
		| DOUBLE_CONST {$$=$1;}
		;
setstatement:   SET setfield var '=' setstring {DRSEDCALL(currentvar == NULL_VARIABLE,
					     DRSEDsetvar(currentvar,$2,$5))}
                | SET setfield dim var '=' setstring
		       { if(( $4 != NULL_VARIABLE) && ($3<1 || $3>$4->ndim))
		         {DRSEDerror("Variable %s does not have a dimension %d",$4->name,$3); DRSEDYYERROR;}
			  DRSEDCALL(currentvar==NULL_VARIABLE,
			    DRSEDsetdim(&currentvar->dim[$3-1],$2,$6))}
		;
setfield:	SOURCE {$$=SOURCE;} 
		|NAME {$$=NAME;} 
		|TITLE {$$=TITLE;} 
		|UNITS {$$=UNITS;} 
		;
setstring:	/* nothing */ {$$=singleBlank;}
		| IDENT
		| USERSTRING
		;
%%
#include "drsed.yy.c"
				/* print help */
void
DRSEDhelp(option)
	int option;
{
	switch(option)
	{
	  case ALL:
		DRSEDprintf(NOFLUSH,"The following commands are available:\n");
	  case CLOSE:
		DRSEDprintf(NOFLUSH,"CLOSE - Close the current output file.\n");
		if(option!=ALL) break;
	  case CREATE:
		DRSEDprintf(NOFLUSH,"CREATE outputfile - Create an ASCII output file.\n");
		if(option != ALL) break;
	  case DATA:
		DRSEDprintf(NOFLUSH,"DATA dimension-number [variable] - Print dimension values.\n");
		if(option != ALL) break;
	  case DESCRIBE:
		DRSEDprintf(NOFLUSH,"DESCRIBE [variable] - Print variable description.\n");
		
		if(option != ALL) break;
	  case HELP:
		DRSEDprintf(NOFLUSH,"HELP [command|ALL]\n");
		
		if(option != ALL) break;
	  case LENGTH:
		DRSEDprintf(NOFLUSH,"LENGTH [variable] - Number of variable elements.\n");
		
		if(option != ALL) break;
	  case LIST:
		DRSEDprintf(NOFLUSH,"LIST [BRIEF|FULL] - List names and titles in current DRS file.\n");
		
		if(option != ALL) break;
	  case MODE:
		DRSEDprintf(NOFLUSH,"MODE [INTERACTIVE|BATCH]\n");
		
		if(option != ALL) break;
	  case NAME:
		DRSEDprintf(NOFLUSH,"NAME [dimension-number] [variable] - Name of (dimension of) variable.\n");
		
		if(option != ALL) break;
	  case ND:
		DRSEDprintf(NOFLUSH,"ND [variable] - Number of dimensions of variable.\n");
		
		if(option != ALL) break;
	  case NV:
		DRSEDprintf(NOFLUSH,"NV - Number of variables in current file.\n");
		
		if(option != ALL) break;
	  case OPEN:
		DRSEDprintf(NOFLUSH,"OPEN drs-dictionary-file - Open a DRS file.\n");
		
		if(option != ALL) break;
	  case PRINT:
		DRSEDprintf(NOFLUSH,"PRINT variable[(user-dimension-ranges)]] - Print variable to standard output.\n");
		
		if(option != ALL) break;
	  case PRINTI:
		DRSEDprintf(NOFLUSH,"PRINTI variable[(index-dimension-ranges)]] - Print variable to standard output.\n");
		
		if(option != ALL) break;
	  case PRINTU:
	/*	DRSEDprintf(NOFLUSH,"PRINTU variable[(dimension-ranges)]]\n"); */
		
		if(option != ALL) break;
	  case QUIT:
		DRSEDprintf(NOFLUSH,"QUIT - Quit DRSED.\n");
		
		if(option != ALL) break;
	  case RANGE:
		DRSEDprintf(NOFLUSH,"RANGE dimension-number [variable[(dimension-ranges)]] - First and last values of a dimension of the variable.\n");
		
		if(option != ALL) break;
	  case SET:
		DRSEDprintf(NOFLUSH,"SET source|name|title|units [dimension-number] [variable] = new-string - Set a naming string for a variable or dimension.\n");
		if(option != ALL) break;
	  case SHAPE:
		DRSEDprintf(NOFLUSH,"SHAPE [variable] - Length of each dimension of the variable.\n");
		
		if(option != ALL) break;
	  case SOURCE:
		DRSEDprintf(NOFLUSH,"SOURCE [dimension-number] [variable] - Source of (dimension of) variable.\n");
		
		if(option != ALL) break;
	  case TIME:
		DRSEDprintf(NOFLUSH,"TIME [variable] - Time that variable was written\n");
		
		if(option != ALL) break;
	  case TITLE:
		DRSEDprintf(NOFLUSH,"TITLE [dimension-number] [variable] - Title of (dimension of) variable.\n");
		
		if(option != ALL) break;
	  case TYPE:
		DRSEDprintf(NOFLUSH,"TYPE [variable] - Data type of variable.\n");
		
		if(option != ALL) break;
	  case WRITE:
		DRSEDprintf(NOFLUSH,"WRITE [variable[(dimension-ranges)]] - Write to the ASCII outputfile created by the most recent CREATE command.\n");
		if(option != ALL) break;
	  case UNITS:
		DRSEDprintf(NOFLUSH,"UNITS [dimension-number] [variable] - Units of (dimension of) variable.\n");
		break;
	}
	switch(option){
	  case ALL:
	  case NAME:
	  case PRINT:
	  case PRINTI:
	  case RANGE:
	  case WRITE:
		DRSEDprintf(NOFLUSH,"\n   The form of dimension-ranges is (range-1,range-2,...,range-n),\nwhere range is either a single dimension value, a range of values of the form value-1:value2,\nor '*' for the entire dimension range. For example,\n\n                print x(*,1,10.5:100)\n\nprints the values of the variable x for all values of the first dimension, where the second\nUSER dimension value is 1 and the third USER dimension values range from 10.5 to 100, inclusive.\n\n   The command `printi' uses indices instead of user dimension values. For example,\n\n                printi x(*,1:5,11)\n\nprints the values of x corresponding to all values of the first dimension, the first five values of the second dimension, and the eleventh value of the third dimension.\n\n   For most commands, if no variable is specified, the most recently\nreferenced variable is used.\n");
	}
}
