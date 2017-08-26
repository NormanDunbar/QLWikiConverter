// Reads an output file from mysql which has the following details:
//
// ********************** etc
// wikilang: Language code (en, fr, etc)
// wikipage: Page Title
// wikiText: yada yada yada
// more lines here
// ...
// ********************** etc
// The final record's output DOES NOT have a trailing line of asterisks.
//
//
// listWiki input_file look_for [> output_file]
//
// Each page, which contains at least on of the "look_for" features
// will be listed and details of the "look_for" will be displayed.
//
// Look_For is one, only, of the following:
//
// ACRONYM, ANCHOR,
// BLOCKQUOTE, BOLD,
// CITATION, CODEBLOCK,
// DEFINITIONLIST,
// FORCEDLINEFEED,
// LINK,
// HEADING1, HEADING2, HEADING3, HR,
// IMAGE, IMAGEGALLERY, INLINECODE, ITALIC,
// ORDEREDLIST,
// REDIRECTION, REFERNECE,
// TABLE,
// UNORDEREDLIST,
// PAGELINK,
// VIDEOLINK.
//
//
// Norman Dunbar.
// 28th July 2017.

#include "main.h"



//------------------------------------------------------------------
// It all kicks off here.
//------------------------------------------------------------------
int main(int argc, char *argv[])
{
    cerr << endl
         << "listWiki - version: " << VERSION << endl;

    // Validate parameters.
    if (argc < 3) {
        cout << "Not enough parameters supplied." << endl;
        usage();
        return ERR_BP;
    }

    // Looks good, let's go!
    // Get the input file first.
    lineNumber = 0;
    mIfs = new ifstream(argv[1]);
    if (!mIfs->good()) {
        cerr << "File: '" << argv[1] << "' not found." << endl
             << "Cannot continue." << endl;
        return ERR_BP;
    }

    // Get the feature to look for and convert to a search string.
    string param2 = argv[2];

    // Upper case it.
    for (string::iterator x = param2.begin(); x != param2.end(); x++) {
        *x = toupper(*x);
    }

    // Is it a valid choice?
    paramMapIter i = lookForThese.find(param2);
    if (i == lookForThese.end()) {
        cerr << "ERROR: '" << param2 << "' is not a valid option." << endl;
        usage();
        return ERR_BP;
    }

    // If we are looking for images, do we want normal or wget output?
    if (param2 == "IMAGE") {
        if (argc == 4) {
            string wget = argv[3];
            // Upper case last parameter.
            for (string::iterator x = wget.begin(); x != wget.end(); x++) {
                *x = toupper(*x);
            }

            // Check.
            if (wget != "WGET") {
                cerr << "ERROR: '" << wget << "' is not a valid option for IMAGE searches." << endl;
                usage();
                return ERR_BP;
            }

            // Valid. Set the wget flag.
            wgetWanted = true;
        }
    }


    // It's valid, extract the look for stuff from the second
    // member of the std::pair. It's a std::map thing!
    lookForThis = i->second;

    cerr << "Searching for " << param2 << "(s), in file: '"
         << argv[1] << "'" << endl << endl;

    // Go look!
    return jfdi();
}


//------------------------------------------------------------------
// The main program loop. Reads the input file until EOF, then exits
// with an appropriate code. Processes each line as it is read.
//
// Ignore data up to the first record separator line.
// Read the following two lines and get the language and page title.
// Create an output file named "PageTitle.LanguageCode.txt".
// Read and outut the remaining lines until EOF or the next record.
// The very first line of content has "wikitext:" at the start.
//
// Example:
//
// *************************** 1. row ***************************
// wikilang: en
// wikipage: 2488
// wikitext: 2488 is an additional set of printer drivers for ...
//------------------------------------------------------------------
int jfdi()
{
    string *aLine = new string();   // One line of input.
    string pageLang;                // Wiki page language code.
    string pageTitle;               // Wiki Page title.
    string textFile;                // Output file name.

    // Makes sure we can call aLine->at(0).
    aLine->assign("Hello World!");

    // Preload the "parser" with the first record separator line.
    while (mIfs->good()) {
        readInputFile(aLine);

        if (aLine->size() >= 10) {
            if (aLine->substr(0, 10) == "**********") {
                break;
            }
        }
    }

    // We have the first record's separator line. From here
    // until EOF, read the following two lines (lang & title)
    // Create an output file name, then scan this record's content
    // for the desired feature of the Wiki.
    while (mIfs->good()) {

        // A new record has been found.
        // Get the language code for this page & check it.
        readInputFile(&pageLang);
        if (pageLang.substr(0, 9) != "wikilang:") {
            cerr << "listWiki: Sync error at line " << lineNumber
                 << ". Expected 'wikilang: ' but found '"
                 << pageLang.substr(0, 10) << "'." << endl;
            return ERR_BF;
        }

        // Get the page name for this page & check it.
        readInputFile(&pageTitle);
        if (pageTitle.substr(0, 9) != "wikipage:") {
            cerr << "listWiki: Sync error at line " << lineNumber
                 << ". Expected 'wikipage: ' but found '"
                 << pageTitle.substr(0, 10) << "'." << endl;
            return ERR_BF;
        }

        // Make sure the page name is legal AND matches that of
        // what it will become after rwapWiki is run.
        for (string::iterator x = pageTitle.begin(); x != pageTitle.end(); x++) {
            if ((ispunct(*x)) ||
                (isspace(*x)))
            {
                *x = '-';
            }
        }

        // Create the expected output file name as would be
        // created by 'wikiPager'.
        textFile = pageTitle.substr(10) + "." +
                   pageLang.substr(10) + ".txt";

        // Read the first content line & check it.
        readInputFile(aLine);
        if (aLine->substr(0, 9) != "wikitext:") {
            cerr << "listWiki: Sync error at line " << lineNumber
                 << ". Expected 'wikitext: ' but found '"
                 << aLine->substr(0, 10) << "'." << endl;
            return ERR_BF;
        }

        // Trim off the "wikitext: " text.
        *aLine = aLine->substr(10);

        // Check for end of current record.
        while (true) {

            // Exit at EOF.
            if (!mIfs->good()) {
                break;
            }

            // Empty lines are ignored.
            if (!aLine->empty()) {

                // Remove Windows CR characters.
                string::size_type crPos = aLine->find("\r");
                while (crPos != string::npos) {
                    aLine->erase(crPos, 1);
                    crPos = aLine->find("\r");
                }

                // Are we done with the current wiki page?
                if ((aLine->size() >= 10) &&
                    (aLine->substr(0, 10) == "**********"))
                {
                    break;
                }

            }

            // Can we find the desired Wiki feature?
            if (lookForThis.lookAtStartOfLine) {
                // Look at the start of the line only.
                for (vsi x = lookForThis.findThis.begin(); x != lookForThis.findThis.end(); x++) {
                    if (aLine->length() < (*x).length()) {
                        // Shorter than the text we want, ignore.
                        continue;
                    }

                    if (aLine->substr(0, (*x).length()) == *x) {
                        // Found it. Subtract 2 as the content starts on line 3.
                        cout << textFile << ": Line: " << lineNumber - 2 << ", \"" << *aLine << '"' << endl;
                    }
                }
            } else {
                // Look anywhere in the line.
                for (vsi x = lookForThis.findThis.begin(); x != lookForThis.findThis.end(); x++) {
                    if (aLine->length() < (*x).length()) {
                        // Shorter than the text we want, ignore.
                        continue;
                    }

                    if (aLine->find(*x) != string::npos) {
                        // Found it. Subtract 2 as the content starts on line 3.
                        // Normal output:
                        if (!wgetWanted) {
                            cout << textFile << ": Line: " << lineNumber - 2 << ", \"" << *aLine << '"' << endl;
                        } else {
                            // Wget output. We definitely have a startPos.
                            string::size_type startPos = aLine->find(*x);

                            // Look for a pipe to indicate the end of the image URL.
                            string::size_type endPos = aLine->find("|", startPos + 2);

                            // No pipe, look for the closing "))" instead. Must be found.
                            if (endPos == string::npos) {
                                endPos = aLine->find("))", startPos + 2);
                            }

                            // Write out in wget format.
                            cout << aLine->substr(startPos + 2, endPos - startPos - 2) << endl;
                        }
                    }
                }
            }

            // Fetch next line of the page text.
            readInputFile(aLine);

        } // end while (true)

        // Finished with this textFile, reset lineNumber.
        lineNumber = 0;

    } // end while (mIfs->good())

    // Recover allocated storage for the input lines.
    if (aLine) {
        delete aLine;
    }

    // All done, no errors.
    return ERR_OK;
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
// Display details of how to use the utility if we encounter a lack
// of parameters, or, an incorrect one.
//------------------------------------------------------------------
void usage() {
    cerr << "USAGE: " << endl << endl
         << "\tlistWiki input_file look_for [ wget ] [ > output_file ]" << endl << endl
         << "LOOK_FOR is one of the following:" << endl << endl
         << "\tACRONYM, ANCHOR," << endl
         << "\tBLOCKQUOTE, BOLD," << endl
         << "\tCITATION, CODEBLOCK," << endl
         << "\tDEFINITIONLIST," << endl
         << "\tFORCEDLINEFEED," << endl
         << "\tLINK," << endl
         << "\tHEADING, HR," << endl
         << "\tIMAGE, IMAGEGALLERY, INLINECODE, ITALIC," << endl
         << "\tORDEREDLIST," << endl
         << "\tREDIRECTION, REFERNECE," << endl
         << "\tTABLE," << endl
         << "\tUNORDEREDLIST," << endl
         << "\tPAGELINK," << endl
         << "\tVIDEOLINK." << endl
         << "\tNote: 'wget' is only allowed on IMAGE searches and outputs a list" << endl
         << "\tof images only suitable for running as input to the wget utility." << endl;
}
