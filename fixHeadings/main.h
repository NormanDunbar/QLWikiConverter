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

string programName = "fixHeadings";
string fileName = "";

uint32_t lineNumber = 0;    // Input file, current line number.
ifstream *mIfs;             // Input file aka argv[1].

#define ERR_OK 0            // No errors.
#define ERR_BP 1            // Bad parameter - Cannot open input file.

//------------------------------------------------------------------
// Function prototypes.
//------------------------------------------------------------------

int jfdi();                         // Just does it all.
bool readInputFile(string *aLine);  // Reads the input file.
void usage();

#endif // MAIN_H

