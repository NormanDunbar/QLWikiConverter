====================================
QL Wiki Converter - rwapWiki Utility
====================================

Introduction
============

In this folder you will find:

**RwapWiki** - a utility which reads a single page file, created by WikiPager, and converts it to almost any output format you desire - provided you tell it which conversion format you need it to be in when complete. Only the source exists - ``main.cpp`` plus a few other header files etc. These will need compiling as explained below.

Compiling
=========

The code uses only standard C++ and the STL (Standard Template Library) to do the hard work. 

Compiling on Linux
------------------

..  code-block:: none

    cd rwapWiki
    mkdir bin
    g++ -o bin/rwapWiki -std=c++11 main.cpp

Compiling on Windows
--------------------

..  code-block:: none

    cd rwapWiki
    mkdir bin
    CXX -o bin\rwapWiki main.cpp

Where ``CXX`` is the executable for your installed C++ compiler. For example, Embarcadero's free C++ compiler is called ``bcc32c.exe`` and if it is on the %PATH%, it can be called simply as ``bcc32c``.

Execution
=========

After compiling, the file ``rwapWiki`` in the ``bin`` folder is the utility to run against the individual page files created by the utility ``wikiPager``. You will need a conversion file to create the desired format for the output files.

There's an HTML example, ``convert.html.txt`` file provided and a blank template to allow you to create your own. The example supplied converts the Wiki pages to HTML files.

The utility can hopefully be used to output text formats for other Wikis, or whatever format you need the Wiki Pages to be - text? LaTeX? XML? etc. It's up to you to provide the (``printer.dat``!) type file to make the conversions.

**WARNING 1**: ``rwapWiki`` cannot handle Image Galleries, if a Wiki page has one of these, you get a warning to the standard error channel. So far, there are no galleries on the QL Wiki.

**WARNING 2**: Because you need to download images used in the Wiki pages, there is a list of all images found, and their URL, produced on the standard error channel as well. The desired output is written to the standard output channel and should be redirected to a suitable output file at your heart's desire.**





