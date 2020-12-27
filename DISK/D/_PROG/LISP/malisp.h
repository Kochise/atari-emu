#define FALSE	0
#define TRUE	1

/* input types */

#define INQUOTE 1
#define LPAREN	2
#define RPAREN	3
#define LETTER	4
#define DIGIT	5


/* token types */

#define IATOM	6
#define RATOM	7
#define SATOM	8
#define FUNC	9
#define LST	10
#define VARI	11
#define QUOTE	12
#define NILL	13
#define T	14
#define COND	15
#define DEFUN	16
#define FCAR	17
#define FCDR	18
#define FCONS	19
#define FEQ	20
#define FATOM	21
#define FQUOTE	22
#define FSETQ	23
#define FUSER	24
#define PLUS	25
#define DIFF	26
#define TIMES	27
#define QUOTIENT 28
#define ADD1	29
#define SUB1	30
#define ZEROP	31
#define NUMBERP 32
#define GREATERP 33
#define LESSP	34
#define PRINT	35
#define NUL	36
#define FUNCALL 37
#define PROG	38
#define GO	39
#define RETRN	40
#define LABL	41
#define FREAD	42
#define FREPLACA 43
#define FREPLACD 44
#define FEVAL	45
#define FAPPLY	46


/* for garbage collection */

#define GARBAGE 47
#define USED	48
#define RUNNING 49


/* more primitives */

#define FAND	50
#define FOR	51
#define FNOT	52
#define FLIST	53

#define ERR	-1

typedef struct LIST {
  char  gcbit;
  char  htype;
  union {
    float num;
    char  *pname;
  } u;
  struct LIST  *left;
  struct LIST  *right;
} LIST;

LIST *init();
LIST *install();
LIST *makelist();
LIST *eval();
LIST *evalcond();
LIST *evalprog();
LIST *pairargs();
LIST *evalargs();
LIST *assoc();
LIST *getvar();
LIST *arith();
LIST *lookup();
LIST *getnum();
LIST *getid();
LIST *new();
LIST *car();
LIST *cdr();
LIST *cons();
LIST *eq();
LIST *atom();
LIST *_and();
LIST *_or();
LIST *_not();
LIST *_list();

