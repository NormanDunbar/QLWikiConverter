#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <map>
#include <vector>

#include "version.h"
#include "main.h"

using namespace std;

//------------------------------------------------------------------------------
// Function prototypes.
//------------------------------------------------------------------------------
int jfdi();                             // Just does it all.
bool readConversionFile();              // Reads the conversions data.
bool readInputFile(string *aLine);      // Reads a line from the input file.
bool readConvertLine(string *aLine);    // Reads a line from the conversion file.

bool doLineStarts(string *aLine);       // Checks for special chars at the start of a line.
bool doHeadings(string *aLine);         // Processes Headings lines.
bool doHR(string *aLine);               // Horozontal Rule.
void doList(string *aLine, char aChar); // Does unordered and ordered lists.
void doDefinitionList(string *aLine, char aChar); // Does definition lists.
void doCodeBlock(string *aLine);        // Code block.
bool doBlockQuotes(string *aLine);      // Block quotes.
bool doTableRow(string *aLine);         // Table row(s).
void doLineIncludes(string *aLine);     // Checks for special chars at the start of a line.
void doBold(string *aLine);             // Processes Bold text.
void doItalic(string *aLine);           // Processes Italic text.
void doInlineCode(string *aLine);       // Process inline code chunks.
void doForcedLineFeed(string *aLine);   // Process forced line feeds.
void doLinks(string *aLine);            // Looks for links. All kinds of links.
void doWikiPageLink(string *aLine);     // Do a Wiki Page Link.
void doUrl(string *aLine);              // Looks for HTTP links.
void doYouTubeLink(string *aLine);      // Do a Wiki Page Link.
void doCitations(string *aLine);        // Citations.
void doReferences(string *aLine);       // References, whatever they are.
void doAnchors(string *aLine);          // Anchors, likewise!
void doAcronyms(string *aLine);         // I can do these!
void doImages(string *aLine);            // Guess!
void doImageGallery(string *aLine);     // I can't do these though!

void doEmbeddedFormats(string *aLine);  // Checks line start stuff for embedded codes.
void closeParagraph();                  // Closes the current paragraph.

string findVariable(const string variableName); // returns the value of a conversion variable.
bool validateVariables();               // Validates that all translation vars are supplied & valid.

//------------------------------------------------------------------------------
// Global variables. I know I shouldn't, but the devil made me do it! Plus, if
// anyone complains, I'll rebel, and usea goto as well. ;-)
//------------------------------------------------------------------------------
string pageTitle;           // Original page name in old Wiki.
string notFound = "***** NOT FOUND *****";  // Translation variable not found.

uint32_t lineNumber = 0;    // Input file, current line number.
ifstream *mIfs;             // Input file aka argv[1].
ifstream *mCfs;             // Conversion file aka argv[2].
bool inBold;                // Are we in a possible multi-line bold string?
bool inItalic;              // Are we in a possible multi-line italic string?
bool inCode;                // Are we in (multi-line?) inline code?
bool inParagraph;           // Are we in a paragraph?

const int ERR_OK = 0;       // No errors.
const int ERR_BP = 1;       // Bad Parameter error.
const int ERR_BC = 2;       // Bad conversion error.

// Variables table - for conversion.
map <string, string> variableMap;
map<string, string>::iterator variableEntry;



//------------------------------------------------------------------------------
// This is a list of all the valid translation variables that must be present,
// even if blank, in each translation file used.
//------------------------------------------------------------------------------
vector<string> translationVariables = {
    {"CONV_PREAMBLE"},
    {"CONV_POSTAMBLE"},
    {"CONV_PAGENAME_PUNCTUATION"},
    {"CONV_PAGENAME_PUNCTUATION_REPLACE_WITH"},
    {"CONV_PAGENAME_SPACES"},
    {"CONV_PAGENAME_SPACES_REPLACE_WITH"},
    {"CONV_ACRONYM_LINK"},
    {"CONV_ANCHOR_LINK"},
    {"CONV_BLOCK_QUOTE_PREAMBLE"},
    {"CONV_BLOCK_QUOTE_POSTAMBLE"},
    {"CONV_BLOCK_QUOTE_LINE_ON"},
    {"CONV_BLOCK_QUOTE_LINE_OFF"},
    {"CONV_BOLD_ON"},
    {"CONV_BOLD_OFF"},
    {"CONV_CITATION_LINK"},
    {"CONV_CITATION_NOSOURCE_LINK"},
    {"CONV_CODE_BLOCK_PREAMBLE"},
    {"CONV_CODE_BLOCK_POSTAMBLE"},
    {"CONV_CODE_LINE_ON"},
    {"CONV_CODE_LINE_OFF"},
    {"CONV_DEFN_LIST_PREAMBLE"},
    {"CONV_DEFN_LIST_POSTAMBLE"},
    {"CONV_DEFN_LIST_TERM_ON"},
    {"CONV_DEFN_LIST_TERM_OFF"},
    {"CONV_DEFN_LIST_DESC_ON"},
    {"CONV_DEFN_LIST_DESC_OFF"},
    {"CONV_FORCE_LINE_FEED_ON"},
    {"CONV_FORCE_LINE_FEED_OFF"},
    {"CONV_URL_LINK"},
    {"CONV_H1_PREAMBLE"},
    {"CONV_H1_POSTAMBLE"},
    {"CONV_H2_PREAMBLE"},
    {"CONV_H2_POSTAMBLE"},
    {"CONV_H3_PREAMBLE"},
    {"CONV_H3_POSTAMBLE"},
    {"CONV_HR_ON"},
    {"CONV_HR_OFF"},
    {"CONV_IMAGE_LINK"},
    {"CONV_IMAGE_LINK_LEFT"},
    {"CONV_IMAGE_LINK_RIGHT"},
    {"CONV_IMAGE_LINK_CENTRE"},
    {"CONV_INLINE_CODE_ON"},
    {"CONV_INLINE_CODE_OFF"},
    {"CONV_ITALIC_ON"},
    {"CONV_ITALIC_OFF"},
    {"CONV_NUM_LIST_PREAMBLE"},
    {"CONV_NUM_LIST_POSTAMBLE"},
    {"CONV_NUM_LIST_ON"},
    {"CONV_NUM_LIST_OFF"},
    {"CONV_PARAGRAPH_PREAMBLE"},
    {"CONV_PARAGRAPH_POSTAMBLE"},
    {"CONV_REFERENCE_LINK"},
    {"CONV_TABLE_PREAMBLE"},
    {"CONV_TABLE_POSTAMBLE"},
    {"CONV_TABLE_ROW_PREAMBLE"},
    {"CONV_TABLE_ROW_POSTAMBLE"},
    {"CONV_TABLE_CELL_PREAMBLE"},
    {"CONV_TABLE_CELL_POSTAMBLE"},
    {"CONV_LIST_PREAMBLE"},
    {"CONV_LIST_POSTAMBLE"},
    {"CONV_LIST_ON"},
    {"CONV_LIST_OFF"},
    {"CONV_WIKI_LINK"},
    {"CONV_YOUTUBE_LINK"}
};


#endif // MAIN_H

