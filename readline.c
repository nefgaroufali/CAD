#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "readline.h" 

#define NRM  "\x1B[0m"		// Normal Color
#define RED  "\x1B[31m"		// Red Color
#define GRN  "\x1B[32m"		// Green Color
#define YEL  "\x1B[33m"		// Yellow Color
#define BLU  "\x1B[34m"		// Blue Color
#define MAG  "\x1B[35m"		// Magenta Color
#define CYN  "\x1B[36m"		// Cyan Color
#define WHT  "\x1B[37m"	    // White Color

char *commands_list[]= {
	// my commands //
	"ls", "less",
	// standard tcl command for TCL8.6//
	"after", "errorInfo", "load", "re_syntax", "tcl_startOfNextWord",
	"append","eval","lrange","read","tcl_startOfPreviousWord",
	"apply","exec","lrepeat","refchan","tcl_traceCompile",
	"argc","exit","lreplace","regexp","tcl_traceExec",
	"argv","expr","lreverse","registry","tcl_version",
	"argv0","fblocked","lsearch","regsub","tcl_wordBreakAfter",
	"array","fconfigure","lset","rename","tcl_wordBreakBefore",
	"auto_execok","fcopy","lsort","return","tcl_wordchars",
	"auto_import","file","mathfunc","safe","tcltest",
	"auto_load","fileevent","mathop","scan","tell",
	"auto_mkindex","filename","memory","seek","throw",
	"auto_path","flush","msgcat","self","time",
	"auto_qualify","for","my","set","tm",
	"auto_reset","foreach","namespace","socket","trace",
	"bgerror","format","next","source","transchan",
	"binary","gets","nextto","split","try",
	"break","glob","oo::class","string","unknown",
	"catch","global","oo::copy","subst","unload",
	"cd","history","oo::define","switch","unset",
	"chan","http","oo::objdefine","tailcall","update",
	"clock","if","oo::object","Tcl","uplevel",
	"close","incr","open","tcl::prefix","upvar",
	"concat","info","package","tcl_endOfWord","variable",
	"continue","interp","parray","tcl_findLibrary","vwait",
	"coroutine","join","pid","tcl_interactive","while",
	"dde","lappend","pkg::create","tcl_library","yield",
	"dict","lassign","pkg_mkIndex","tcl_nonwordchars","yieldto",
	"encoding","lindex","platform","tcl_patchLevel","zlib",
	NULL
};


// complete commands with target for completion text bounded //
// by start and end  //
char **custom_completer(const char *text, int start, int end)
{
	char **match;

	match = (char **)NULL;

	// detect if the word to be completed is a command (word at start of line)
	// or it is the name of a file in current directory 
	if (!start) {
		match = rl_completion_matches(text, tab_generator);
	}
	
	// if null then call default readline //
	return match;
}

// generate command for tab completetion //
char *tab_generator(const char *text, int state)
{
	// index is static in order to keep state //
	static int index_list;
	// input text length //
	static int length;

	char *name = NULL;

	// initialize word, if this is a new word to complete, and //
	// set index to 0 //
	if (!state) {
		index_list = 0;
		length = strlen(text);
	}

	// match name from command list until NULL //
	while( (name = commands_list[index_list]) )
	{
		index_list++;

		// if match //
		if ( strncmp(name, text, length) == 0 ) 
		{
			// return the matched string //
			return strdup(name);
		}
	}

	return NULL;
}
