#include <stdio.h>
#include "parser.h"

#define eof 9
#define epsilon 10
// name of nonterminal
const char *NONTERMINAL_NAME[6] = {
"goal",
"expr",
"expr'",
"term",
"term'",
"factor",
};
// ll(1) parser table
const int LL_PARSE_TABLE[6][11] = {
    {0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 2, 0, 2, 2, 0, 0},
    {0, 3, 4, 0, 0, 0, 5, 0, 0, 5, 5},
    {0, 0, 0, 0, 0, 8, 0, 8, 8, 0, 0},
    {0, 0, 0, 9, 10, 0, 0, 0, 0, 0, 11},
    {0, 0, 0, 0, 0, 12, 0, 13, 14, 0, 0}
};
// begin index of nonterminal
const int non_index = 11;
// production table
const int productions[14][3] = {
{},
    {12}, 
    {14, 13}, 
    {1, 14, 13}, {2, 14, 13}, {10},
    {14, 3, 16}, {14, 4, 16}, {16},
    {3, 16, 15}, {4, 16, 15}, {10},
    {5, 12, 6}, {7}, {8},
};
// synchronized set
const int SYNCHRONIZED_SET[6][11] = {
{5 ,7 ,9 ,8},
{6 ,7 ,5 ,9 ,8},
{2 ,1 ,6 ,9 ,10},
{1 ,2 ,3 ,4 ,6 ,5 ,7 ,9 ,8},
{3 ,4 ,10},
{1 ,2 ,3 ,4 ,6 ,7 ,5 ,9 ,8},
};
