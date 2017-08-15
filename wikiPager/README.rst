=====================================
QL Wiki Converter - wikiPager Utility
=====================================

Introduction
============

In this folder you will find:

**wikiPager** - a utility, or at least, the C++ source code for same, which reads the file created by the MySQL script ``content.sql`` and creates IN THE CURRENT FOLDER an individual text file for EACH and EVERY Wiki page found in the file created by the afore mentioned script. Only the source exists - ``main.cpp`` plus a few other header files etc. These will need compiling as explained below.

Compiling
=========

The code uses only standard C++ and the STL (Standard Template Library) to do the hard work. 

Compiling on Linux
------------------

..  code-block:: none

    cd wikiPager
    mkdir bin
    g++ -o bin/wikiPager -std=c++11 main.cpp

Compiling on Windows
--------------------

..  code-block:: none

    cd wikiPager
    mkdir bin
    CXX -o bin\wikiPager main.cpp

Where ``CXX`` is the executable for your installed C++ compiler. For example, Embarcadero's free C++ compiler is called ``bcc32c.exe`` and if it is on the %PATH%, it can be called simply as ``bcc32c``.

Execution
=========

After compiling, the file ``wikiPager`` in the ``bin`` folder is the utility to run against the single output file created by the ``content.sql`` script. If we assume that content.out is the file created from the database, by the script, then execute ``wikiPager`` as follows:

..  code-block:: none

    cd /location/of/content.out
    mkdir temporaryFiles
	cd temporaryFiles
	.path/to/bin/wikipager ../content.out


A list of files being created will scroll up the screen as each one is created and written **in the current directory**. These individual pages files represent the individual pages in the QL Wiki. Once complete, these are your source pages that the utility ``rwapWiki`` can be used to convert into some other format.

The current folder will now be filled with one file named ``Page Title``.``language``.``txt`` for each of the Wiki pages found in the ``content.out`` file. The filenames are slightly massaged to remove punctuation and spaces which is handy as it will then match up to what the ``rwapWiki`` utility creates when processing Wiki Page Links in the source files.

