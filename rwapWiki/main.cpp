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

#include "version.h"
#include "main.h"

using namespace std;

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

// Watch these! In LaTeX, a backslash is acceptable, but in C++
// "\ref" becomes "linefeed ef"! We can double up here, but in
// the conversion file, we might get away with it!
// In HTML, it's "<a href="page_name#">text for link></a>"
#define CONV_WIKI_LINK "<a href=\"%PAGE_NAME%#\">%PAGE_NAME%</a>"
#define CONV_URL_LINK "<a href=\"%URL%\" title=\"%TITLE_TEXT%\">%LINK_TEXT%</a>"

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

    // Get the input file first.
    mIfs = new ifstream(argv[1]);
    if (!mIfs->good()) {
        cout << "File: '" << argv[1] << "' not found." << endl
             << "Cannot continue." << endl;
        return ERR_BP;
    }

    // Get the conversion file.
    mCfs = new ifstream(argv[2]);
    if (!mCfs->good()) {
        cout << "File: '" << argv[2] << "' not found." << endl
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

    cout << CONV_PREAMBLE << endl;

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
            if (aLine->find('-') == 0 ||
                aLine->find('*') == 0 ||
                aLine->find(';') == 0 ||
                aLine->find('#') == 0) {
                closeParagraph();
                doList(aLine, aLine->at(0));

                // Because we have read the first line past the list
                // we don't need to read the next line.
                continue;
            }

            // Code Blocks also loop around
            if (aLine->find(' ') == 0) {
                closeParagraph();
                doCodeBlock(aLine);

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
                    cout << CONV_PARAGRAPH_PREAMBLE;
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
        cout << CONV_PARAGRAPH_POSTAMBLE << endl;
    }

    cout << CONV_POSTAMBLE << endl;

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

    if (aLine->find('!') == 0) {
        result |= doHeadings(aLine);
    }

    // Horozontal Rule? 4 or more '='?
    if (aLine->find("====") == 0) {
        result |= doHR(aLine);
    }

    // Block Quotes - one per line.
    // These can contain bold and Italic.
    if (aLine->find('>') == 0) {
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

    if (aLine->find("[") != string::npos) {
        doLinks(aLine);
    }

}


//------------------------------------------------------------------
// CheckHeadings. Sorts out the headings for a line of text.
//------------------------------------------------------------------
bool doHeadings(string *aLine) {

    // Heading level 1.
    if (aLine->find("!!!") == 0) {
        *aLine = aLine->substr(3);
        aLine->insert(0, CONV_H1_PREAMBLE);
        aLine->append(CONV_H1_POSTAMBLE);
        return true;

    }

    // Heading level 2.
    if (aLine->find("!!") == 0) {
        *aLine = aLine->substr(2);
        aLine->insert(0, CONV_H2_PREAMBLE);
        aLine->append(CONV_H2_POSTAMBLE);
        return true;
    }

    // Heading level 3.
    *aLine = aLine->substr(1);
    aLine->insert(0, CONV_H3_PREAMBLE);
    aLine->append(CONV_H3_POSTAMBLE);
    return true;
}

//------------------------------------------------------------------
// The HR characters apply to a single line, so we simply replace
// the entire line. Always returns true, it just works!
//------------------------------------------------------------------
bool doHR(string *aLine) {
    aLine->assign(CONV_HR_ON);
    aLine->append(CONV_HR_OFF);
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
        listItemBegin = CONV_LIST_ON;
        listItemEnd = CONV_LIST_OFF;
    }

    if (orderedList) {
        listItemBegin = CONV_NUM_LIST_ON;
        listItemEnd = CONV_NUM_LIST_OFF;
    }


    // First item in this list, do the list preamble.
    if (orderedList) {
        cout << CONV_NUM_LIST_PREAMBLE << endl;
    }

    if (unorderedList) {
        cout << CONV_LIST_PREAMBLE << endl;
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
                cout << CONV_NUM_LIST_POSTAMBLE << endl;
            }

            if (unorderedList) {
                cout << CONV_LIST_POSTAMBLE << endl;
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
        cerr << "Invalid list nesting in doDefinitionList." << endl;
        return;
    }

    // First item in this list, do the list preamble.
    cout << CONV_DEFN_LIST_PREAMBLE << endl;

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
            cout << CONV_DEFN_LIST_POSTAMBLE << endl;
            return;
        }

        // Must be the same level. A definition list in the Wiki
        // looks like ';term:definition'.
        string::size_type colon = aLine->find(':', currentLevel);
        if (colon != string::npos) {
            aLine->replace(colon, 1, string(CONV_DEFN_LIST_TERM_OFF) + string(CONV_DEFN_LIST_DESC_ON));
        }
        aLine->replace(0, nestLevel, CONV_DEFN_LIST_TERM_ON);

        aLine->append(CONV_DEFN_LIST_DESC_OFF);
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
    cout << CONV_CODE_BLOCK_PREAMBLE << endl;

    // Loop to write out code lines.
    while (mIfs->good() &&
           !aLine->empty() &&
           aLine->at(0) == ' ') {
        // We can lose the leading space. But keep
        // all other leading spaces as they are significant.
        cout << CONV_CODE_LINE_ON
             << aLine->substr(1)
             << CONV_CODE_LINE_OFF
             << endl;

        // Get the next line.
        readInputFile(aLine);
    }


    // Postamble last.
    cout << CONV_CODE_BLOCK_POSTAMBLE << endl;
}


//------------------------------------------------------------------
// Does block quotes. Repeats the reading of the input file until we
// find a line that exits from the block quote.
//------------------------------------------------------------------
bool doBlockQuotes(string *aLine) {

    // Lose the leading '>' and add the preamble stuff.
    *aLine = aLine->substr(1);
    aLine->insert(0, CONV_BLOCK_QUOTE_LINE_ON);
    aLine->insert(0, CONV_BLOCK_QUOTE_PREAMBLE);

    aLine->append(CONV_BLOCK_QUOTE_LINE_OFF);
    aLine->append(CONV_BLOCK_QUOTE_POSTAMBLE);
    return true;
}


//------------------------------------------------------------------
// Check linestart stuff for embedded formatting.
//------------------------------------------------------------------
void doEmbeddedFormats(string *aLine) {
    doBold(aLine);
    doItalic(aLine);
    doInlineCode(aLine);
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

            aLine->replace(boldOff, 2, CONV_BOLD_OFF);
            aLine->replace(boldOn, 2, CONV_BOLD_ON);

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
            aLine->replace(boldOn, 2, CONV_BOLD_OFF);
            inBold = false;
        } else {
            // Otherwise, boldOn is the start of a multi-line bold.
            aLine->replace(boldOn, 2, CONV_BOLD_ON);
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

            aLine->replace(italicOff, 2, CONV_ITALIC_OFF);
            aLine->replace(italicOn, 2, CONV_ITALIC_ON);

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
            aLine->replace(italicOn, 2, CONV_ITALIC_OFF);
            inItalic = false;
        } else {
            // Otherwise, boldOn is the start of a multi-line italic.
            aLine->replace(italicOn, 2, CONV_ITALIC_ON);
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

            aLine->replace(codeOff, 2, CONV_INLINE_CODE_OFF);
            aLine->replace(codeOn, 2, CONV_INLINE_CODE_ON);

            // Any more? Which we only need check if we got both previously.
            codeOn = aLine->find("@@");
            codeOff = aLine->find("@@", codeOn + 2);
            continue;
        }

        // We can only ever have codeOn and not codeOff.
        if (inCode) {
            // Then codeOn is the end of a multi-line italic.
            aLine->replace(codeOn, 2, CONV_INLINE_CODE_OFF);
            inCode = false;
        } else {
            // Otherwise, codeOn is the start of a multi-line italic.
            aLine->replace(codeOn, 2, CONV_INLINE_CODE_ON);
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
        aLine->insert(lf + 3, CONV_FORCE_LINE_FEED_OFF);
        aLine->replace(lf, 3, CONV_FORCE_LINE_FEED_ON);

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

    string::size_type linkStart = aLine->find('[');
    string::size_type linkEnd = aLine->find(']', linkStart);

    // Links should all be on the same line.
    if (linkEnd == string::npos) {
        cerr << "Missing link terminator ']' on line " << lineNumber
             << "of input file." << endl;
        return;
    }


    if (linkStart != string::npos) {
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
        string newLink = CONV_WIKI_LINK;

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
// Process an HTTP URL. The format is:
//
// [ linktext | URL | Language | titleText ]
//
// Which, in HTML, becomes:
//
// <a href="URL" title="titleText">linkText</a>.
//------------------------------------------------------------------
/*
std::stringstream test("this_is_a_test_string");
std::string segment;
std::vector<std::string> seglist;

while(std::getline(test, segment, '_'))
{
   seglist.push_back(segment);
}
*/
//------------------------------------------------------------------

void doUrl(string *aLine, string::size_type pipeStart) {

    // #define CONV_URL_LINK ""<a href=\"%URL%\" title=\"%TITLE_TEXT%\">%LINK_TEXT%</a>

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
        while (getline(oldLinkText, segment, '|')) {
            linkStuff.push_back(segment);
        }

        // At this point, we have:
        //
        // linkStuff[0] = Link text.
        // linkStuff[1] = URL.
        // linkStuff[2] = Language.
        // linkStuff[3] = Title text.

        // Now we need to stuff it into the replacement text.
        string newLink = CONV_URL_LINK;

        // Replace all occurrences of %LINK_TEXT% with the correct text.
        string::size_type textStart = newLink.find("%LINK_TEXT%");
        while (textStart != string::npos) {
            newLink.replace(textStart, 11, linkStuff[0]);
            textStart = newLink.find("%LINK_TEXT%");
        }

        // Replace all occurrences of %URL% with the correct text.
        textStart = newLink.find("%URL%");
        while (textStart != string::npos) {
            newLink.replace(textStart, 5, linkStuff[1]);
            textStart = newLink.find("%URL%");
        }

        // Replace all occurrences of %LINK_TEXT% with the correct text.
        textStart = newLink.find("%LANGUAGE%");
        while (textStart != string::npos) {
            newLink.replace(textStart, 10, linkStuff[2]);
            textStart = newLink.find("%LANGUAGE%");
        }

        // Replace all occurrences of %LINK_TEXT% with the correct text.
        textStart = newLink.find("%TITLE_TEXT%");
        while (textStart != string::npos) {
            newLink.replace(textStart, 12, linkStuff[3]);
            textStart = newLink.find("%TITLE_TEXT%");
        }


        // Now we can do the actual replacement.
        aLine->replace(linkStart, linkEnd - linkStart + 1, newLink);

        // Any more links?
        linkStart = aLine->find('[');
        linkEnd = aLine->find(']', linkStart);
    }
}


//------------------------------------------------------------------
// We need to close the current paragraph from a few places. The
// code below might help!
//------------------------------------------------------------------
void closeParagraph() {

    if (inParagraph) {
        cout << CONV_PARAGRAPH_POSTAMBLE << endl;
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
