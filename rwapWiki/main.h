#ifndef MAIN_H
#define MAIN_H

using std::string;
using std::map;

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


#endif // MAIN_H

