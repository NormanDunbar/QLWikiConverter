// As the default translation file for DokuWiki adds the Page Title as
// a heading 1 to the generated pages, we do have some pages with two
// consecutive H1s whihc looks bad.
// This utility will list all those pages whihc have such a thing and
// allow the maintainer (aka, me!) to fix those that need it.
//
//      fixHeadings input_file[s].
//
// Norman Dunbar.
// 23rd August 2017.

#include "main.h"



//------------------------------------------------------------------
// It all kicks off here.
//------------------------------------------------------------------
int main(int argc, char *argv[])
{
    cerr << endl
         << programName << " - version: " << VERSION << endl;

    // Validate parameters.
    if (argc < 2) {
        cout << "Not enough parameters supplied." << endl;
        usage();
        return ERR_BP;
    }

    // Looks good, let's go!
    for (auto x = 1; x < argc; x++) {
        lineNumber = 0;
        fileName = argv[x];

        mIfs = new ifstream(fileName);

        if (!mIfs->good()) {
            cerr << "File: '" << argv[x] << "' cannot be opened/not found." << endl
                 << "\twill be ignored." << endl;
        }

        if (jfdi() != ERR_OK) {
            cerr << "File: '" << argv[x] << "' did not process properly." << endl
                 << "\tsee earlier errors or warnings." << endl;
        }

        mIfs->close();
    }
}


//------------------------------------------------------------------
// The main processing is done here. It's simple.
// Read the first non-blank line from the input file.
// Then read the next non-blank line.
// If they are both Heading 1 ("====== text ======") then list the
// details.
//------------------------------------------------------------------
int jfdi()
{
    string *lineOne = new string();   // One line of input.
    string *lineTwo = new string();   // Another line.

    if (mIfs->good()) {

        // Read until first non-blank line read.
        while (readInputFile(lineOne)) {
            if (!lineOne->empty()) {
                break;
            }
        }

        // Read until next non-blank line read.
        while (readInputFile(lineTwo)) {
            if (!lineTwo->empty()) {
                break;
            }
        }

        // We have two non-blank lines. Trim leading spaces.
        string::size_type firstPos = lineOne->find_first_not_of(" ");
        if (firstPos != string::npos) {
            *lineOne = lineOne->substr(firstPos);
        }

        // We have two non-blank lines. Trim leading spaces.
        firstPos = lineTwo->find_first_not_of(" ");
        if (firstPos != string::npos) {
            *lineTwo = lineTwo->substr(firstPos);
        }

        // Headings?
        bool lineOneHeading = false;
        bool lineTwoHeading = false;

        if (lineOne->size() >= 6) {
            if (lineOne->substr(0, 6) == "======") {
                lineOneHeading = true;
            }
        }

        if (lineTwo->size() >= 6) {
            if (lineTwo->substr(0, 6) == "======") {
                lineTwoHeading = true;
            }
        }

        if (lineOneHeading && lineTwoHeading) {
            cout << fileName << endl
                 << "[1] " << *lineOne << endl
                 << "[2] " << *lineTwo << endl;
        }
    }

    delete lineOne;
    delete lineTwo;

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
         << "\t" << programName << " input_file [input_file ...]" << endl << endl;
}
