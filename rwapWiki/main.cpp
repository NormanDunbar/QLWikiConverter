// Reads an output file from mysql which has a title (on the first line)
// followed by some content on the remaining lines.
// The content will be converted according to the parameter file
// supplied on the command line.
//
// rwapWiki input_file conversion_file >output_file.
//
// Norman Dunbar.
// 10th July 2017.

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <map>

#include "version.h"
#include "main.h"

using namespace std;

/*
//------------------------------------------------------------------
// TEMPORARY Stuff. Testing with a conversion to HTML.
//------------------------------------------------------------------
#define CONV_PREAMBLE "<html><head><title></title></head><body>"
#define CONV_POSTAMBLE "</body></html>"

#define CONV_H1_PREAMBLE "<h1>"
#define CONV_H1_POSTAMBLE "</h1>"
#define CONV_H2_PREAMBLE "<h2>"
#define CONV_H2_POSTAMBLE "</h2>"
#define CONV_H3_PREAMBLE "<h3>"
#define CONV_H3_POSTAMBLE "</h3>"

#define CONV_BOLD_ON "<strong>"
#define CONV_BOLD_OFF "</strong>"

#define CONV_ITALIC_ON "<em>"
#define CONV_ITALIC_OFF "</em>"

#define CONV_HR_ON "<br>"
#define CONV_HR_OFF ""

#define CONV_LIST_PREAMBLE "<ul>"
#define CONV_LIST_POSTAMBLE "</ul>"

#define CONV_LIST_ON "<li>"
#define CONV_LIST_OFF "</li>"

#define CONV_NUM_LIST_PREAMBLE "<ol>"
#define CONV_NUM_LIST_POSTAMBLE "</ol>"

#define CONV_NUM_LIST_ON "<li>"
#define CONV_NUM_LIST_OFF "</li>"

#define CONV_DEFN_LIST_PREAMBLE "<dl>"
#define CONV_DEFN_LIST_POSTAMBLE "</dl>"

#define CONV_DEFN_LIST_TERM_ON "<dt>"
#define CONV_DEFN_LIST_TERM_OFF "</dt>"
#define CONV_DEFN_LIST_DESC_ON "<dd>"
#define CONV_DEFN_LIST_DESC_OFF "</dd>"

#define CONV_CODE_BLOCK_PREAMBLE "<pre>"
#define CONV_CODE_BLOCK_POSTAMBLE "</pre>"

#define CONV_CODE_LINE_ON ""
#define CONV_CODE_LINE_OFF ""

#define CONV_BLOCK_QUOTE_PREAMBLE "<blockquote>"
#define CONV_BLOCK_QUOTE_POSTAMBLE "</blockquote>"

#define CONV_BLOCK_QUOTE_LINE_ON "<p>"
#define CONV_BLOCK_QUOTE_LINE_OFF "</p>"

#define CONV_INLINE_CODE_ON "<code>"
#define CONV_INLINE_CODE_OFF "</code>"

#define CONV_FORCE_LINE_FEED_ON "<br>"
#define CONV_FORCE_LINE_FEED_OFF ""

#define CONV_PARAGRAPH_PREAMBLE "<p>"
#define CONV_PARAGRAPH_POSTAMBLE "</p>"

#define CONV_TABLE_PREAMBLE "<table border=\"1\">"
#define CONV_TABLE_POSTAMBLE "</table>"
#define CONV_TABLE_ROW_PREAMBLE "<tr>"
#define CONV_TABLE_ROW_POSTAMBLE "</tr>"
#define CONV_TABLE_CELL_PREAMBLE "<td>"
#define CONV_TABLE_CELL_POSTAMBLE "</td>"


// Watch these! In LaTeX, a backslash is acceptable, but in C++
// "\ref" becomes "linefeed ef"! We can double up here, but in
// the conversion file, we might get away with it!
// In HTML, it's "<a href="page_name#">text for link></a>"
#define CONV_WIKI_LINK "<a href=\"%PAGE_NAME%.html\">%PAGE_NAME%</a>"
#define CONV_URL_LINK "<a href=\"%URL%\" title=\"%TITLE_TEXT%\">%LINK_TEXT%</a>"
//#define CONV_YOUTUBE_LINK "<iframe width=\"30%\" height=\"30%\" src=\"%URL%\" frameborder=\"1\" allowfullscreen></iframe>"
#define CONV_YOUTUBE_LINK "<iframe width=\"30%\" height=\"30%\" src=\"https://youtube.com/embed/%VIDEO_ID%\" frameborder=\"1\" allowfullscreen></iframe>"
#define CONV_REFERENCE_LINK "<u>%REFERENCE%</u>"
#define CONV_ACRONYM_LINK "<abbr title=\"%TITLE_TEXT%\">%ACRONYM%</abbr>"
#define CONV_CITATION_LINK "<abbr title=\"%SOURCE%\">%CITATION%</abbr>"
#define CONV_ANCHOR_LINK "<abbr title=\"%TITLE_TEXT%\">%ANCHOR%</abbr>"
#define CONV_IMAGE_LINK "<a href=\"%SRC%\" title=\"%TITLE_TEXT%\"><img src=\"%SRC%\" alt=\"%ALT_TEXT%\" width=\"%WIDTH%\" height=\"%HEIGHT%\" border=\"0\" align=\"%ALIGN%\"></a>"
*/
//------------------------------------------------------------------
// Globals.
// Yes, I know! Don't start! Otherwise I'll use a GOTO as well!
//------------------------------------------------------------------

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

//------------------------------------------------------------------
// It all kicks off here.
//------------------------------------------------------------------
int main(int argc, char *argv[])
{
    cerr << endl
         << "rwapWiki - version: " << VERSION << endl;

    // Validate parameters.
    if (argc < 3) {
        cout << "Not enough parameters supplied." << endl
             << "USAGE:" << endl << endl
             << "\trwapWiki input_file conversion_file [> output_file]" << endl;
        return 1;
    }

    // Looks good, let's go!

    // Get the conversion file.
    mCfs = new ifstream(argv[2]);
    if (!mCfs->good()) {
        cerr << "File: '" << argv[2] << "' not found." << endl
             << "Cannot continue." << endl;
        return ERR_BP;
    } else {
        // Read the conversion parameters.
        if (!readConversionFile()) {
            cerr << "File: '" << argv[2] << "' cannot be read." << endl
                 << "Cannot continue." << endl;
            return ERR_BP;
        }
    }

    // Get the input file first.
    lineNumber = 0;
    mIfs = new ifstream(argv[1]);
    if (!mIfs->good()) {
        cerr << "File: '" << argv[1] << "' not found." << endl
             << "Cannot continue." << endl;
        return ERR_BP;
    }




    return jfdi();

}


//------------------------------------------------------------------
// The main program loop. Reads the input file until EOF, then exits
// with an appropriate code. Processes each line as it is read.
//------------------------------------------------------------------
int jfdi()
{
    string *aLine = new string();

    cout <<  findVariable("CONV_PREAMBLE");

    // Preload the "parser" with the first line of text.
    readInputFile(aLine);
    inParagraph = false;

    while (mIfs->good()) {
        //cout << "BEFORE: '" << *aLine << "'" << endl;

        // Process each line. Some lines need special handling as
        // they may be on multiple lines - code lines for example. Things
        // that loop around get to write out their own data. We don't care
        // once we have identified the first line here.
        if (!aLine->empty()) {
            // Lists get special treatment, they loop around.
            if (aLine->at(0) == '-'||
                aLine->at(0) == '*' ||
                aLine->at(0) == ';' ||
                aLine->at(0) == '#') {
                closeParagraph();
                doList(aLine, aLine->at(0));

                // Because we have read the first line past the list
                // we don't need to read the next line.
                continue;
            }

            // Code Blocks also loop around.
            if (aLine->at(0) == ' ') {
                closeParagraph();
                doCodeBlock(aLine);

                // Because we have read the first line past the code block
                // we don't need to read the next line.
                continue;
            }

            // Table Rows also loop around.
            if (aLine->at(0) == '|') {
                closeParagraph();
                doTableRow(aLine);

                // Because we have read the first line past the code block
                // we don't need to read the next line.
                continue;
            }

            // Do non-list stuff. If we process as a line start,
            // then we don't need any other processing. If we didn't,
            // do a linestart, we need to look for any embedded stuff.
            if (!doLineStarts(aLine)) {
                // We didn't do any line starts, so we need to open a new
                // paragraph here.
                if (!inParagraph) {
                    cout << findVariable("CONV_PARAGRAPH_PREAMBLE");
                    inParagraph = true;
                }
                doLineIncludes(aLine);
            }

        } else {
            // Empty line, terminate this paragraph & reset.
            if (inParagraph) {
                closeParagraph();
            }
        }

        // Write out the fully reformatted line.
        cout << *aLine << endl;

        // Get the next line.
        if (!readInputFile(aLine)) {
            break;
        }
    }

    // The input file is exhausted, were we still in a paragraph?
    if (inParagraph) {
        cout << findVariable("CONV_PARAGRAPH_POSTAMBLE") << endl;
    }

    cout << findVariable("CONV_POSTAMBLE");

    if (aLine) {
        delete aLine;
    }

    return ERR_OK;
}


//------------------------------------------------------------------
// Checks for special characters at the start of a line. Replaces
// them with the desired replacement text, if they are found.
//------------------------------------------------------------------
bool doLineStarts(string *aLine) {
    // Headings? These can contain bold and Italic.
    bool result = false;

    if (aLine->at(0) == '!') {
        result |= doHeadings(aLine);
    }

    // Horozontal Rule? 4 or more '='?
    if (aLine->substr(0, 4) == "====") {
        result |= doHR(aLine);
    }

    // Block Quotes - one per line.
    // These can contain bold and Italic.
    if (aLine->at(0) == '>') {
        result |= doBlockQuotes(aLine);
    }

    // The above can have embedded formatting too. Process
    // that, if present, after sorting out the linestart stuff.
    if (result) {
        doEmbeddedFormats(aLine);

        // Then close the current paragraph, if we are in one.
        if (inParagraph) {
            closeParagraph();
        }
    }

    return result;
}


//------------------------------------------------------------------
// Checks for special characters in the middle of a line. Replaces
// them with the desired replacement text, if they are found.
//
// WARNING: Some of these might wrap onto subsequent lines. Hmmm.
//------------------------------------------------------------------
void doLineIncludes(string *aLine) {

    if (aLine->find("__") != string::npos) {
        doBold(aLine);
    }

    if (aLine->find("''") != string::npos) {
        doItalic(aLine);
    }

    if (aLine->find("@@") != string::npos) {
        doInlineCode(aLine);
    }

    if (aLine->find("%%%") != string::npos) {
        doForcedLineFeed(aLine);
    }

    if ((aLine->find("[") != string::npos) ||
        (aLine->find("(vid)") != string::npos)){
        doLinks(aLine);
    }

    if (aLine->find("??") != string::npos) {
        doAcronyms(aLine);
    }

    if (aLine->find("{{") != string::npos) {
        doReferences(aLine);
    }

    if (aLine->find("^^") != string::npos) {
        doCitations(aLine);
    }

    if (aLine->find("~~") != string::npos) {
        doAnchors(aLine);
    }

    if (aLine->find("((") != string::npos) {
        doImages(aLine);
    }
    if (aLine->find("%%") != string::npos) {
        doImageGallery(aLine);
    }
}


//------------------------------------------------------------------
// CheckHeadings. Sorts out the headings for a line of text.
//------------------------------------------------------------------
bool doHeadings(string *aLine) {

    // Heading level 1.
    if (aLine->find("!!!") == 0) {
        *aLine = aLine->substr(3);
        aLine->insert(0, findVariable("CONV_H1_PREAMBLE"));
        aLine->append(findVariable("CONV_H1_POSTAMBLE"));
        return true;

    }

    // Heading level 2.
    if (aLine->find("!!") == 0) {
        *aLine = aLine->substr(2);
        aLine->insert(0, findVariable("CONV_H2_PREAMBLE"));
        aLine->append(findVariable("CONV_H2_POSTAMBLE"));
        return true;
    }

    // Heading level 3.
    *aLine = aLine->substr(1);
    aLine->insert(0, findVariable("CONV_H3_PREAMBLE"));
    aLine->append(findVariable("CONV_H3_POSTAMBLE"));
    return true;
}

//------------------------------------------------------------------
// The HR characters apply to a single line, so we simply replace
// the entire line. Always returns true, it just works!
//------------------------------------------------------------------
bool doHR(string *aLine) {
    aLine->assign(findVariable("CONV_HR_ON"));
    aLine->append(findVariable("CONV_HR_OFF"));
    return true;
}


//------------------------------------------------------------------
// Processes a list. Read and process each line until we hit a line
// that is not in a list. We enter here with the first line of a new
// list.
//------------------------------------------------------------------
void doList(string *aLine, char aChar) {

    string listItemBegin;
    string listItemEnd;

    // How many leading "aChar"s are there?
    // There cannot be zero or we wouldn't be here!
    string::size_type nestLevel = aLine->find_first_not_of(aChar);

    // What kind of list are we now in?
    bool unorderedList = (aChar == '-' || aChar == '*');
    bool orderedList = (aChar == '#');
    bool definitionList = (aChar == ';');

    // Definition Lists doen't wwork in the Wiki, but we can
    // make them work here. However, they need different handling.
    if (definitionList) {
        doDefinitionList(aLine, aChar);
        return;
    }

    // The remaining list types are similar enough to be done together.
    if (unorderedList) {
        listItemBegin = findVariable("CONV_LIST_ON");
        listItemEnd = findVariable("CONV_LIST_OFF");
    }

    if (orderedList) {
        listItemBegin = findVariable("CONV_NUM_LIST_ON");
        listItemEnd = findVariable("CONV_NUM_LIST_OFF");
    }


    // First item in this list, do the list preamble.
    if (orderedList) {
        cout << findVariable("CONV_NUM_LIST_PREAMBLE") << endl;
    }

    if (unorderedList) {
        cout << findVariable("CONV_LIST_PREAMBLE") << endl;
    }


    // Loop around and do the list items.
    while (mIfs->good()) {
        // Has the curent nesting level changed. Only really
        // affects the second and subsequent lines. There can be no
        // leading aChars though, watch out! That's the end of a list.
        string::size_type currentLevel = aLine->find_first_not_of(aChar);
        if (currentLevel == string::npos) {
            currentLevel = 0;
        }

        //cout << "DBG: '" << *aLine << '\'' << endl;
        //cout << "currentLevel: " << currentLevel << endl;
        //cout << "nestLevel: " << nestLevel << endl;

        // Are we now in a nested list?
        // If so, recurse into here to do the nested list.
        if (currentLevel > nestLevel) {
            doList(aLine, aLine->at(0));

            // When we return, we have the next line already
            // so we skip to the start of this loop again.
            continue;
        }

        // Or have we gone back up a level?
        // If so, exit  from this nested level.
        if (currentLevel < nestLevel) {

            // We are done herr, close the list.
            if (orderedList) {
                cout << findVariable("CONV_NUM_LIST_POSTAMBLE") << endl;
            }

            if (unorderedList) {
                cout << findVariable("CONV_LIST_POSTAMBLE") << endl;
            }
            // We also need to consider definition lists too.
            return;
        }

        // Must be the same level.
        aLine->replace(0, nestLevel, listItemBegin);
        aLine->append(listItemEnd);
        cout << *aLine << endl;

        // And read the next line.
        readInputFile(aLine);
    }

}


//------------------------------------------------------------------
// Processes a list. Read and process each line until we hit a line
// that is not in a list. We enter here with the first line of a new
// list.
//------------------------------------------------------------------
void doDefinitionList(string *aLine, char aChar) {

    // Can we even nest definition lists (dl)? It turns out we can.
    // We can also nest the other list types in a dl, but it stuffs
    // up the outer dl when we exit from the nested ones. So don't
    // allow incorrect nesting.

    // How many leading "aChar"s are there?
    // There cannot be zero or we wouldn't be here!
    string::size_type nestLevel = aLine->find_first_not_of(aChar);

    // What kind of list are we now in?
    bool definitionList = (aChar == ';');

    if (!definitionList) {
        cerr << "DoDefinitionList(): Invalid list nesting detected." << endl;
        return;
    }

    // First item in this list, do the list preamble.
    cout << findVariable("CONV_DEFN_LIST_PREAMBLE") << endl;

    // Loop around and do the list items.
    while (mIfs->good()) {
        // Has the curent nesting level changed. Only really
        // affects the second and subsequent lines. There can be no
        // leading aChars though, watch out! That's the end of a list.
        string::size_type currentLevel = aLine->find_first_not_of(aChar);
        if (currentLevel == string::npos) {
            currentLevel = 0;
        }

        //cout << "DBG: '" << *aLine << '\'' << endl;
        //cout << "currentLevel: " << currentLevel << endl;
        //cout << "nestLevel: " << nestLevel << endl;

        // Are we now in a nested list?
        // If so, recurse into here to do the nested list.
        if (currentLevel > nestLevel) {
            doDefinitionList(aLine, aLine->at(0));

            // When we return, we have the next line already
            // so we skip to the start of this loop again.
            continue;
        }

        // Or have we gone back up a level?
        // If so, exit  from this nested level.
        if (currentLevel < nestLevel) {

            // We are done here, close the list.
            cout << findVariable("CONV_DEFN_LIST_POSTAMBLE") << endl;
            return;
        }

        // Must be the same level. A definition list in the Wiki
        // looks like ';term:definition'.
        string::size_type colon = aLine->find(':', currentLevel);
        if (colon != string::npos) {
            aLine->replace(colon, 1, findVariable("CONV_DEFN_LIST_TERM_OFF") + findVariable("CONV_DEFN_LIST_DESC_ON"));
        }
        aLine->replace(0, nestLevel, findVariable("CONV_DEFN_LIST_TERM_ON"));

        aLine->append(findVariable("CONV_DEFN_LIST_DESC_OFF"));
        cout << *aLine << endl;

        // And read the next line.
        readInputFile(aLine);
    }

}


//------------------------------------------------------------------
// Does code blocks. Repeats the reading of the input file until we
// find a line that exits from the code block.
//------------------------------------------------------------------
void doCodeBlock(string *aLine) {

    // Preamble first.
    cout << findVariable("CONV_CODE_BLOCK_PREAMBLE") << endl;

    // Loop to write out code lines.
    while (mIfs->good() &&
           !aLine->empty() &&
           aLine->at(0) == ' ') {
        // We can lose the leading space. But keep
        // all other leading spaces as they are significant.
        cout << findVariable("CONV_CODE_LINE_ON")
             << aLine->substr(1)
             << findVariable("CONV_CODE_LINE_OFF")
             << endl;

        // Get the next line.
        readInputFile(aLine);
    }


    // Postamble last.
    cout << findVariable("CONV_CODE_BLOCK_POSTAMBLE") << endl;
}


//------------------------------------------------------------------
// Does block quotes. Repeats the reading of the input file until we
// find a line that exits from the block quote.
//------------------------------------------------------------------
bool doBlockQuotes(string *aLine) {

    // Lose the leading '>' and add the preamble stuff.
    *aLine = aLine->substr(1);
    aLine->insert(0, findVariable("CONV_BLOCK_QUOTE_LINE_ON"));
    aLine->insert(0, findVariable("CONV_BLOCK_QUOTE_PREAMBLE"));

    aLine->append(findVariable("CONV_BLOCK_QUOTE_LINE_OFF"));
    aLine->append(findVariable("CONV_BLOCK_QUOTE_POSTAMBLE"));
    return true;
}


//------------------------------------------------------------------
// Does table rows. Repeats the reading of the input file until we
// find a line that exits from the table.
//------------------------------------------------------------------
bool doTableRow(string *aLine) {

    vector<string> cellStuff;
    vector<string>::iterator cell;

    // Preamble first.
    cout << endl << findVariable("CONV_TABLE_PREAMBLE") << endl;

    // Loop to write out code lines.
    while (mIfs->good() &&
           !aLine->empty() &&
           aLine->at(0) == '|') {

        // Split this line into cells at the '|' character.
        stringstream tableCells(aLine->substr(1));
        string segment;

        // Read back from the stream and split at the '|' character.
        cellStuff.clear();
        while (getline(tableCells, segment, '|')) {
            cellStuff.push_back(segment);
        }

        // Row Preamble first.
        cout << findVariable("CONV_TABLE_ROW_PREAMBLE");

        // Process each cell in the row.
        for (cell = cellStuff.begin(); cell != cellStuff.end(); cell++) {

            // Not sure about this. Do we need to check for
            // Bold etc in table cells? I suppose so.
            doEmbeddedFormats(&(*cell));


            cout << findVariable("CONV_TABLE_CELL_PREAMBLE")
                 << *cell
                 << findVariable("CONV_TABLE_CELL_POSTAMBLE");
        }
        cout << findVariable("CONV_TABLE_ROW_POSTAMBLE") << endl;

        // Get the next line.
        readInputFile(aLine);
    }


    // Postamble last.
    cout << findVariable("CONV_TABLE_POSTAMBLE") << endl;
    return true;
}


//------------------------------------------------------------------
// Check linestart stuff for embedded formatting. This is used when
// lines that start with an identifier that extends until the final
// LF opn the line, *may* have embedded "stuff" in them. All that
// stuff is sorted out here. It can ge very weird indeed! But it
// works!
//------------------------------------------------------------------
void doEmbeddedFormats(string *aLine) {
    doBold(aLine);
    doItalic(aLine);
    doInlineCode(aLine);
    doLinks(aLine);
    doForcedLineFeed(aLine);
    doCitations(aLine);
    doReferences(aLine);
    doAnchors(aLine);
    doAcronyms(aLine);
    doImages(aLine);
}


//------------------------------------------------------------------
// Citations are "^^citation|source link^^" but what the hell does
// the Wiki do with them I have you to determine. Playing in the
// sandbox doesn't give many clues. Still...
//------------------------------------------------------------------
void doCitations(string *aLine) {

    string::size_type linkStart = aLine->find("^^");
    string::size_type linkEnd = aLine->find("^^", linkStart + 2);

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the citation & source.
        string refName = aLine->substr(linkStart + 2, linkEnd - linkStart - 2);
        string::size_type pipeStart = refName.find('|');
        if (pipeStart == string::npos) {
            cerr << "DoCitations(): Cannot locate '|' in citation in input file on line "
                 << lineNumber << endl;
            return;
        }

        // Split out the citation and text.
        string citation = refName.substr(0, pipeStart);
        string sourceText = refName.substr(pipeStart + 1, string::npos);

        string newLink = findVariable("CONV_CITATION_LINK");


        // Replace all occurrences of %CITATION% with the citation text.
        string::size_type nameStart = newLink.find("%CITATION%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 10, citation);
            nameStart = newLink.find("%CITATION%");
        }

        // Replace all occurrences of %SOURCE% with the sourcetext.
        nameStart = newLink.find("%SOURCE%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 8, sourceText);
            nameStart = newLink.find("%SOURCE%");
        }

        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd - linkStart + 2, newLink);

        // Any more links?
        linkStart = aLine->find("^^");
        linkEnd = aLine->find("^^", linkStart);
    }

}


//------------------------------------------------------------------
// References are "{{reference}}" but what the hell does
// the Wiki do with them I have you to determine. Playing in the
// sandbox doesn't give many clues. Still...
//------------------------------------------------------------------
void doReferences(string *aLine) {

    string::size_type linkStart = aLine->find("{{");
    string::size_type linkEnd = aLine->find("}}", linkStart + 2);

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the page_name.
        string refName = aLine->substr(linkStart + 2, linkEnd - linkStart - 2);
        string newLink = findVariable("CONV_REFERENCE_LINK");

        // Replace all occurrences of %REFERENCE% with the page name.
        string::size_type nameStart = newLink.find("%REFERENCE%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 11, refName);
            nameStart = newLink.find("%REFERENCE%");
        }

        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd - linkStart + 2, newLink);

        // Any more links?
        linkStart = aLine->find("{{");
        linkEnd = aLine->find("}}", linkStart);
    }

}


//------------------------------------------------------------------
// Anchors are "~~anchor|title~~" - see above! I have no idea what
// they are for in the Wiki. C'est la vie.
//------------------------------------------------------------------
void doAnchors(string *aLine) {

    string::size_type linkStart = aLine->find("~~");
    string::size_type linkEnd = aLine->find("~~", linkStart + 2);

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the anchor & title.
        string refName = aLine->substr(linkStart + 2, linkEnd - linkStart - 2);
        string::size_type pipeStart = refName.find('|');
        if (pipeStart == string::npos) {
            cerr << "DoAnchors(): Cannot locate '|' in anchor in input file on line "
                 << lineNumber << endl;
            return;
        }

        // Split out the anchor and title.
        string anchor = refName.substr(0, pipeStart);
        string titleText = refName.substr(pipeStart + 1, string::npos);

        string newLink = findVariable("CONV_ANCHOR_LINK");


        // Replace all occurrences of %CITATION% with the citation text.
        string::size_type nameStart = newLink.find("%ANCHOR%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 8, anchor);
            nameStart = newLink.find("%ANCHOR%");
        }

        // Replace all occurrences of %SOURCE% with the sourcetext.
        nameStart = newLink.find("%TITLE_TEXT%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 12, titleText);
            nameStart = newLink.find("%TITLE_TEXT%");
        }

        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd - linkStart + 2, newLink);

        // Any more links?
        linkStart = aLine->find("~~");
        linkEnd = aLine->find("~~", linkStart);
    }

}


//------------------------------------------------------------------
// Acronyms are "??acronym|Explanation Text??" and should be one a
// single line.
//------------------------------------------------------------------
void doAcronyms(string *aLine) {

    string::size_type linkStart = aLine->find("??");
    string::size_type linkEnd = aLine->find("??", linkStart + 2);

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the Acronym & explanation.
        string refName = aLine->substr(linkStart + 2, linkEnd - linkStart - 2);
        string::size_type pipeStart = refName.find('|');
        if (pipeStart == string::npos) {
            cerr << "DoAcronyms(): Cannot locate '|' in acronym in input file on line "
                 << lineNumber << endl;
            return;
        }

        // Split out the acronym and text.
        string acronym = refName.substr(0, pipeStart);
        string explanationText = refName.substr(pipeStart + 1, string::npos);

        string newLink = findVariable("CONV_ACRONYM_LINK");


        // Replace all occurrences of %ACRONYM% with the page name.
        string::size_type nameStart = newLink.find("%ACRONYM%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 9, acronym);
            nameStart = newLink.find("%ACRONYM%");
        }

        // Replace all occurrences of %TITLE_TEXT% with the page name.
        nameStart = newLink.find("%TITLE_TEXT%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 12, explanationText);
            nameStart = newLink.find("%TITLE_TEXT%");
        }

        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd - linkStart + 2, newLink);

        // Any more links?
        linkStart = aLine->find("??");
        linkEnd = aLine->find("??", linkStart);
    }

}


//------------------------------------------------------------------
// Replace boldOn and boldOff with the appropriate replacement text.
// But watch out for multi-line bold text.
//------------------------------------------------------------------
void doBold(string *aLine) {

    string::size_type boldOn = aLine->find("__");
    string::size_type boldOff = aLine->find("__", boldOn + 2);

    while ((boldOn != string::npos) ||
           (boldOff != string::npos)) {

        // If none of them were found, we are done.
        if ((boldOn == string::npos) &&
            (boldOff == string::npos)) {
           return;
        }

        // If we have both boldOn and boldOff, then
        // replace these backwards, or we get a bit confused if
        // the boldOn replacement text is longer than 2 characters.
        // Ask me how I know!
        if ((boldOn != string::npos) &&
            (boldOff != string::npos)) {

            inBold = false;

            aLine->replace(boldOff, 2, findVariable("CONV_BOLD_OFF"));
            aLine->replace(boldOn, 2, findVariable("CONV_BOLD_ON"));

            // Any more? Which we only need check if we got both previously.
            boldOn = aLine->find("__");
            boldOff = aLine->find("__", boldOn + 2);
            continue;
        }

        // We can only have boldOn and not boldOff.
        // We cannot ever have boldOff without boldOn
        // until we set it explicitly of course!
        if (inBold) {
            // Then boldOn is the end of a multi-line bold.
            aLine->replace(boldOn, 2, findVariable("CONV_BOLD_OFF"));
            inBold = false;
        } else {
            // Otherwise, boldOn is the start of a multi-line bold.
            aLine->replace(boldOn, 2, findVariable("CONV_BOLD_ON"));
            inBold = true;
        }

        // And because we only found one of the two, we are done.
        return;
    }
}

//------------------------------------------------------------------
// Replace italic characters with the appropriate replacement text.
// But watch out for multi-line italic text.
//------------------------------------------------------------------
void doItalic(string *aLine) {

    string::size_type italicOn = aLine->find("''");
    string::size_type italicOff = aLine->find("''", italicOn + 2);

    while ((italicOn != string::npos) ||
           (italicOff != string::npos)) {

        // If none of them were found, we are done.
        if ((italicOn == string::npos) &&
            (italicOff == string::npos)) {
           return;
        }

        // If we have both italicOn and italicOff, then
        // replace these backwards, or we get a bit confused if
        // the italicOn replacement text is longer than 2 characters.
        // Ask me how I know!
        if ((italicOn != string::npos) &&
            (italicOff != string::npos)) {

            inItalic = false;

            aLine->replace(italicOff, 2, findVariable("CONV_ITALIC_OFF"));
            aLine->replace(italicOn, 2, findVariable("CONV_ITALIC_ON"));

            // Any more? Which we only need check if we got both previously.
            italicOn = aLine->find("''");
            italicOff = aLine->find("''", italicOn + 2);
            continue;
        }

        // We can only have italicOn and not italicOff.
        // We cannot ever have italicOff without italicOn
        // until we set it explicitly of course!
        if (inItalic) {
            // Then boldOn is the end of a multi-line italic.
            aLine->replace(italicOn, 2, findVariable("CONV_ITALIC_OFF"));
            inItalic = false;
        } else {
            // Otherwise, boldOn is the start of a multi-line italic.
            aLine->replace(italicOn, 2, findVariable("CONV_ITALIC_ON"));
            inItalic = true;
        }

        // And because we only found one of the two, we are done.
        return;
    }
}

//------------------------------------------------------------------
// Replace inlione code with the appropriate replacement text.
// But watch out for multi-line inline code text.
//------------------------------------------------------------------
void doInlineCode(string *aLine) {

    string::size_type codeOn = aLine->find("@@");
    string::size_type codeOff = aLine->find("@@", codeOn + 2);

     while ((codeOn != string::npos) ||
           (codeOff != string::npos)) {

        // If none of them were found, we are done.
        if ((codeOn == string::npos) &&
            (codeOff == string::npos)) {
           return;
        }

        // If we have both codeOn and codeOff, then
        // replace these backwards, or we get a bit confused if
        // the codeOn replacement text is longer than 2 characters.
        if ((codeOn != string::npos) &&
            (codeOff != string::npos)) {

            inCode = false;

            aLine->replace(codeOff, 2, findVariable("CONV_INLINE_CODE_OFF"));
            aLine->replace(codeOn, 2, findVariable("CONV_INLINE_CODE_ON"));

            // Any more? Which we only need check if we got both previously.
            codeOn = aLine->find("@@");
            codeOff = aLine->find("@@", codeOn + 2);
            continue;
        }

        // We can only ever have codeOn and not codeOff.
        if (inCode) {
            // Then codeOn is the end of a multi-line italic.
            aLine->replace(codeOn, 2, findVariable("CONV_INLINE_CODE_OFF"));
            inCode = false;
        } else {
            // Otherwise, codeOn is the start of a multi-line italic.
            aLine->replace(codeOn, 2, findVariable("CONV_INLINE_CODE_ON"));
            inCode = true;
        }

        // And because we only found one of the two, we are done.
        return;
    }

}

//------------------------------------------------------------------
// Replace all the '%%%' characters with a forced line feed.
//------------------------------------------------------------------
void doForcedLineFeed(string *aLine) {
    string::size_type lf = aLine->find("%%%");

    while (lf != string::npos) {
        // Do it backwards.
        aLine->insert(lf + 3, findVariable("CONV_FORCE_LINE_FEED_OFF"));
        aLine->replace(lf, 3, findVariable("CONV_FORCE_LINE_FEED_ON"));

        // Anymore?
        lf = aLine->find("%%%");
    }
}


//------------------------------------------------------------------
// Looks for links in the current line, calls out to the appropriate
// function to process the corresponding link type.
//------------------------------------------------------------------
// Links are expected to all be on a single line - correct or not?
//------------------------------------------------------------------
void doLinks(string *aLine) {

    string::size_type youTubeLinkStart = aLine->find("(vid)");
    string::size_type youTubeLinkEnd = aLine->find("(/vid)");

    if (youTubeLinkStart != string::npos) {
        // We have a You Tube video link.
        doYouTubeLink(aLine);
        return;
    }

    // We don't have a You Tube video link - try other links.
    string::size_type linkStart = aLine->find('[');
    string::size_type linkEnd = aLine->find(']', linkStart);

    // Links should all be on the same line.
    if (linkStart != string::npos) {
        if (linkEnd == string::npos) {
            cerr << "DoLinks(): Missing link terminator ']' on line " << lineNumber
                 << "of input file." << endl;
            return;
        }
    }

    // Do we have a pipe (|) or not? If not, this is
    // a Wiki Page link.
    string::size_type pipeStart = aLine->find('|', linkStart);

    if (pipeStart != string::npos) {
        // We have a URL, HTTP Link.
        doUrl(aLine, pipeStart);
    } else {
        // Wiki age links [page_name].
        doWikiPageLink(aLine);
    }
}


//------------------------------------------------------------------
// Embed? a You Tube Video link.
//
// (vid) URL (/vid) eg http://youtube.com/watch?v=XXXXX   (URL)    or
// (vid) URL (/vid) eg http://youtube.com/embed/XXXXX     (VIDEO_ID)
//------------------------------------------------------------------
void doYouTubeLink(string *aLine) {

    string::size_type linkStart = aLine->find("(vid)");
    string::size_type linkEnd = aLine->find("(/vid)", linkStart);

    // We need the end on the same line please.
    if (linkEnd == string::npos) {
        cerr << "DoYouTubeLink(): Missing You Tube link terminator ']' on line " << lineNumber
             << "of input file." << endl;
        return;
    }

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the video URL.
        string urlName = aLine->substr(linkStart + 5, linkEnd - linkStart -5);
        string newLink = findVariable("CONV_YOUTUBE_LINK");

        // Extract the Video Id, in case we need it.
        string videoId = "";
        string::size_type idStart = urlName.find("?v=");

        // Do we have a valid ID? If so, it's everything to the end
        // of the link URL.
        if (idStart != string::npos) {
            videoId = urlName.substr(idStart + 3);
        }

        // Replace all occurrences of %URL% with the video URL.
        string::size_type urlStart = newLink.find("%URL%");
        while (urlStart != string::npos) {
            newLink.replace(urlStart, 5, urlName);
            urlStart = newLink.find("%URL%");
        }

        // also, replace all occurrences of %VIDEO_ID% with the video name.
        urlStart = newLink.find("%VIDEO_ID%");
        while (urlStart != string::npos) {
            newLink.replace(urlStart, 10, videoId);
            urlStart = newLink.find("%VIDEO_ID%");
        }

        // Now we can do the actual replacement.
        aLine->replace(linkStart, urlName.length() + 11, newLink);

        // Any more links?
        linkStart = aLine->find("(vid)");
        linkEnd = aLine->find("(/vid)", linkStart);
    }
}



//------------------------------------------------------------------
// Process a Wiki page link (a link to another Wiki page). The format
// is:
//
// [ page name ]
//------------------------------------------------------------------
void doWikiPageLink(string *aLine) {

    string::size_type linkStart = aLine->find('[');
    string::size_type linkEnd = aLine->find(']', linkStart);

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the page_name.
        string pageName = aLine->substr(linkStart + 1, linkEnd - linkStart -1);
        string newLink = findVariable("CONV_WIKI_LINK");

        // Replace all occurrences of %PAGE_NAME% with the page name.
        string::size_type nameStart = newLink.find("%PAGE_NAME%");
        while (nameStart != string::npos) {
            newLink.replace(nameStart, 11, pageName);
            nameStart = newLink.find("%PAGE_NAME%");
        }

        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd - linkStart + 1, newLink);

        // Any more links?
        linkStart = aLine->find('[');
        linkEnd = aLine->find(']', linkStart);
    }
}

//------------------------------------------------------------------
// Split a URL into (up to) 4 parts at the '|' character, and build
// them into an output format for a link in the output file.
//------------------------------------------------------------------
void doUrl(string *aLine, string::size_type pipeStart) {

    vector<string> linkStuff;

    string::size_type linkStart = aLine->find('[');
    string::size_type linkEnd = aLine->find(']', linkStart);

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the link text.
        string linkText = aLine->substr(linkStart + 1, linkEnd - linkStart -1);

        // Stream it.
        stringstream oldLinkText(linkText);
        string segment;

        // Read back from the stream and split at the '|' character.
        linkStuff.clear();
        while (getline(oldLinkText, segment, '|')) {
            linkStuff.push_back(segment);
        }

        // At this point, we have:
        //
        // linkStuff[0] = Link text. (mandatory)
        // linkStuff[1] = URL. (mandatory)
        // linkStuff[2] = Language. (optional)
        // linkStuff[3] = Title text. (optional)

        // Now we need to stuff it into the replacement text.
        string newLink = findVariable("CONV_URL_LINK");

        // How many chunks of the link did we get?
        uint8_t chunks = linkStuff.size();
        if (chunks < 2) {
            // Illegal link.
            cerr << "DoURL(): Invalid link on line " << lineNumber
                 << " of input file. The invalid link is '"
                 << linkText << "'" << endl;
        }

        // Replace all occurrences of %LINK_TEXT% with the correct text.
        // MANDATORY.
        string::size_type textStart = newLink.find("%LINK_TEXT%");
        while (textStart != string::npos) {
            newLink.replace(textStart, 11, linkStuff[0]);
            textStart = newLink.find("%LINK_TEXT%");
        }

        // Replace all occurrences of %URL% with the correct text.
        // MANDATORY.
        textStart = newLink.find("%URL%");
        while (textStart != string::npos) {
            newLink.replace(textStart, 5, linkStuff[1]);
            textStart = newLink.find("%URL%");
        }

        // Replace all occurrences of %LINK_TEXT% with the correct text.
        // OPTIONAL.
        if (chunks > 2) {
            textStart = newLink.find("%LANGUAGE%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 10, linkStuff[2]);
                textStart = newLink.find("%LANGUAGE%");
            }
        }

        // Replace all occurrences of %LINK_TEXT% with the correct text.
        // OPTIONAL.
        if (chunks > 3) {
            textStart = newLink.find("%TITLE_TEXT%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 12, linkStuff[3]);
                textStart = newLink.find("%TITLE_TEXT%");
            }
        }


        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd - linkStart + 1, newLink);

        // Any more links?
        linkStart = aLine->find('[');
        linkEnd = aLine->find(']', linkStart);
    }
}


//------------------------------------------------------------------
// Split an image into (up to) 7 parts at the '|' character, and
// build them into an output format for images in the output file.
// The image link format is:
//
//      ((source|alt|align|long description|width|height|???))
//
// SOURCE = URL of image. Always web address.
// ALT = Alt Text for image.
// ALIGN = the text align="X" where x in 'LlgGRrDd' for left/right.
// LONG DESCRIPTION = Couold be used for title= in <A> links in html.
// WIDTH = Image Width in pixels.
// HEIGHT = Image height in pixels.
// ??? = POPUP. Pops up a big image if clicked. (default)
//------------------------------------------------------------------
void doImages(string *aLine) {

    vector<string> linkStuff;

    string::size_type linkStart = aLine->find("((");
    string::size_type linkEnd = aLine->find("))", linkStart + 2);

    while ((linkStart != string::npos) &&
           (linkEnd != string::npos)) {

        // Extract the image link text.
        string linkText = aLine->substr(linkStart + 2, linkEnd - linkStart -1);

        // Stream it.
        stringstream oldLinkText(linkText);
        string segment;

        // Read back from the stream and split at the '|' character.
        while (getline(oldLinkText, segment, '|')) {
            linkStuff.push_back(segment);
        }

        // At this point, we have:
        //
        // linkStuff[0] = Image source URL. (%SRC%) MANDATORY.
        // linkStuff[1] = ALT Text. (%ALT_TEXT%) OPTIONAL.
        // linkStuff[2] = Align. (%ALIGN%) OPTIONAL.
        // linkStuff[3] = Title text. (%TITLE_TEXT%) OPTIONAL.
        // linkStuff[4] = Width. (%WIDTH%) OPTIONAL.
        // linkStuff[5] = Height (%HEIGHT%) OPTIONAL.
        // linkStuff[6] = Null or POPUP. (Not Used) OPTIONAL.

        uint8_t chunks = linkStuff.size();
        if (chunks < 1) {
            // We have a duff link.
            cerr << "DoImages(): Invalid link on line " << lineNumber
                 << " of the input file. The invalid image link is '"
                 << linkText << "'" << endl;
            return;
        }

        // Now we need to stuff it into the replacement text.
        string newLink = findVariable("CONV_IMAGE_LINK");

        // Output the Image URL in case we need to download it.
        cerr << "IMAGE LINK: " << linkStuff[0] << endl;

        // Replace all occurrences of %SRC% with the correct text.
        // MANDATORY.
        string::size_type textStart = newLink.find("%SRC%");
        while (textStart != string::npos) {
            newLink.replace(textStart, 5, linkStuff[0]);
            textStart = newLink.find("%SRC%");
        }

        // ALL the remainder are optional.

        // Replace all occurrences of %ALT_TEXT% with the correct text.
        if (chunks > 1) {
            textStart = newLink.find("%ALT_TEXT%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 10, linkStuff[1]);
                textStart = newLink.find("%ALT_TEXT%");
            }
        }

        // Replace all occurrences of %ALIGN% with the correct text.
        // The text is in double quotes, and is one character long.
        if (chunks > 2) {
            textStart = newLink.find("%ALIGN%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 7, linkStuff[2].substr(1, 1));
                textStart = newLink.find("%ALIGN%");
            }

            // Replace all occurrences of %ALIGN_EXPAND% with the correct text.
            // The text is in double quotes, and is one character long but we
            // expand this to left or right accordingly.
            string alignText;
            char alignChar = linkStuff[2].at(1);    // Double quoted.
            if ((alignChar == 'l') ||
                (alignChar == 'L') ||
                (alignChar == 'g') ||
                (alignChar == 'G')) {
                alignText = "left";
            } else {
                alignText = "right";
                }
            textStart = newLink.find("%ALIGN_EXPAND%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 14, alignText);
                textStart = newLink.find("%ALIGN_EXPAND%");
            }
        }


        // Replace all occurrences of %LINK_TEXT% with the correct text.
        if (chunks > 3) {
            textStart = newLink.find("%TITLE_TEXT%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 12, linkStuff[3]);
                textStart = newLink.find("%TITLE_TEXT%");
            }
        }

        // Replace all occurrences of %WIDTH% with the correct text.
        if (chunks > 4) {
            textStart = newLink.find("%WIDTH%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 7, linkStuff[4]);
                textStart = newLink.find("%WIDTH%");
            }
        }

        // Replace all occurrences of %HEIGHT% with the correct text.
        if (chunks > 5) {
            textStart = newLink.find("%HEIGHT%");
            while (textStart != string::npos) {
                newLink.replace(textStart, 8, linkStuff[5]);
                textStart = newLink.find("%HEIGHT%");
            }
        }

        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd + 2, newLink);

        // Any more links?
        linkStart = aLine->find("((");
        linkEnd = aLine->find("))", linkStart + 2);
    }
}


//------------------------------------------------------------------
// Image Gallery - cannot cope. Just barf!
//------------------------------------------------------------------
void doImageGallery(string *aLine) {
    cerr << "DoImageGallery(): OOPS! Image Gallery set up found at line " << lineNumber
         << " of the input file. I can't cope with those, sorry."
         << endl;
}


//------------------------------------------------------------------
// We need to close the current paragraph from a few places. The
// code below might help!
//------------------------------------------------------------------
void closeParagraph() {

    if (inParagraph) {
        cout << findVariable("CONV_PARAGRAPH_POSTAMBLE") << endl;
        inParagraph = false;
    }
}


//------------------------------------------------------------------
// Reads the input file and returns a bool indicating success or not
// however it happens.
//------------------------------------------------------------------
bool readInputFile(string *aLine) {

    if (mIfs->good()) {
        getline(*mIfs, *aLine);
        lineNumber++;
    }

    return mIfs->good();
}


//------------------------------------------------------------------
// Process the conversion file and returns a bool indicating success
// or not of the processing.
//
// Each line is a comment (#...) or not. If not a comment then it is
// a variable name, in upper case and equals and the rest is the
// replacement text.
//------------------------------------------------------------------
bool readConversionFile() {

    string cLine = "";

    while (mCfs->good()) {
        readConvertLine(&cLine);

        // Strip out comments.
        if ((cLine.empty() ||
            (cLine.at(0) == '#'))) {
                continue;
        }

        // Process a valid line.
        // Split into two parts, the variable and the value.
        string::size_type equals = cLine.find('=');
        if (equals == string::npos) {
            // We have an invalid line.
            cerr << "ReadConversionFile(): Invalid line. No '=' found, at line "
                 << lineNumber << " in conversion file." << endl;
            return false;
        }

        // Extract the variable and value.
        string variableName = cLine.substr(0, equals);
        string variableValue = cLine.substr(equals + 1);

        //cerr << "Name : '" << variableName << "'" << endl;
        //cerr << "Value: '" << variableValue << "'" << endl;

        // Build a pair, for insertion into the variables list.
        // Overwrite value if variable name already there.
        pair<map<string, string>::iterator, bool> exists;
        exists = variableMap.insert(pair<string, string>(variableName, variableValue));

        // If the bool is false, the insert failed, already there, so overwire
        // the value.
        //
        // Exists is a pair<mBinds::iterator, bool>. It indexes the map at the
        // position of the newly inserted or already existing variable name.
        //
        // Exists.second is the bool. True=inserted, False=already exists.
        // Exists.first is an iterator (pseudo pointer) to a pair <string, string>.
        // Exists.first->first is the first string, aka the variableName.
        // Exists.first->second is the second string, aka the variableValue.
        //
        // Phew!
        if (!exists.second) {
            // Overwrite the current setting.
            exists.first->second = variableValue;
            cerr << "ReadConversionFile(): Overwriting " << variableName
                 << " with new value. Already exists." << endl;
        }
    }

    return true;
}


//
// Search the variables map for a given variable name.
// Returns a valid iterator if all ok, otherwise returns mVariables.end().
string findVariable(const string variableName) {

    map<string, string>::iterator i = variableMap.find(variableName);

    if (i == variableMap.end()) {
        // Not found.
        cerr << "FindVariable(): VariableName: " << variableName
              << " not found." << endl;
    }

    return (*i).second;
}



//------------------------------------------------------------------
// Reads the convert file and returns a bool indicating success or not
// however it happens.
//------------------------------------------------------------------
bool readConvertLine(string *cLine) {

    if (mCfs->good()) {
        getline(*mCfs, *cLine);
        lineNumber++;
    }

    return mCfs->good();
}


