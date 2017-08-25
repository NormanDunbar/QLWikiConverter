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
// rwapPager input_file
//
// Each record in the input file, will be written out to a separate
// text file named "PageTitle.LanguageCode.txt"
//
// These files can then be read by rwapWiki to produce output pages
// converted to some other use.
//
//
// Norman Dunbar.
// 28th July 2017.

#include "main.h"

using namespace std;

//------------------------------------------------------------------
// Globals.
// Yes, I know! Don't start! Otherwise I'll use a GOTO as well!
//------------------------------------------------------------------

uint32_t lineNumber = 0;    // Input file, current line number.
ifstream *mIfs;             // Input file aka argv[1].

#define ERR_OK 0            // No errors.
#define ERR_BP 1            // Bad parameter - Cannot open input file.
#define ERR_BF 2            // Input file isn't as expected format.


//------------------------------------------------------------------
// It all kicks off here.
//------------------------------------------------------------------
int main(int argc, char *argv[])
{
    cerr << endl
         << "wikiPager - version: " << VERSION << endl;

    // Validate parameters.
    if (argc < 2) {
        cout << "Not enough parameters supplied." << endl
             << "USAGE:" << endl << endl
             << "\twikiPager input_file" << endl;
        return 1;
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

    return jfdi();
}


//------------------------------------------------------------------
// The main program loop. Reads the input file until EOF, then exits
// with an appropriate code. Processes each line as it is read.
//
// Ignore data up to the first record separator line.
// Read the following two lines and get the language and page title.
// Create an output file named "PageTitle.LanguageCode.txt".
//
// The first line written to the output will be the original page
// name. After that we simply read and write the remaining lines
// until EOF or the next record.
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
    string pageFileName;            // Wiki Page title to create textFile from.
    string pageName;                // Original title on old Wiki.
    string textFile;                // Output file name.
    ofstream *mOfs = NULL;          // Output file.

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
    // Create an output file, then write this record's content
    // to it. Finish at the next separator or at EOF.
    while (mIfs->good()) {

        // A new record has been found.
        // Get the language code for this page & check it.
        readInputFile(&pageLang);
        if (pageLang.substr(0, 10) != "wikilang: ") {
            cerr << "wikiPager: Sync error at line " << lineNumber
                 << ". Expected 'wikilang: ' but found '"
                 << pageLang.substr(0, 10) << "'." << endl;
            return ERR_BF;
        }

        // Get the page title  for this page & check it.
        readInputFile(&pageFileName);
        if (pageFileName.substr(0, 10) != "wikipage: ") {
            cerr << "wikiPager: Sync error at line " << lineNumber
                 << ". Expected 'wikipage: ' but found '"
                 << pageFileName.substr(0, 10) << "'." << endl;
            return ERR_BF;
        }

        // Make sure the page title is legal AND matches that of
        // what it will become after rwapWiki is run. Save the original
        // page title as the page name first though - it's needed, unchanged.
        pageName = pageFileName.substr(10);

        // Copy it back, minus "wikipage: ".
        pageFileName = pageName;

        // We do this backwards as we might need to drop punctuation characters.
        // This is based on DokuWiki where punctuation is lost and spaces become
        // underscores.
        // end() returns one past the end, so -- first!
        for (string::iterator x = --pageFileName.end(); x >= pageFileName.begin(); x--) {
            if (ispunct(*x)) {
                pageFileName.erase(x);
            } else {
                if (isspace(*x)) {
                    *x = '_';
                }
            }
        }

        // Create a new output file.
        textFile = pageFileName + "." +
                   pageLang.substr(10) + ".txt";

        cout << "Writing: '" << textFile << "'." << endl;

        mOfs = new ofstream(textFile);
        if (!mOfs->good()) {
            cerr << "Cannot open '" << textFile << "' for writing." << endl;
            return ERR_BF;
        }

        // Write the page title.
        *mOfs << "PageName: " << pageName << endl;

        // Read the first content line & check it.
        readInputFile(aLine);
        if (aLine->substr(0, 9) != "wikitext:") {
            cerr << "wikiPager: Sync error at line " << lineNumber
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

            // Empty lines are a PITA!
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

            // So far, so good, write to the output file.
            *mOfs << *aLine << endl;

            // Fetch next line of the page text.
            readInputFile(aLine);

        } // end while (true)

        // Close the current output file.
        if (mOfs->is_open()) {
            mOfs->close();
            delete mOfs;
            mOfs = NULL;
        }

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
