#include <stdio.h>
#include <ctype.h>
#include "lisp.h"

int _stack = 5000;

LIST *TRU;
LIST *alist;
LIST *oblist;

char progon;


FILE *fd;     /* input file descriptor */


/********************************************************/
/*			main program			*/
/********************************************************/

main()
{
  initialize();
  load_library();

  printf("\t\tMarc Adler's LISP Interpreter\n");
  fd = stdin;
  interpret();
}



interpret()
{
  LIST *p, *q;   register int c;

  while ((c = gettok()) != EOF) {
    switch (c) {
      case LPAREN   : getc(fd);   /* span the paren */
		      q = makelist();
		      p = eval(q, alist);
		      break;

      case LETTER   : p = cdr(car(getid()));
		      break;
    }

    if (fd == stdin) {
      printf("value => ");
      if (p == NULL) printf("nil");
      else lisp_print(cons(p, NULL));
      printf("\n");
    }
  }
}



/******************************************************************/
/*	       initialization proceedures			  */
/******************************************************************/

initialize()
{
  init("'", QUOTE);		init("car", FCAR);
  init("cond", COND);           init("cdr", FCDR);
  init("defun", DEFUN);         init("cons",FCONS);
  init("nil", NILL);            init("atom",FATOM);
  init("prog",PROG);            init("eq",  FEQ);
  init("go",  GO);              init("setq",FSETQ);
  init("return",RETRN);         init("print",PRINT);
  init("read", FREAD);
  init("rplaca",FREPLACA);      init("rplacd",FREPLACD);
  init("apply", FAPPLY);        init("eval",  FEVAL);
  init("and", FAND);            init("or", FOR);
  init("not", FNOT);
  init("plus",  PLUS);          init("zerop", ZEROP);
  init("diff",  DIFF);          init("greaterp", GREATERP);
  init("times", TIMES);         init("lessp", LESSP);
  init("add1",  ADD1);          init("sub1",  SUB1);
  init("quot",  QUOTIENT);      TRU = cons(init("t",T), NULL);
  init("numberp",NUMBERP);      rplact(TRU, SATOM);
  init("null",  NUL);           init("funcall",FUNCALL);

  oblist = alist;
}


LIST *init(name, t)
  char *name;  int t;
{
  register LIST *p;

  p = install(name);
  rplact(p, t);
  return(p);
}


/******************************************************************/
/*     create the executable list form of a LISP program	  */
/******************************************************************/

LIST *makelist()
{
  register LIST *p;

  switch (gettok()) {
    case LPAREN   : getc(fd);	/* span the paren ????? */
		    p = makelist();
		    p = cons(p, makelist());
		    rplact(p, LST);
		    return(p);

    case LETTER   : p = getid();
		    return(cons(p, makelist()));

    case INQUOTE  : p = getid();
		    p = cons(p, makelist());
		    rplaca(p, cons(car(p), cons(car(cdr(p)), NULL)));
		    rplacd(p, cdr(cdr(p)));
		    return(p);

    case DIGIT    : p = getnum();
		    return(cons(p, makelist()));

    case RPAREN   : getc(fd);	/* span rparen ?????? */
		    return(NULL);
  }
}


/* lisp_print - walks along the list structure printing atoms */
lisp_print(p)
  register LIST *p;
{
  char *getname();

  if (p != NULL)

    if (type(p) == RATOM)	printf("%f ", p->u.num);
    else if (type(p) == IATOM)	printf("%d ", (int) p->u.num);
    else if (type(p) == SATOM)	printf("%s ", getname(car(p)));
    else if (type(car(p)) == LST) {
      putchar('(');
      lisp_print(car(p));
      putchar(')');
      lisp_print(cdr(p));
    }
    else if (type(p) == LST) {
      lisp_print(car(p));
      lisp_print(cdr(p));
    }
    else
      printf("******** can't print it out *******\n");
}


/******************************************************************/
/*		    evaluate a LISP function			  */
/******************************************************************/

LIST *eval(x, alist)
  register LIST *x, *alist;
{
  register LIST *p, *q;  int savt, t;   char *getname();

  if (x == NULL)  return(NULL);
  t = type(x);
  if (t == VARI) return(assoc(alist, getname(car(x))));
  if (t == IATOM || t == RATOM) return(x);
  if (t == LABL) return(NULL);

  switch (type(car(x))) {

	case T  	: return(TRU);

	case NILL	: return(NULL);

	case QUOTE	: var_to_atom(car(cdr(x)));
			  return(car(cdr(x)));

	case FCAR	: return(car(eval(cdr(x), alist)));

	case FCDR	: return(cdr(eval(cdr(x), alist)));

	case FATOM	: return(atom(eval(cdr(x), alist)));

	case FEQ	: return(eq(eval(car(cdr(x)),alist),
				    eval(cdr(cdr(x)),alist)));

	case NUL	: return(eq(eval(car(cdr(x)), alist), NULL));

	case FCONS	: return(cons(eval(car(cdr(x)),alist),
				      eval(cdr(cdr(x)), alist)));

	case FLIST	: return(_list(x));

	case COND	: return(evalcond(cdr(x), alist));

	case FSETQ	: p = eval(cdr(cdr(x)), alist);
			  rplacd(getvar(alist, getname(car(car(cdr(x))))), p);
			  return(p);

	case DEFUN	: rplact(car(car(cdr(x))), FUSER);
			  rplacd(car(car(cdr(x))), cdr(cdr(x)));
			  var_to_user(cdr(cdr(cdr(x))));
			  if (fd == stdin)
			    printf("%s\n", getname(car(car(cdr(x)))));
			  return(NULL);

	case FUSER	: p = cdr(car(car(x)));   /* p is statement list */
			  return( eval(car(cdr(p)),
			pairargs(car(p),evalargs(cdr(x),alist),alist,FALSE)));

	case FAPPLY	:
	case FUNCALL	: p = eval(car(cdr(x)), alist);   /* func name */
			  if (isfunc( savt = type(car(p)) )) {
			    p = cons(p, cdr(cdr(x)));
			    if (savt == FUSER)
			      rplact(car(p), FUSER);
			    q = eval(p, alist);
			    rplact(car(p), savt);
			    return(q);
			  }
			  else return(NULL);


	case FEVAL	: p = eval(cdr(x), alist);
			  if (type(p) == SATOM)
			    return(assoc(alist, getname(car(p))));
			  else
			    return(eval(p, alist));

	case PRINT	: lisp_print(eval(car(cdr(x)), alist));
			  putchar('\n');
			  return(NULL);

	case FREAD	: return(makelist());

	case FAND	: return(_and(x));
	case FOR	: return(_or(x));
	case FNOT	: return(_not(x));

	case PLUS	:
	case DIFF	:
	case TIMES	:
	case QUOTIENT	:
	case GREATERP	:
	case LESSP	:  return(arith(car(x), eval(car(cdr(x)),alist),
						eval(cdr(cdr(x)),alist)));

	case ADD1	:
	case SUB1	:return(arith(car(x), eval(car(cdr(x)),alist),NULL));

	case ZEROP	: p = eval(car(cdr(x)), alist);
			  return( (p->u.num == 0) ? TRU : NULL);

	case NUMBERP	: savt = type(eval(car(cdr(x)), alist));
			  return( (savt==IATOM || savt==RATOM) ? TRU : NULL);

	case PROG	: return(evalprog(x, alist));

	case GO 	: return(cdr(car(car(cdr(x)))));

	case RETRN	: progon = FALSE;
			  return(eval(cdr(x), alist));

	case LST	: if (cdr(x) == NULL) return(eval(car(x), alist));
			  return(cons(eval(car(x),alist),eval(cdr(x),alist)));

	case VARI	: return(assoc(alist, getname(car(car(x)))));

	case IATOM	:
	case RATOM	: return(car(x));

  } /* switch */
}


LIST *evalcond(expr, alist)
  LIST *expr, *alist;
{
  if (expr == NULL)  return(NULL);
  if (eval(car(car(expr)), alist) != NULL)     /* expr was true */
    return(eval(car(cdr(car(expr))), alist));  /* return result */
  return(evalcond(cdr(expr), alist));	       /* eval rest of args */
}


LIST *evalprog(p, alist)
  LIST *p, *alist;
{
  register LIST *x;

  /* set up parameters as locals */
  alist = pairargs(car(cdr(p)), cons(NULL, NULL), alist, TRUE);
  progon = TRUE;
  p = cdr(cdr(p));	 /* p now points to the statement list */
  find_labels(p);	 /* set up all labels in the prog */

  while (p != NULL && progon) {
    x = eval(car(p), alist);
    if (type(car(car(p))) == GO)
      p = x;    	 /* GO returned the next statement to go to */
    else
      p = cdr(p);	 /* just follow regular chain of statements */
  }

  progon = TRUE;	 /* in case of nested progs */
  return(x);
}


/* pairargs - installs parameters in the alist, and sets the value to be */
/*	      the value of the corresponding argument			 */
LIST *pairargs(params, args, alist, prog)
  LIST *params, *args, *alist;	int prog;
{
  register LIST *p;

  if (params == NULL)	 /* no more args to be evaluated */
    return(alist);

  p = cons(NULL, car(args));  /* value of param is corresponding arg */
  p->u.pname = getname(car(car(params)));
  rplact(p, VARI);
  if (prog)
    return(cons(p, pairargs(cdr(params), cons(NULL,NULL), alist, prog)));
  else
    return(cons(p, pairargs(cdr(params), cdr(args),	  alist, prog)));
}


LIST *evalargs(arglist, alist)
  LIST *arglist, *alist;
{
  if (arglist == NULL) return(NULL);
  return(cons(eval(car(arglist),alist), evalargs(cdr(arglist), alist)));
}


LIST *assoc(alist, name)
  LIST *alist;	char *name;
{
  return(cdr(getvar(alist, name)));
}

LIST *getvar(alist, name)
  LIST *alist;	char *name;
{
  return(lookup(alist, name));
}


/* arith - performs arithmetic on numeric items */
LIST *arith(op, x, y)
  LIST *op, *x, *y;
{
  LIST *p;  float res;   int t = type(op);

  if (t == LESSP) return((x->u.num < y->u.num) ? TRU : NULL);
  if (t == GREATERP) return ((x->u.num > y->u.num) ? TRU : NULL);

  switch (t) {
    case PLUS   : res = x->u.num + y->u.num;  break;
    case DIFF   : res = x->u.num - y->u.num;  break;
    case TIMES  : res = x->u.num * y->u.num;  break;
    case QUOTIENT:res = x->u.num / y->u.num;  break;
    case ADD1   : res = x->u.num + 1;       break;
    case SUB1   : res = x->u.num - 1;       break;
  }

  p = cons(NULL, NULL);
  if (type(x) == IATOM && (type(y) == IATOM || t == ADD1 || t == SUB1)) {
    p->u.num = (int) res;  rplact(p, IATOM);
  }
  else {
    p->u.num = res;	   rplact(p, RATOM);
  }
  return(p);
}




/******************************************************************/
/*			    input functions			  */
/******************************************************************/

/* advance - skips white space in input file */
advance()
{
  register int c;  char *strchr();

  while ((c=getc(fd)) != EOF && strchr(" \t\n",c) != NULL) ;
  ungetc(c, fd);
  return (c);
}

LIST *lookup(head, name)
  LIST *head;  char *name;
{
  register LIST *p;   char *getname();

  for (p = head; p != NULL && strcmp(name,getname(car(p))); p = cdr(p))
    ;
  return ((p == NULL) ? NULL : car(p));
}

LIST *install(name)
  char *name;
{
  register LIST *p;   char *emalloc();

  p = cons(NULL, NULL);
  strcpy(p->u.pname = emalloc(strlen(name)+1), name);
  rplact(p, VARI);
  alist = cons(p, alist);
  return(p);
}

LIST *getnum()
{
  register LIST *p;    float sum, n;  int c;

  sum = 0.0;
  p = cons(NULL, NULL);
  rplact(p, IATOM);

  while (isdigit(c = getc(fd)))
    sum = sum * 10 + c - '0';

  if (c == '.') {	 /* the number is real */
    n = 10;
    rplact(p, RATOM);
    while (isdigit(c = getc(fd))) {
      sum += (c - '0')/n;
      n *= 10;
    }
  }

  ungetc(c, fd);
  p->u.num = sum;
  return(p);
}


LIST *getid()
{
  char inbuf[120];
  register LIST *p, *idptr;   int c;   register char *s = inbuf;

 *s++ = c = getc(fd);
 if (c != '\'') {
   for ( ; isalnum(c=getc(fd)); *s++ = c) ;
   ungetc(c, fd);
 }
 *s = '\0';

 if ((idptr = lookup(oblist,inbuf)) == NULL)	 /* not a LISP function */
   if ((idptr = lookup(alist,inbuf)) == NULL)	 /* id not declared yet */
     idptr = install(inbuf);			 /* install it in alist */

 p = cons(idptr, NULL);
 rplact(p, type(idptr));
 return(p);
}


gettok()
{
  char c, buf[120];

  while ((c = advance()) == ';')     /* saw a comment */
    fgets(buf, 120, fd);	     /* eat the rest of the line */
  if (isalpha(c))
    return(LETTER);
  if (isdigit(c))
    return(DIGIT);
  switch (c) {
    case '('    : return(LPAREN);
    case ')'    : return(RPAREN);
    case '\''   : return(INQUOTE);
    default	: return(ERR);
  }
}


/*****************************************************************/
/*		     LISP primitive functions			 */
/*****************************************************************/

/* new - gets a new node from the free storage */
LIST *new()
{
  register LIST *p;  char *emalloc();

  p = (struct LIST *) emalloc(sizeof(LIST));
  p->gcbit = RUNNING;
}

type(p)
  register LIST *p;
{
  return((int) p->htype);
}

char *getname(p)
  register LIST *p;
{
  return((p == NULL) ? NULL : p->u.pname);
}

rplaca(p, q)
  register LIST *p, *q;
{
  p->left = q;
}

rplacd(p, q)
  register LIST *p, *q;
{
  p->right = q;
}

rplact(p, t)
  register LIST *p;  int t;
{
  p->htype = t;
}

LIST *car(p)
  register LIST *p;
{
  return((p == NULL) ? NULL : p->left);
}

LIST *cdr(p)
  register LIST *p;
{
  return((p == NULL) ? NULL : p->right);
}

LIST *cons(p, q)
  register LIST *p, *q;
{
  register LIST *x;

  x = new();
  rplaca(x, p);
  rplacd(x, q);
  rplact(x, LST);
  return(x);
}

LIST *eq(x, y)
  register LIST *x, *y;
{
  if (x == NULL || y == NULL) {
    if (x == y) return(TRU);
  }
  else if (type(x) == SATOM && type(y) == SATOM && car(x) == car(y))
    return(TRU);
  return(NULL);
}

LIST *atom(x)
  register LIST *x;
{
  register int typ;

  if (x == NULL || (typ = type(x)) == IATOM || typ == RATOM || typ == SATOM)
    return(TRU);
  return(NULL);
}

	   /* logical connectives - and, or, not */

LIST *_and(x)
  register LIST *x;
{
  register LIST *p;
  for (p = cdr(x);  p != NULL;  p = cdr(p))
    if (eval(car(p)) == NULL)  return(NULL);
  return(TRU);
}

LIST *_or(x)
  LIST *x;
{
  register LIST *p;
  for (p = cdr(x);  p != NULL;  p = cdr(p))
    if(eval(car(p)) != NULL)  return(TRU);
  return(NULL);
}

LIST *_not(x)
  LIST *x;
{
  return (eval(cdr(x)) == NULL) ? TRU : NULL;
}


	       /* other primitives */

LIST *_list(x)
  LIST *x;
{
  LIST *res, *p;

  for (res = NULL, p = cdr(x);	p != NULL;  p = cdr(p))
    res = cons(res, car(p));
  return(res);
}


var_to_user(p)
  register LIST *p;
{
  if (p != NULL)
    if (type(p) == VARI) {
      if (type(car(p)) == FUSER)
	rplact(p, FUSER);
    }
    else if (type(p) == LST) {
      var_to_user(car(p));   var_to_user(cdr(p));
    }
}

var_to_atom(p)
  register LIST *p;
{
  register int t;

  if (p != NULL)
    if ((t = type(p)) != LST && !isfunc(t) || t == FUSER)
      rplact(p, SATOM);
    else {
      var_to_atom(car(p));   var_to_atom(cdr(p));
    }
}

/* find_labels - change the type of all labels in a PROG to LABL */
find_labels(p)
  LIST *p;
{
  for ( ;  p != NULL;  p = cdr(p))
    if (type(car(p)) == VARI) {
      rplact(car(p), LABL);	      /* change the type to LABL */
      rplacd(car(car(p)), cdr(p));    /* label points to next statement */
    }
}



/************************************************************/
/*		      garbage collection		    */
/************************************************************/

/* marktree - recursively marks all used items in a list */
marktree(p)
  register LIST *p;
{
  if (p != NULL) {
    if (type(p) == LST) {
      marktree(car(p));   marktree(cdr(p));
    }
    p->gcbit = USED;
  }
}


/*********************** storage allocator *****************/

char *emalloc(size)
  int size;
{
  char *s, *malloc();

  if ((s = malloc(size)) == NULL) {
    fprintf(stderr, "OUT OF MEMORY!!!\n");
    exit();
  }
  return (s);
}


/* routine to load the library of lisp functions in */
load_library()
{
  FILE *fopen();

  if ((fd = fopen("lisplib", "r")) != NULL) {
    interpret();
    fclose(fd);
  }
  fd = stdin;
}


/* isfunc - returns YES if type t is a user-function or a lisp primitive */
isfunc(t)
  register int t;
{
  return (t==FUSER || t==ADD1 || t==SUB1 || t==PLUS || t==DIFF || t==TIMES ||
	  t==QUOTIENT || t==LESSP || t==GREATERP || t==ZEROP || t==NUMBERP ||
	  t==FCAR || t==FCDR || t==FCONS || t==FREAD || t==PRINT || t==FNOT||
	  t==FAND || t==FOR  || t==FEVAL || t==FEQ || t==FATOM);
}


debug(p)
  LIST *p;
{
  printf("DEBUG ---\n");  debug2(p);  putchar('\n');
}

debug2(p)
  LIST *p;
{
  int t;

  if (p != NULL) {
    if ((t = type(p)) == LST) {
      putchar('(');  debug2(car(p));  debug2(cdr(p)); putchar(')');
    }
    else if (t == RATOM) printf("RATOM %f ", p->u.num);
    else if (t == IATOM) printf("IATOM %d ", (int) p->u.num);
    else if (t == SATOM) printf("SATOM %s ", getname(car(p)));
    else printf("FUNC %d ", type(p));
  }
}

