
/* Initial C code */
%{
#include "y.tab.h"
#include "symtab.h"
%}


ID [a-zA-Z][a-zA-Z0-9_]*


%%
[ \t]+ ;

\-?[0-9]+ |
\-?[0-9]+\. |
\-?[0-9]+\.[0-9]+ |
\-?\.[0-9]+ { yylval.val = atof(yytext);return DOUBLE;}

"//".* { return COMMENT;}

"light" {return LIGHT;}
"constants" {return CONSTANTS;}
"save_coord_system" {return SAVE_COORDS;}
"camera" {return CAMERA;}
"ambient" {return AMBIENT;}

"torus" { return TORUS;}
"sphere" { return SPHERE;}
"box" {return BOX;}
"line" {return LINE;}
"mesh" {return MESH;}

"set" {return SET;}
"move" {return MOVE;}
"scale" {return SCALE;}
"rotate" {return ROTATE;}
"basename" {return BASENAME;}
"save_knobs" {return SAVE_KNOBS;}
"tween" {return TWEEN;}
"frames" {return FRAMES;}
"vary" {return VARY;}

"push" {return PUSH;}
"pop" {return POP;}
"save" {return SAVE;}

"shading" {return SHADING;}

phong|flat|goroud|wireframe {
strncpy(yylval.string, yytext, MAX_NAME); yylval.string[MAX_NAME] = '\0'; return SHADING_TYPE;}

"setknobs" {return SETKNOBS;}
"focal" {return FOCAL;}
"display" {return DISPLAY;}

":" {return CO;}

[a-zA-Z][\.a-zA-Z0-9_/]* {
strncpy(yylval.string, yytext, MAX_NAME); yylval.string[MAX_NAME] = '\0'; return STRING;}


%%
