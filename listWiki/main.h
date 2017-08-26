#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <map>

#include "version.h"

using namespace std;

//------------------------------------------------------------------
// Globals.
// Yes, I know! Don't start! Otherwise I'll use a GOTO as well!
//------------------------------------------------------------------

uint32_t lineNumber = 0;    // Input file, current line number.
ifstream *mIfs;             // Input file aka argv[1].

// A structure holding the details of something to look for
// when searching. Needed as some features - I'm looking at
// you unordered lists - can have more than one entry.

typedef vector<string>::iterator vsi;

typedef struct lookForStuff {
    vector<string> findThis;    // List of characters to look for.
    bool lookAtStartOfLine;     // Look at the start of line only, or not.
} lookForStuff;

// Define a single one, to be used.
lookForStuff lookForThis;

// A map of what parameter to look for, and all the characters we
// need to look for to find it, and whether is occurs at the start
// of the line or not.

typedef map<string, struct lookForStuff> paramMap;
typedef map<string, struct lookForStuff>::iterator paramMapIter;
//
// I got a bargain bag of '{' and '}' on eBay, thought
// I'd use them here! ;-)
paramMap lookForThese = {
    {"ACRONYM",         {{"??"},          false}},
    {"ANCHOR",          {{"--"},          false}},
    {"BLOCKQUOTE",      {{">"},           true}},
    {"BOLD",            {{"__"},          false}},
    {"CITATION",        {{"^^"},          false}},
    {"CODEBLOCK",       {{" "},           true}},
    {"DEFINITIONLIST",  {{";"},           true}},
    {"FORCEDLINEFEED",  {{"%%%"},         false}},
    {"HEADING",         {{"!"},           true}},
    {"HR",              {{"===="},        false}},
    {"IMAGE",           {{"(("},          false}},
    {"IMAGEGALLERY",    {{"%%"},          false}},
    {"INLINECODE",      {{"@@"},          false}},
    {"ITALIC",          {{"''"},          false}},
    {"LINK",            {{"["},           false}},
    {"ORDEREDLIST",     {{"#"},           true}},
    {"PAGELINK",        {{"["},           false}},
    {"REDIRECTION",     {{"&"},           true}},
    {"REFERENCE",       {{"{{"},          false}},
    {"TABLE",           {{"|"},           true}},
    {"UNORDEREDLIST",   {{"*","-"},       true}},
    {"VIDEOLINK",       {{"(vid)"},       false}}
};

#define ERR_OK 0            // No errors.
#define ERR_BP 1            // Bad parameter - Cannot open input file.
#define ERR_BF 2            // Input file isn't as expected format.

bool wgetWanted = false;    // Do we want an image file for use with wget?


//------------------------------------------------------------------
// Function prototypes.
//------------------------------------------------------------------

int jfdi();                         // Just does it all.
bool readInputFile(string *aLine);  // Reads the input file.
void usage();

#endif // MAIN_H

