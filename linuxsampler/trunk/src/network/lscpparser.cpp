/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CHAR = 258,
     DOTNUM = 259,
     NUMBER = 260,
     STRINGVAL = 261,
     SP = 262,
     LF = 263,
     CR = 264,
     HASH = 265,
     EQ = 266,
     ADD = 267,
     GET = 268,
     CREATE = 269,
     DESTROY = 270,
     LIST = 271,
     LOAD = 272,
     NON_MODAL = 273,
     REMOVE = 274,
     SET = 275,
     SUBSCRIBE = 276,
     UNSUBSCRIBE = 277,
     RESET = 278,
     QUIT = 279,
     CHANNEL = 280,
     NOTIFICATION = 281,
     AVAILABLE_ENGINES = 282,
     AVAILABLE_AUDIO_OUTPUT_DRIVERS = 283,
     CHANNELS = 284,
     INFO = 285,
     BUFFER_FILL = 286,
     STREAM_COUNT = 287,
     VOICE_COUNT = 288,
     INSTRUMENT = 289,
     ENGINE = 290,
     AUDIO_OUTPUT_CHANNEL = 291,
     AUDIO_OUTPUT_CHANNEL_PARAMETER = 292,
     AUDIO_OUTPUT_DEVICE = 293,
     AUDIO_OUTPUT_DEVICES = 294,
     AUDIO_OUTPUT_DEVICE_PARAMETER = 295,
     AUDIO_OUTPUT_DRIVER = 296,
     AUDIO_OUTPUT_DRIVER_PARAMETER = 297,
     AUDIO_OUTPUT_TYPE = 298,
     MIDI_INPUT = 299,
     MIDI_INPUT_TYPE = 300,
     MIDI_INPUT_PORT = 301,
     MIDI_INPUT_CHANNEL = 302,
     VOLUME = 303,
     MIDI_INPUT_DRIVER = 304,
     MIDI_INPUT_DRIVER_PARAMETER = 305,
     AVAILABLE_MIDI_INPUT_DRIVERS = 306,
     MIDI_INPUT_DEVICE = 307,
     MIDI_INPUT_DEVICES = 308,
     MIDI_INPUT_DEVICE_PARAMETER = 309,
     MIDI_INPUT_PORT_PARAMETER = 310,
     BYTES = 311,
     PERCENTAGE = 312,
     MISCELLANEOUS = 313
   };
#endif
#define CHAR 258
#define DOTNUM 259
#define NUMBER 260
#define STRINGVAL 261
#define SP 262
#define LF 263
#define CR 264
#define HASH 265
#define EQ 266
#define ADD 267
#define GET 268
#define CREATE 269
#define DESTROY 270
#define LIST 271
#define LOAD 272
#define NON_MODAL 273
#define REMOVE 274
#define SET 275
#define SUBSCRIBE 276
#define UNSUBSCRIBE 277
#define RESET 278
#define QUIT 279
#define CHANNEL 280
#define NOTIFICATION 281
#define AVAILABLE_ENGINES 282
#define AVAILABLE_AUDIO_OUTPUT_DRIVERS 283
#define CHANNELS 284
#define INFO 285
#define BUFFER_FILL 286
#define STREAM_COUNT 287
#define VOICE_COUNT 288
#define INSTRUMENT 289
#define ENGINE 290
#define AUDIO_OUTPUT_CHANNEL 291
#define AUDIO_OUTPUT_CHANNEL_PARAMETER 292
#define AUDIO_OUTPUT_DEVICE 293
#define AUDIO_OUTPUT_DEVICES 294
#define AUDIO_OUTPUT_DEVICE_PARAMETER 295
#define AUDIO_OUTPUT_DRIVER 296
#define AUDIO_OUTPUT_DRIVER_PARAMETER 297
#define AUDIO_OUTPUT_TYPE 298
#define MIDI_INPUT 299
#define MIDI_INPUT_TYPE 300
#define MIDI_INPUT_PORT 301
#define MIDI_INPUT_CHANNEL 302
#define VOLUME 303
#define MIDI_INPUT_DRIVER 304
#define MIDI_INPUT_DRIVER_PARAMETER 305
#define AVAILABLE_MIDI_INPUT_DRIVERS 306
#define MIDI_INPUT_DEVICE 307
#define MIDI_INPUT_DEVICES 308
#define MIDI_INPUT_DEVICE_PARAMETER 309
#define MIDI_INPUT_PORT_PARAMETER 310
#define BYTES 311
#define PERCENTAGE 312
#define MISCELLANEOUS 313




/* Copy the first part of user declarations.  */
#line 23 "lscp.y"


#include "lscpparser.h"
#include "lscpserver.h"

// as we need an reentrant scanner, we have to pass the pointer to the scanner with each yylex() call
#define YYLEX_PARAM ((yyparse_param_t*) yyparse_param)->pScanner

// to save us typing work in the rules action definitions
#define LSCPSERVER ((yyparse_param_t*) yyparse_param)->pServer

// clears input buffer and restarts scanner.
void restart(yyparse_param_t* pparam, int& yychar);
#define RESTART restart((yyparse_param_t*) YYPARSE_PARAM, yychar)

// external reference to the main scanner function yylex()
extern YY_DECL;

// external reference to restart the lex scanner
extern void yyrestart(FILE* input_file, yyscan_t yyscanner);

// we provide our own version of yyerror() so we don't have to link against the yacc library
void yyerror(const char* s);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 228 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  30
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   293

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  32
/* YYNRULES -- Number of rules. */
#define YYNRULES  108
/* YYNRULES -- Number of states. */
#define YYNSTATES  307

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     9,    14,    15,    17,    19,    21,
      23,    26,    29,    32,    35,    39,    43,    47,    51,    55,
      59,    65,    69,    73,    77,    83,    85,    87,    89,    91,
      93,    95,    97,    99,   101,   103,   105,   107,   109,   111,
     113,   119,   127,   137,   139,   145,   153,   163,   165,   167,
     173,   179,   187,   195,   205,   207,   213,   221,   227,   233,
     239,   247,   257,   265,   275,   279,   285,   289,   295,   299,
     303,   307,   311,   315,   321,   329,   335,   345,   351,   357,
     363,   369,   375,   379,   385,   387,   389,   391,   393,   399,
     407,   411,   413,   415,   417,   419,   421,   423,   425,   427,
     429,   431,   433,   435,   437,   439,   441,   443,   445
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      60,     0,    -1,    61,    -1,    60,     8,    61,    -1,    60,
       9,     8,    61,    -1,    -1,    62,    -1,    63,    -1,     1,
      -1,    10,    -1,    62,    10,    -1,    62,     7,    -1,    62,
       5,    -1,    62,    90,    -1,    12,     7,    25,    -1,    13,
       7,    66,    -1,    14,     7,    68,    -1,    15,     7,    69,
      -1,    16,     7,    74,    -1,    17,     7,    70,    -1,    19,
       7,    25,     7,    85,    -1,    20,     7,    67,    -1,    21,
       7,    64,    -1,    22,     7,    65,    -1,    23,     7,    25,
       7,    85,    -1,    24,    -1,    29,    -1,    33,    -1,    32,
      -1,    31,    -1,    30,    -1,    58,    -1,    29,    -1,    33,
      -1,    32,    -1,    31,    -1,    30,    -1,    58,    -1,    27,
      -1,    51,    -1,    49,     7,    30,     7,    90,    -1,    50,
       7,    30,     7,    90,     7,    90,    -1,    50,     7,    30,
       7,    90,     7,    90,     7,    72,    -1,    28,    -1,    41,
       7,    30,     7,    90,    -1,    42,     7,    30,     7,    90,
       7,    90,    -1,    42,     7,    30,     7,    90,     7,    90,
       7,    72,    -1,    39,    -1,    53,    -1,    38,     7,    30,
       7,     5,    -1,    52,     7,    30,     7,     5,    -1,    46,
       7,    30,     7,     5,     7,     5,    -1,    36,     7,    30,
       7,     5,     7,     5,    -1,    37,     7,    30,     7,     5,
       7,     5,     7,    90,    -1,    29,    -1,    25,     7,    30,
       7,    85,    -1,    25,     7,    31,     7,    73,     7,    85,
      -1,    25,     7,    32,     7,    85,    -1,    25,     7,    33,
       7,    85,    -1,    35,     7,    30,     7,    87,    -1,    40,
       7,     5,     7,    90,    11,    89,    -1,    37,     7,     5,
       7,     5,     7,    90,    11,    89,    -1,    54,     7,     5,
       7,    90,    11,    89,    -1,    55,     7,     5,     7,     5,
       7,    90,    11,    89,    -1,    25,     7,    71,    -1,    38,
       7,    90,     7,    72,    -1,    38,     7,    90,    -1,    52,
       7,    90,     7,    72,    -1,    52,     7,    90,    -1,    38,
       7,     5,    -1,    52,     7,     5,    -1,    34,     7,    75,
      -1,    35,     7,    76,    -1,    38,     7,    85,     7,    77,
      -1,    36,     7,    85,     7,    78,     7,    78,    -1,    43,
       7,    85,     7,    79,    -1,    44,     7,    85,     7,    80,
       7,    81,     7,    82,    -1,    52,     7,    85,     7,    80,
      -1,    46,     7,    85,     7,    81,    -1,    47,     7,    85,
       7,    82,    -1,    45,     7,    85,     7,    83,    -1,    48,
       7,    85,     7,    84,    -1,    90,    11,    89,    -1,    72,
       7,    90,    11,    89,    -1,    56,    -1,    57,    -1,    39,
      -1,    53,    -1,    88,     7,    86,     7,    85,    -1,    18,
       7,    88,     7,    86,     7,    85,    -1,    87,     7,    85,
      -1,     5,    -1,     5,    -1,    90,    -1,     5,    -1,     5,
      -1,     5,    -1,    90,    -1,     4,    -1,     5,    -1,     5,
      -1,     5,    -1,    90,    -1,     6,    -1,     6,    -1,     5,
      -1,     4,    -1,     3,    -1,    90,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    78,    78,    79,    80,    83,    84,    85,    86,    89,
      90,    91,    92,    93,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   110,   111,   112,   113,
     114,   115,   118,   119,   120,   121,   122,   123,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     150,   151,   152,   153,   154,   157,   158,   159,   160,   163,
     164,   167,   168,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   182,   183,   186,   187,   190,   191,   194,   195,
     198,   201,   204,   207,   210,   213,   216,   219,   222,   223,
     226,   229,   232,   235,   238,   239,   240,   243,   244
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CHAR", "DOTNUM", "NUMBER", "STRINGVAL", 
  "SP", "LF", "CR", "HASH", "EQ", "ADD", "GET", "CREATE", "DESTROY", 
  "LIST", "LOAD", "NON_MODAL", "REMOVE", "SET", "SUBSCRIBE", 
  "UNSUBSCRIBE", "RESET", "QUIT", "CHANNEL", "NOTIFICATION", 
  "AVAILABLE_ENGINES", "AVAILABLE_AUDIO_OUTPUT_DRIVERS", "CHANNELS", 
  "INFO", "BUFFER_FILL", "STREAM_COUNT", "VOICE_COUNT", "INSTRUMENT", 
  "ENGINE", "AUDIO_OUTPUT_CHANNEL", "AUDIO_OUTPUT_CHANNEL_PARAMETER", 
  "AUDIO_OUTPUT_DEVICE", "AUDIO_OUTPUT_DEVICES", 
  "AUDIO_OUTPUT_DEVICE_PARAMETER", "AUDIO_OUTPUT_DRIVER", 
  "AUDIO_OUTPUT_DRIVER_PARAMETER", "AUDIO_OUTPUT_TYPE", "MIDI_INPUT", 
  "MIDI_INPUT_TYPE", "MIDI_INPUT_PORT", "MIDI_INPUT_CHANNEL", "VOLUME", 
  "MIDI_INPUT_DRIVER", "MIDI_INPUT_DRIVER_PARAMETER", 
  "AVAILABLE_MIDI_INPUT_DRIVERS", "MIDI_INPUT_DEVICE", 
  "MIDI_INPUT_DEVICES", "MIDI_INPUT_DEVICE_PARAMETER", 
  "MIDI_INPUT_PORT_PARAMETER", "BYTES", "PERCENTAGE", "MISCELLANEOUS", 
  "$accept", "input", "line", "comment", "command", "subscribe_event", 
  "unsubscribe_event", "get_instruction", "set_instruction", 
  "create_instruction", "destroy_instruction", "load_instruction", 
  "set_chan_instruction", "key_val_list", "buffer_size_type", 
  "list_instruction", "load_instr_args", "load_engine_args", 
  "audio_output_device", "audio_output_channel", "audio_output_type", 
  "midi_input_device", "midi_input_port", "midi_input_channel", 
  "midi_input_type", "volume", "sampler_channel", "instrument_index", 
  "engine_name", "filename", "param_val", "string", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    59,    60,    60,    60,    61,    61,    61,    61,    62,
      62,    62,    62,    62,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    64,    64,    64,    64,
      64,    64,    65,    65,    65,    65,    65,    65,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      67,    67,    67,    67,    67,    68,    68,    68,    68,    69,
      69,    70,    70,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    72,    72,    73,    73,    74,    74,    75,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    84,
      85,    86,    87,    88,    89,    89,    89,    90,    90
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     4,     0,     1,     1,     1,     1,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       5,     3,     3,     3,     5,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       5,     7,     9,     1,     5,     7,     9,     1,     1,     5,
       5,     7,     7,     9,     1,     5,     7,     5,     5,     5,
       7,     9,     7,     9,     3,     5,     3,     5,     3,     3,
       3,     3,     3,     5,     7,     5,     9,     5,     5,     5,
       5,     5,     3,     5,     1,     1,     1,     1,     5,     7,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     8,     9,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,     0,     2,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     0,     0,   107,    12,    11,    10,    13,    14,     0,
      38,    43,    54,     0,     0,     0,     0,    47,     0,     0,
       0,     0,     0,    39,     0,    48,    15,     0,     0,    16,
       0,     0,    17,    86,    87,    18,     0,     0,    19,     0,
       0,     0,     0,     0,     0,    21,    26,    30,    29,    28,
      27,    31,    22,    32,    36,    35,    34,    33,    37,    23,
       0,     3,     0,   108,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    66,    68,    69,    70,   103,     0,    71,
       0,    72,     0,   102,   100,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    64,     0,     0,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55,    84,    85,     0,    57,    58,    59,
       0,     0,    49,    44,     0,     0,    40,     0,    50,    65,
       0,    67,     0,   101,     0,    90,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    56,    52,     0,    45,    51,    41,     0,   106,
     105,   104,    82,     0,    88,    92,     0,    91,    73,    75,
      93,    94,     0,    80,    97,    95,    78,    96,    79,    98,
      99,    81,    77,     0,    60,    62,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    53,    46,    42,    83,
      89,    74,     0,    61,    63,     0,    76
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    15,    16,    17,    18,    82,    89,    56,    75,    59,
      62,    68,   155,   209,   196,    65,   139,   141,   268,   266,
     269,   272,   276,   278,   273,   281,   145,   214,   142,   140,
     262,   210
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -245
static const short yypact[] =
{
     168,  -245,  -245,    30,    51,    68,   101,   105,   109,   129,
     130,   131,   132,   133,  -245,    34,  -245,    52,  -245,    16,
      82,   -21,    -8,    -1,   -27,    55,   -22,    -9,    -4,    70,
    -245,   168,    90,  -245,  -245,  -245,  -245,    93,  -245,   134,
    -245,  -245,  -245,   135,   136,   137,   138,  -245,   139,   140,
     142,   143,   144,  -245,   147,  -245,  -245,   150,   151,  -245,
     152,   153,  -245,  -245,  -245,  -245,   154,   155,  -245,   156,
     157,   158,   159,   160,   163,  -245,  -245,  -245,  -245,  -245,
    -245,  -245,  -245,  -245,  -245,  -245,  -245,  -245,  -245,  -245,
     164,  -245,   168,  -245,    73,   149,   165,   166,   167,   169,
     170,   171,   172,   173,   174,   183,   183,   188,   189,    -2,
     183,   193,    38,   200,   201,   202,   203,   193,  -245,   204,
     205,   206,   207,   209,   212,   213,   214,   215,   216,   218,
     220,   221,   222,     6,     7,  -245,  -245,  -245,   223,  -245,
     225,  -245,   226,    93,  -245,  -245,   228,   229,   230,   231,
     232,   233,   234,   235,   236,  -245,   237,   238,   239,   240,
    -245,   193,    21,   193,   193,   183,   243,   244,   245,   183,
     183,   246,   183,   183,   247,   183,   183,   248,   250,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   251,
     183,   183,   252,  -245,  -245,  -245,   253,  -245,  -245,  -245,
     254,   255,  -245,    93,    32,   256,    93,    33,  -245,   257,
       8,   257,   258,  -245,   259,  -245,   260,   261,   262,   264,
     265,   266,   267,   268,   269,   270,    50,    76,   271,   193,
     274,   275,   183,   276,   183,   183,   121,   250,   193,   277,
     278,   183,   279,   183,   280,   281,   125,   279,   183,   121,
     121,   183,  -245,  -245,   282,    53,  -245,    94,    88,  -245,
    -245,  -245,  -245,   283,  -245,  -245,   284,  -245,  -245,  -245,
      93,  -245,   285,  -245,    93,  -245,  -245,  -245,  -245,  -245,
    -245,  -245,  -245,    89,  -245,  -245,    91,   183,   183,   183,
     121,   193,   277,   280,   121,   121,    93,   257,   257,  -245,
    -245,  -245,   286,  -245,  -245,   281,  -245
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -245,  -245,   -19,  -245,  -245,  -245,  -245,  -245,  -245,  -245,
    -245,  -245,  -245,  -174,  -245,  -245,  -245,  -245,  -245,  -120,
    -245,   -38,   -83,   -52,  -245,  -245,  -116,    22,   122,    81,
    -244,   -17
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -6
static const short yytable[] =
{
      37,   160,   211,    70,   137,   284,   285,    66,    67,    93,
      93,    93,    91,   175,   176,    71,   138,    57,    72,   236,
      76,    77,    78,    79,    80,    83,    84,    85,    86,    87,
      60,    58,    73,    74,    30,    93,    93,    19,    63,   232,
     234,    38,    31,    32,    61,   193,   299,   197,   198,    81,
     303,   304,    64,    93,    88,    33,    93,    34,    20,    35,
     288,   249,    36,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   118,   146,    21,   147,   194,   195,    93,
      69,   148,   149,   150,   151,   152,   153,   250,   133,   134,
     154,    93,    93,   143,    93,    90,    93,    93,    92,   290,
     294,   289,   295,   119,   120,   121,   122,    39,    22,    40,
      41,    42,    23,   252,   297,   298,    24,    43,    44,    45,
      46,    47,   264,    48,    49,   259,   260,   261,    50,   279,
     280,    51,    52,    53,    54,    55,    25,    26,    27,    28,
      29,    94,    95,    96,    97,    98,    99,   100,   143,   101,
     102,   103,   203,   204,   104,   206,   207,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    -5,     1,
     116,   117,   301,   226,   227,   300,    -5,    -5,     2,   123,
       3,     4,     5,     6,     7,     8,    33,     9,    10,    11,
      12,    13,    14,   135,   136,   124,   125,   126,   144,   127,
     128,   129,   130,   131,   132,   156,   157,   158,   159,   282,
     302,   161,   162,   163,   164,   255,   165,   257,   258,   166,
     167,   168,   169,   170,   270,   171,   274,   172,   173,   174,
     177,   283,   178,   179,   286,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   200,   201,
     202,   205,   208,   306,   137,   213,   225,   228,   212,   263,
     229,   230,   231,   233,   235,   237,   238,   239,   240,   241,
     296,   242,   243,   244,   245,   246,   247,   248,   251,   253,
     254,   256,   265,   267,   271,   275,   277,   199,     0,   287,
     291,   292,   293,   305
};

static const short yycheck[] =
{
      17,   117,   176,    25,     6,   249,   250,    34,    35,     3,
       3,     3,    31,     7,     7,    37,    18,    38,    40,    11,
      29,    30,    31,    32,    33,    29,    30,    31,    32,    33,
      38,    52,    54,    55,     0,     3,     3,     7,    39,     7,
       7,    25,     8,     9,    52,   161,   290,   163,   164,    58,
     294,   295,    53,     3,    58,     3,     3,     5,     7,     7,
       7,    11,    10,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,    92,    36,     7,    38,    56,    57,     3,
      25,    43,    44,    45,    46,    47,    48,    11,   105,   106,
      52,     3,     3,   110,     3,    25,     3,     3,     8,    11,
      11,     7,    11,    30,    31,    32,    33,    25,     7,    27,
      28,    29,     7,   229,   288,   289,     7,    35,    36,    37,
      38,    39,   238,    41,    42,     4,     5,     6,    46,     4,
       5,    49,    50,    51,    52,    53,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,   165,     7,
       7,     7,   169,   170,     7,   172,   173,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     0,     1,
       7,     7,   292,   190,   191,   291,     8,     9,    10,    30,
      12,    13,    14,    15,    16,    17,     3,    19,    20,    21,
      22,    23,    24,     5,     5,    30,    30,    30,     5,    30,
      30,    30,    30,    30,    30,     5,     5,     5,     5,   247,
     293,     7,     7,     7,     7,   232,     7,   234,   235,     7,
       7,     7,     7,     7,   241,     7,   243,     7,     7,     7,
       7,   248,     7,     7,   251,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     5,     5,
       5,     5,     5,   305,     6,     5,     5,     5,   177,   237,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     287,     7,     7,     7,     7,     7,     7,     7,     7,     5,
       5,     5,     5,     5,     5,     5,     5,   165,    -1,     7,
       7,     7,     7,     7
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,    10,    12,    13,    14,    15,    16,    17,    19,
      20,    21,    22,    23,    24,    60,    61,    62,    63,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       0,     8,     9,     3,     5,     7,    10,    90,    25,    25,
      27,    28,    29,    35,    36,    37,    38,    39,    41,    42,
      46,    49,    50,    51,    52,    53,    66,    38,    52,    68,
      38,    52,    69,    39,    53,    74,    34,    35,    70,    25,
      25,    37,    40,    54,    55,    67,    29,    30,    31,    32,
      33,    58,    64,    29,    30,    31,    32,    33,    58,    65,
      25,    61,     8,     3,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,    61,    30,
      31,    32,    33,    30,    30,    30,    30,    30,    30,    30,
      30,    30,    30,    90,    90,     5,     5,     6,    18,    75,
      88,    76,    87,    90,     5,    85,    36,    38,    43,    44,
      45,    46,    47,    48,    52,    71,     5,     5,     5,     5,
      85,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,    85,    56,    57,    73,    85,    85,    87,
       5,     5,     5,    90,    90,     5,    90,    90,     5,    72,
      90,    72,    88,     5,    86,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,     5,    90,    90,     5,     7,
       7,     7,     7,     7,     7,     7,    11,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,    11,
      11,     7,    85,     5,     5,    90,     5,    90,    90,     4,
       5,     6,    89,    86,    85,     5,    78,     5,    77,    79,
      90,     5,    80,    83,    90,     5,    81,     5,    82,     4,
       5,    84,    80,    90,    89,    89,    90,     7,     7,     7,
      11,     7,     7,     7,    11,    11,    90,    72,    72,    89,
      85,    78,    81,    89,    89,     7,    82
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 7:
#line 85 "lscp.y"
    { LSCPSERVER->AnswerClient(yyvsp[0].String); }
    break;

  case 8:
#line 86 "lscp.y"
    { LSCPSERVER->AnswerClient("Err:0:Unknown command.\r\n"); RESTART; return LSCP_SYNTAX_ERROR; }
    break;

  case 14:
#line 96 "lscp.y"
    { yyval.String = LSCPSERVER->AddChannel();                  }
    break;

  case 15:
#line 97 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 16:
#line 98 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 17:
#line 99 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 18:
#line 100 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 19:
#line 101 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 20:
#line 102 "lscp.y"
    { yyval.String = LSCPSERVER->RemoveChannel(yyvsp[0].Number);             }
    break;

  case 21:
#line 103 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 22:
#line 104 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 23:
#line 105 "lscp.y"
    { yyval.String = yyvsp[0].String;                                        }
    break;

  case 24:
#line 106 "lscp.y"
    { yyval.String = LSCPSERVER->ResetChannel(yyvsp[0].Number);              }
    break;

  case 25:
#line 107 "lscp.y"
    { LSCPSERVER->AnswerClient("Bye!\r\n"); return 0; }
    break;

  case 26:
#line 110 "lscp.y"
    { yyval.String = LSCPSERVER->SubscribeNotification(event_channels); }
    break;

  case 27:
#line 111 "lscp.y"
    { yyval.String = LSCPSERVER->SubscribeNotification(event_voice_count); }
    break;

  case 28:
#line 112 "lscp.y"
    { yyval.String = LSCPSERVER->SubscribeNotification(event_stream_count); }
    break;

  case 29:
#line 113 "lscp.y"
    { yyval.String = LSCPSERVER->SubscribeNotification(event_channel_buffer_fill); }
    break;

  case 30:
#line 114 "lscp.y"
    { yyval.String = LSCPSERVER->SubscribeNotification(event_channel_info); }
    break;

  case 31:
#line 115 "lscp.y"
    { yyval.String = LSCPSERVER->SubscribeNotification(event_misc); }
    break;

  case 32:
#line 118 "lscp.y"
    { yyval.String = LSCPSERVER->UnsubscribeNotification(event_channels); }
    break;

  case 33:
#line 119 "lscp.y"
    { yyval.String = LSCPSERVER->UnsubscribeNotification(event_voice_count); }
    break;

  case 34:
#line 120 "lscp.y"
    { yyval.String = LSCPSERVER->UnsubscribeNotification(event_stream_count); }
    break;

  case 35:
#line 121 "lscp.y"
    { yyval.String = LSCPSERVER->UnsubscribeNotification(event_channel_buffer_fill); }
    break;

  case 36:
#line 122 "lscp.y"
    { yyval.String = LSCPSERVER->UnsubscribeNotification(event_channel_info); }
    break;

  case 37:
#line 123 "lscp.y"
    { yyval.String = LSCPSERVER->UnsubscribeNotification(event_misc); }
    break;

  case 38:
#line 126 "lscp.y"
    { yyval.String = LSCPSERVER->GetAvailableEngines();                          }
    break;

  case 39:
#line 127 "lscp.y"
    { yyval.String = LSCPSERVER->GetAvailableMidiInputDrivers();               }
    break;

  case 40:
#line 128 "lscp.y"
    { yyval.String = LSCPSERVER->GetMidiInputDriverInfo(yyvsp[0].String);                   }
    break;

  case 41:
#line 129 "lscp.y"
    { yyval.String = LSCPSERVER->GetMidiInputDriverParameterInfo(yyvsp[-2].String, yyvsp[0].String);      }
    break;

  case 42:
#line 130 "lscp.y"
    { yyval.String = LSCPSERVER->GetMidiInputDriverParameterInfo(yyvsp[-4].String, yyvsp[-2].String, yyvsp[0].KeyValList);  }
    break;

  case 43:
#line 131 "lscp.y"
    { yyval.String = LSCPSERVER->GetAvailableAudioOutputDrivers();               }
    break;

  case 44:
#line 132 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputDriverInfo(yyvsp[0].String);                   }
    break;

  case 45:
#line 133 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputDriverParameterInfo(yyvsp[-2].String, yyvsp[0].String);      }
    break;

  case 46:
#line 134 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputDriverParameterInfo(yyvsp[-4].String, yyvsp[-2].String, yyvsp[0].KeyValList);  }
    break;

  case 47:
#line 135 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputDeviceCount();                    }
    break;

  case 48:
#line 136 "lscp.y"
    { yyval.String = LSCPSERVER->GetMidiInputDeviceCount();                    }
    break;

  case 49:
#line 137 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputDeviceInfo(yyvsp[0].Number);                   }
    break;

  case 50:
#line 138 "lscp.y"
    { yyval.String = LSCPSERVER->GetMidiInputDeviceInfo(yyvsp[0].Number);                   }
    break;

  case 51:
#line 139 "lscp.y"
    { yyval.String = LSCPSERVER->GetMidiInputPortInfo(yyvsp[-2].Number, yyvsp[0].Number);                   }
    break;

  case 52:
#line 140 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputChannelInfo(yyvsp[-2].Number, yyvsp[0].Number);              }
    break;

  case 53:
#line 141 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputChannelParameterInfo(yyvsp[-4].Number, yyvsp[-2].Number, yyvsp[0].String); }
    break;

  case 54:
#line 142 "lscp.y"
    { yyval.String = LSCPSERVER->GetChannels();                                  }
    break;

  case 55:
#line 143 "lscp.y"
    { yyval.String = LSCPSERVER->GetChannelInfo(yyvsp[0].Number);                             }
    break;

  case 56:
#line 144 "lscp.y"
    { yyval.String = LSCPSERVER->GetBufferFill(yyvsp[-2].FillResponse, yyvsp[0].Number);                          }
    break;

  case 57:
#line 145 "lscp.y"
    { yyval.String = LSCPSERVER->GetStreamCount(yyvsp[0].Number);                             }
    break;

  case 58:
#line 146 "lscp.y"
    { yyval.String = LSCPSERVER->GetVoiceCount(yyvsp[0].Number);                              }
    break;

  case 59:
#line 147 "lscp.y"
    { yyval.String = LSCPSERVER->GetEngineInfo(yyvsp[0].String);                              }
    break;

  case 60:
#line 150 "lscp.y"
    { yyval.String = LSCPSERVER->SetAudioOutputDeviceParameter(yyvsp[-4].Number, yyvsp[-2].String, yyvsp[0].String);      }
    break;

  case 61:
#line 151 "lscp.y"
    { yyval.String = LSCPSERVER->SetAudioOutputChannelParameter(yyvsp[-6].Number, yyvsp[-4].Number, yyvsp[-2].String, yyvsp[0].String); }
    break;

  case 62:
#line 152 "lscp.y"
    { yyval.String = LSCPSERVER->SetMidiInputDeviceParameter(yyvsp[-4].Number, yyvsp[-2].String, yyvsp[0].String); }
    break;

  case 63:
#line 153 "lscp.y"
    { yyval.String = LSCPSERVER->SetMidiInputPortParameter(yyvsp[-6].Number, yyvsp[-4].Number, yyvsp[-2].String, yyvsp[0].String); }
    break;

  case 64:
#line 154 "lscp.y"
    { yyval.String = yyvsp[0].String;                                                         }
    break;

  case 65:
#line 157 "lscp.y"
    { yyval.String = LSCPSERVER->CreateAudioOutputDevice(yyvsp[-2].String,yyvsp[0].KeyValList); }
    break;

  case 66:
#line 158 "lscp.y"
    { yyval.String = LSCPSERVER->CreateAudioOutputDevice(yyvsp[0].String);    }
    break;

  case 67:
#line 159 "lscp.y"
    { yyval.String = LSCPSERVER->CreateMidiInputDevice(yyvsp[-2].String,yyvsp[0].KeyValList);   }
    break;

  case 68:
#line 160 "lscp.y"
    { yyval.String = LSCPSERVER->CreateMidiInputDevice(yyvsp[0].String);      }
    break;

  case 69:
#line 163 "lscp.y"
    { yyval.String = LSCPSERVER->DestroyAudioOutputDevice(yyvsp[0].Number); }
    break;

  case 70:
#line 164 "lscp.y"
    { yyval.String = LSCPSERVER->DestroyMidiInputDevice(yyvsp[0].Number); }
    break;

  case 71:
#line 167 "lscp.y"
    { yyval.String = yyvsp[0].String; }
    break;

  case 72:
#line 168 "lscp.y"
    { yyval.String = yyvsp[0].String; }
    break;

  case 73:
#line 171 "lscp.y"
    { yyval.String = LSCPSERVER->SetAudioOutputDevice(yyvsp[0].Number, yyvsp[-2].Number);      }
    break;

  case 74:
#line 172 "lscp.y"
    { yyval.String = LSCPSERVER->SetAudioOutputChannel(yyvsp[-2].Number, yyvsp[0].Number, yyvsp[-4].Number); }
    break;

  case 75:
#line 173 "lscp.y"
    { yyval.String = LSCPSERVER->SetAudioOutputType(yyvsp[0].String, yyvsp[-2].Number);        }
    break;

  case 76:
#line 174 "lscp.y"
    { yyval.String = LSCPSERVER->SetMIDIInput(yyvsp[-4].Number, yyvsp[-2].Number, yyvsp[0].Number, yyvsp[-6].Number);  }
    break;

  case 77:
#line 175 "lscp.y"
    { yyval.String = LSCPSERVER->SetMIDIInputDevice(yyvsp[0].Number, yyvsp[-2].Number);        }
    break;

  case 78:
#line 176 "lscp.y"
    { yyval.String = LSCPSERVER->SetMIDIInputPort(yyvsp[0].Number, yyvsp[-2].Number);          }
    break;

  case 79:
#line 177 "lscp.y"
    { yyval.String = LSCPSERVER->SetMIDIInputChannel(yyvsp[0].Number, yyvsp[-2].Number);       }
    break;

  case 80:
#line 178 "lscp.y"
    { yyval.String = LSCPSERVER->SetMIDIInputType(yyvsp[0].String, yyvsp[-2].Number);          }
    break;

  case 81:
#line 179 "lscp.y"
    { yyval.String = LSCPSERVER->SetVolume(yyvsp[0].Dotnum, yyvsp[-2].Number);                 }
    break;

  case 82:
#line 182 "lscp.y"
    { yyval.KeyValList[yyvsp[-2].String] = yyvsp[0].String;          }
    break;

  case 83:
#line 183 "lscp.y"
    { yyval.KeyValList = yyvsp[-4].KeyValList; yyval.KeyValList[yyvsp[-2].String] = yyvsp[0].String; }
    break;

  case 84:
#line 186 "lscp.y"
    { yyval.FillResponse = fill_response_bytes;      }
    break;

  case 85:
#line 187 "lscp.y"
    { yyval.FillResponse = fill_response_percentage; }
    break;

  case 86:
#line 190 "lscp.y"
    { yyval.String = LSCPSERVER->GetAudioOutputDevices(); }
    break;

  case 87:
#line 191 "lscp.y"
    { yyval.String = LSCPSERVER->GetMidiInputDevices(); }
    break;

  case 88:
#line 194 "lscp.y"
    { yyval.String = LSCPSERVER->LoadInstrument(yyvsp[-4].String, yyvsp[-2].Number, yyvsp[0].Number); }
    break;

  case 89:
#line 195 "lscp.y"
    { yyval.String = LSCPSERVER->LoadInstrument(yyvsp[-4].String, yyvsp[-2].Number, yyvsp[0].Number, true); }
    break;

  case 90:
#line 198 "lscp.y"
    { yyval.String = LSCPSERVER->LoadEngine(yyvsp[-2].String, yyvsp[0].Number); }
    break;

  case 99:
#line 223 "lscp.y"
    { yyval.Dotnum = yyvsp[0].Number; }
    break;

  case 104:
#line 238 "lscp.y"
    { yyval.String = yyvsp[0].String;                                             }
    break;

  case 105:
#line 239 "lscp.y"
    { std::stringstream ss; ss << yyvsp[0].Number; yyval.String = ss.str();       }
    break;

  case 106:
#line 240 "lscp.y"
    { std::stringstream ss; ss << yyvsp[0].Dotnum; yyval.String = ss.str();       }
    break;

  case 107:
#line 243 "lscp.y"
    { std::string s; s = yyvsp[0].Char; yyval.String = s; }
    break;

  case 108:
#line 244 "lscp.y"
    { yyval.String = yyvsp[-1].String + yyvsp[0].Char;                  }
    break;


    }

/* Line 991 of yacc.c.  */
#line 1784 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__) \
    && !defined __cplusplus
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 247 "lscp.y"


/**
 * Will be called when an error occured (usually syntax error).
 */
void yyerror(const char* s) {
    dmsg(2,("LSCPParser: %s\n", s));
}

/**
 * Clears input buffer and restarts scanner.
 */
void restart(yyparse_param_t* pparam, int& yychar) {
    // restart scanner
    yyrestart(stdin, pparam->pScanner);
    // flush input buffer
    static char buf[1024];
    while(recv(hSession, buf, 1024, MSG_DONTWAIT) > 0);
    // reset lookahead symbol
    yyclearin;
}


