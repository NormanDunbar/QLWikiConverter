=======================================
Converting the QL Wiki to Other Formats
=======================================


Introduction
============

For full details, see `this topic <http://qlforum.co.uk/viewtopic.php?f=12&t=1832>`_ on `The QL Forum <http://qlforum.co.uk>`_.

Currently the QL Wiki, amongst others, is hosted and maintained by Rich of RWAP Services. It has been noted that other Wikis etc have foundered on the basis of "something" happening to the one single maintainer. Rich wants to avoid this happening.

To this end, a number of suggestions have been put forward on the matter (read the whole topic at the link above) and conversion to another format was suggested. Where and how (or who) would be hosting the converted Wiki is/was/has yet to be decided, as has the format - there are Wikis aplenty out there!

To make the task of converting the existing, and not really well known, *WiClear* Wiki (`http://wiclear.free.fr/ <http://wiclear.free.fr/>`_) source code from the format used by WiClear to some other format, I wrote this utility. It is possibly more important as the WiClear software has not been updated since 2007 and who knows what bugs or security problems exist?

*What other format* being the question. There have been a few suggested and a couple of trials have seemingly taken place. The utility, ``rwapWiki`` attempts to solve this problem by allowing ahem, *almost* any other format you desire - provided a translation file can be created for it.

This document attempts to explain how the system works, what it converts and how. This might make writing new translation files a little easier. The utility is supplied with a single, text, HTML translation file which works *reasonably* well - see the section on *Problem Areas Identified* below.

    **Update:** As of 18th August 2017, a new example file is also supplied. This one is ``convert.DOKUWIKI.txt`` and converts the WiClear source to that required by the DokuWiki system. The creation of this new file is described below in the section *A Worked Example - DokuWiki*.


WorkFlow
========

This section explains the process of converting the *latest* version of the QL Wiki, to some other format.

Briefly
-------

The workflow is simple:

-   Drop the existing wiki database, if it exists. It is a standard MySQL database.
-   Import the latest database dumpfile.
-   Export the content in a specific format.
-   Split the output into page files - one per Wiki Topic.
-   Convert the required page files to the desired format.
-   Use the converted files in some appropriate manner.

The latter stage may well require that you build a conversion file if one is not provided for you.

In Detail
---------


-   Run mySQL (or MaraiaDB) and, as the root user (or another user with drop database privileges), run the following:

    ..  code-block:: none
    
        mysql -u root -p
        
        show databases;
        
        +----------------------+
        | Database             |
        +----------------------+
        | ...                  |
        | rwapadve_ql_software |
        +----------------------+

        drop database rwapadve_ql_software;
        commit;
        exit;

    You will be prompted for the root password when you first connect.
        
-   Run mySql again, to create a new database from the latest SQL dump file:

    ..  code-block:: none
    
        mysql -u root -p <ql_wiki_2017_07_28.sql
        
    You will be prompted again for the root password. When the script finishes, the *rwapadve_ql_software* database will have been created again. You will be disconnected automatically at the end of the script.
    
    
-   Run mySQL again, this time to extract the content. You will need the file ``content.sql`` from the ``MySQL`` folder.
        
    ..  code-block:: none

        mysql -u root -p <content.sql >content.out

    The file ``content.out`` now contains the whole of the Wiki (including the sandbox and tutorial areas - c'est la vie!) in a format that the ``wikiPager`` utility understands.
        
-   Run the ``wikiPager`` utility against the ``content.out`` file to create numerous files, each one containing the Wiki source code for a single Wiki topic. The files will be created in the current directory, so it is advisable to create a new working directory first, and change into it before running the utility, as follows:

    ..  code-block:: none

        mkdir Pages
        cd Pages
        /path/to/wikiPager ../content.out
        cd ..

    There will be one file created for every topic in the QL Wiki. The page file name is of the format ``Page-Title``.``language``.``txt``, with the page title (from the MySQL database) being *massaged* to replace spaces and punctuation with hyphens - this is mostly to assist in creating page files on Windows as it can be a wee bit precious about the characters that can be used in file names. Letter case in the page title is retained.
    
    At present, most of the Wiki is in English, and there are a few pages in French or Dutch languages. These are ignored (sorry) for the rest of this document - I'm doing what the British do best, ignoring every other language in the world!
    
-   The ``Pages`` directory contains the files we wish to convert to *some other format*. The rest of this example will assume HTML is desired, as that's what I did my testing with! The utility ``rwapWiki`` is the one which reads each page file, and creates an output in the desired format. To do this, it has to know what to convert the original Wiki source code into for the various features of the Wiki source language. Details of what is converted are to be found below, as are details on how to build your own conversion file.

    ..  code-block:: none

        mkdir HTML
        /path/to/rwapWiki Pages/inputfile.en.txt /path/to/convert.HTML.txt >HTML/inputfile.html
    
    The file ``inputfile.en.txt`` in the ``Pages`` directory will be converted according to the rules in ``convert.HTML.txt`` and written to standard output, where it is redirected to the file ``inputfile.html`` in the newly created ``HTML`` directory. The output file should be named similarly to the input file, only the ``.en.txt`` extensions are changed to ``.html`` (in this case) as that is what the conversion file, ``convert.HTML.txt`` decrees.
    
    Repeat as necessary for every single file in the Pages directory, or at least, for the ones you are interested in.

-   In this case, we have converted the page files to HTML, so we can use a browser to examine the results. Normally, the Wiki starts by displaying the first topic, with happened to be the page with the title 'Sinclair QL Home Computer', so we must point our browser at the file ``HTML/Sinclair-QL-Home-Computer.html`` and follow the links from there.

Batching the Conversion
-----------------------

Converting each and every page in the above manner could get a little tiresome. There has to be a better way - and there is. On Linux, it is a simple matter. Windows, as per the norm, seems to go out of its way to make life difficult. Read on.

Linux
~~~~~

It is assumed that you are located in the directory where you have the ``Pages`` and the ``HTML`` directories. Type the following at the prompt to convert everything in ``Pages`` in the English language to an HTML file in the ``HTML`` directory:

..  code-block:: bash

    for x in `ls Pages/*.en.txt`
    do
        infile="${x}"
        outfile=`basename "${x%.en.txt}.html"`
        /path/to/rwapWiki ${infile} /path/to/convert.HTML.txt >HTML/${outfile}
    done

Once completed, Open ``HTML/Sinclair-QL-Home-Computer.html`` in your browser, and follow the links from there.    
    

Windows
~~~~~~~

This is definitely not as simple as it is on Linux!

First we need a small batch file to do the conversion for a single file, this is ``PageIt.cmd``:

..  code-block:: batch

    @echo off
    
    set INFILE=%1
    set OUTFILE=%INFILE:.en.txt=.html%
    rwapWiki\rwapWiki Pages\%INFILE% rwapWiki\convert.HTML.txt >HTML\%OUTFILE%

We need this file because a ``FOR /F`` loop in Windows doesn't seem to be able to cope with being able to run more than one command. There are no errors, but the second and subsequent commands can no longer see the value passed in ``%F``.

Now we need a loop to read the files from the ``Pages`` directory, and convert each one by calling the ``PageIt.cmd`` file created above. **Warning**: The following *only* works if you are typing directly into a command window:

..  code-block:: batch

    for /F "usebackq tokens=1" %F in (`dir /B Pages\*.en.txt`) do (PageIt.cmd %F)

The single quotes used above are back ticks (`) and not the familiar single quotes (') we know and love for strings in the QL. They don't tend to work the same! 

Did I mention that ``%F`` above is case sensitive? You must use the same case each time you type it. Also, it can only be a single letter long - Anyone remember ZX-81 variables? I'm sure that they were the same! Progress eh?
    
If you want to do the above in a command file, then create one, ``PageAll.cmd`` for example, with the following command in it:

..  code-block:: batch

    for /F "usebackq tokens=1" %%F in (`dir /B Pages\*.en.txt`) do (PageIt.cmd %%F)
    
You have to double up the percent signs from one only, in command mode, to two in batch mode, to get a working command file. Sigh. As above, ``%%F`` is still case sensitive.

Once completed, Open ``HTML/Sinclair-QL-Home-Computer.html`` in your browser, and follow the links from there.    
        

Problem Areas Identified
========================

The following list outlines a number of conversion problems that I have come across in testing. I'm assuming that there are others and that some of them will perhaps, not apply in other conversion formats. Testing has been done on HTML conversions - so far!

It's plainly obvious that no conversion program will be 100% able to convert everything, but having something converted with a couple of foibles is much better than having to convert everything by hand.

Wiki Page Links
---------------

Browsing around the Wiki pages, converted to HTML, works for most pages and links, however, some pages are referenced (linked to), but do not actually exist as pages in the original Wiki. These links will inevitably fail to work. There is nothing that can be done as there is no page to link to. One example of this is the *Screen Compression Utility* link on the *Dilwyn Jones* page (``HTML/Dilwyn-Jones.html``.)

On Linux, where filenames are case sensitive, some of the links in the QL Wiki, to other pages in the Wiki, are spelt with lower case names while the actual page has an upper or mixed case name. This causes those links to fail on Linux because the link and the file it references are differently named. One example of this is again on the *Dilwyn Jones* page. There is a link to all known QL links. The original page is named *Links* but the link to the page is *links* and the two are not the same on Linux. On Windows, this is not a problem and the *links* link, links to the *Links* page as expected.

Pages with Accented Characters
------------------------------

Page names which have accented characters cause no problems on Linux, as such characters are permitted in file names (Assuming UTF8 is in use, which is *usually* the case by default). 

The filenames generated on Windows, however, are different as it doesn't seem to be able to cope with some of the the accented characters. For example, the page entitled *Jürgen Falkenberg* has the 'u' with an umlaut above it. On Linux this is fine, on Windows it generates a file named 'JÃ¼rgen-Falkenberg.html' instead where the 'A' has a tilde (~) above it and the '1/4' is a single character. Links in the generated files correctly go to the file with the accented 'u' as per Linux, but the page cannot be found as it doesn't exist with that name.

The problem exists for the following pages:

-   Hüthig-Verlag
-   Jürgen Falkenberg
-   Janko Mrsic-Flögel
-   Schön (although there are numerous links to this page in the Wiki, the page itself is not actually present.)
-   Schön KBL128 QL Case

There *may* be others.

Image Galleries
---------------

These cannot be converted. Sorry. However, as there are none in the QL Wiki, it's not a problem. There is one in the sandbox area, but that's for messing about in, so it's not considered a problem, and is not linked to from any of the QL pages.

Images
------

The original Wiki required that images were set up as a URL linking to images that existed *somewhere on the internet*, there was no apparent ability to load images from local files. The conversion process will list all the images it finds, but will not (can not) download them. Image links in the converted files will refer to the original locations - or as decreed by the conversion's translation file.

Equally, the description for an image (the text that appears when you hover over it) will either be some descriptive text, or, a link to a text file *somewhere on the internet* where the descriptive text can be found in the named file. Again, the conversion program will list the descriptive text or filenames as appropriate as part of the conversion.

If you decide to download the individual images locally, you will obviously need to edit the generated HTML files to change the "src=" attribute on the "<img>" tags accordingly. Please look in the IMAGES/wget_list.txt file for a full list of all the images that are linked to in the QL Wiki (as of 28th July 2017.) This file can be used as follows:

..  code-block:: bash

    mkdir Images
    cd Images
    wget -i /path/to/wget_list.txt
    
When completed, there should be a number of images - around 600 - in the ``Images`` directory. In testing, I've had a few problems as I'm at work and the proxy server filters out a lot of files, leaving me with a lot of JPEG image files (``*.jpg``) containing a 2 by 1 white pixel in GIF format! Coincidentally, I get a similar image for a lot of the Wiki pages when I attempt to open the image URL directly from my browser, so it's definitely a work problem!   


How it Works
============

The Content.sql Script
----------------------

The script extracts all the content pages from the database. Only the most recent pages are extracted - there is no history taken from the database. The output file is written in a special format, as follows:

..  code-block:: none


    *************************** 1. row ***************************
    wikilang: en
    wikipage: 2488
    wikitext: 2488 is an additional set of printer drivers ...
    yada yada yada
    Etc etc etc
    *************************** 2. row ***************************
    ...

Each record is surrounded by the rows of asterisks except for the final one, which is terminated by the end of file.

The text ``wikilang``, ``wikipage`` and ``wikitext`` are used by the ``wikiPager`` utility to output the correct data to the correctly named files. 

WikiPager
---------

From the above output file, created by the ``content.sql`` script, ``wikiPager`` splits the input stream into separate pages. It does this by looking for the record separators - the rows of asterisks, and then reads the next two lines to get the language code (``en`` above) and the page name (``2488`` above.) These are used to create the output file in the format ``Page-Title``.``language``.``txt``. The text of the page is then extracted from the third record onwards, and written to the output file.

File names are adjusted to convert punctuation and spaces into hyphens as this avoids errors on Windows systems which has a few ideas about what constitutes a valid file name!

Files are created in the current directory, so it is advisable to create a new directory and change into it before running the utility.

When complete, you should have over 1,000 separate pages, each on containing a single Wiki topic. All languages are extracted but in the main, the vast majority are in English, with only a few in French and Dutch.

RwapWiki
--------

This utility reads a conversion file containing special bits of text etc, or blanks, that will be used when certain features of the Wiki source language are detected in the input file. The conversion file is described below, in some detail, and will not be discussed here any further.

The input file is opened, and read line by line. For each line read, the following processes are carried out:

-   Pre-load the parser by reading the first line of the input file.
-   Main loop starts here by checking for EOF and exiting the loop if found.
-   Process Lists - then start the main loop again. These read an extra line form the file, so they just start the main loop again.
-   Process Code Blocks - then start the main loop again. These read an extra line form the file, so they just start the main loop again.
-   Process Table Rows - then start the main loop again. These read an extra line form the file, so they just start the main loop again.
-   Process lines with special characters ate the start of the line. If nothing was processed, because the line didn't have a special character at the start, then look for embedded sequences of special characters.
-   Write out the fully reformatted line.
-   Read the next input line.
-   End of the main loop.

From the above, you can see that Lists, Code Blocks and Table Rows end up, when completed, by having read one too many lines from the input file, so they avoid the tail end of the main loop and simply restart, checking for EOF before running through any required processing again.

If a line had any special characters at the start, it is processed (see below) and then finishes the main loop, however,. if the line didn't do an processing, it is processed by  the code that looks for embedded characters before ending the main loop with another read from the input file.

The following section outlines, in some details, the processing of each different type of feature that gets carried out, and converted, from the original Wiki source code.

ListWiki
--------

This utility reads a conversion file, created by ``wikiPager``, and attempts to find all occurrences of a particular Wiki Feature, such as citations, acronyms etc. The full list is:

-   ACRONYM, ANCHOR
-   BLOCKQUOTE, BOLD
-   CITATION, CODEBLOCK
-   DEFINITIONLIST
-   FORCEDLINEFEED
-   HEADING, HR
-   IMAGE, IMAGEGALLERY, INLINECODE, ITALIC
-   LINK
-   ORDEREDLIST
-   PAGELINK
-   REDIRECTION, REFERENCE
-   TABLE
-   UNORDEREDLIST
-   VIDEOLINK

The code doesn't get it right each and every time, but it's useful in finding stuff that might not be acting correctly when a file is converted.

While the utility reads the output file from the ``content.sql`` script , it displays its finding as if it had been scanning the individual files output by the ``wikiPager`` utility, with the correct line numbers etc.



What Gets Converted
===================

Line Starts
-----------

The following features of the Wiki have special characters at the start of a line - hence *Line Starts* - to indicate what the line is to be used as. ``RwapWiki`` deals with these line starts, and when done, then tries to process any (valid) embedded features left in the translated line. These are covered below in the *Embedded Formatting* section.

When a line start is processed, we cannot be in a paragraph any longer, so if we were in one, we close it

Headings
~~~~~~~~

Heading lines start with one, two or three exclamation marks (!). These indicate the heading level, with '!!!' indicating level 1 and '!' indicating level 3. Only three levels of heading are permitted. There may be embedded formatting such as bold or italic, so after conversion here, the line is processed for any valid embedded formatting. (See below for details).

A heading from the input file will resemble the following, but note that I have added spaces to make the reading easier:

..  code-block:: none

    !! Heading Text <EOL>
    
Where '<EOL>' is the end of line, which can be ``CR``, ``CRLF`` or ``LF``.

This shows a level 2 heading, so the line is reformatted to be as follows:

..  code-block:: none

    <CONV_H2_PREAMBLE>Heading Text<CONV_H2_POSTAMBLE>
    
The 'CONV' variables are read from the conversion file. In the case of HTML these will be ``<h2>`` and ``</h2>`` as you would expect. Similar variables exist for the level 1 and level 3 headings.

Horizontal Rules
~~~~~~~~~~~~~~~~

No special processing is required for horizontal rules, we simply discard the input line - which consists of 4 or more equals signs (=) in a row followed by a line feed - and set the line to contain only the following variables from the conversion file:

..  code-block:: none

    <CONV_HR_ON><CONV_HR_OFF>
    
For the example HTML conversion, this will be blank for ``CONV_HR_OFF`` and ``<br>`` for ``CONV_HR_ON``.

Block Quotes
~~~~~~~~~~~~

A block quote line in the input file will resemble the following:

..  code-block:: none

    > yada yada yada <EOL>
    
There may be more than one block quote line, but we only process them one by one in the main loop.


All we do with each of the lines is to strip off the leading '>' and convert it to the following, (all on one line):

..  code-block:: none

    <CONV_BLOCK_QUOTE_PREAMBLE> <CONV_BLOCK_QUOTE_LINE_ON> ... <CONV_BLOCK_QUOTE_LINE_OFF> <CONV_BLOCK_QUOTE_POSTAMBLE>
    
Where '...' represents the block quoted text.

    **NOTE**: The double arrow at the start of the second line above is *not* part of the conversion, it merely indicates that the code line has had to be split to fit it onto the page and that it should be all on a single line.

We have two prefix variables and two suffix variables to content with as some output formats may require an prefix/suffix for the block quote, and an prefix/suffix for each line of text within. HTML requires this, as follows:

..  code-block:: html

    <blockquote><p> Your text </p></blockquote>


Line Includes
-------------

Any line that is not processed as a *Line Start* line, is then checked for and processed according to whether or not it contains embedded formatting. Some of the valid embedded formatting that can occur is detailed below in the section entitled *Embedded Formatting*.

Paragraphs
~~~~~~~~~~

Paragraphs are simply lines of text, terminated by a pair of end of line sequences as appropriate for the Operating System used to create the page of text in the Wiki. For Windows this will be ``crlf crlf``, while Linux (and Mac?) will have ``lf lf``. (Without any spaces of course.)

Code Blocks
~~~~~~~~~~~

Code blocks, as opposed to *Inline Code* which is dealt with elsewhere, are indicated as follows:

..  code-block:: none

    <space> Line of code <EOL>
    <space> Another line of code <EOL>
    <space> A further line of code <EOL>
    ...
    
One leading space is all it takes to start a code block in the Wiki source. Further spaces at the start of a line will simply be considered part of the code line.

There are 4 separate translation variables for a code block. These are:

..  code-block:: none

    CONV_CODE_BLOCK_PREAMBLE
    CONV_CODE_BLOCK_POSTAMBLE
    
    CONV_CODE_LINE_ON
    CONV_CODE_LINE_OFF
    
The first two start and stop a block of code, while the latter two start and stop a single line within the code block. This is required for those output formats which require such things. In the example HTML translation, the latter two are not required while the former two are set to ``<pre>`` and ``</pre>`` respectively.

When the first line of a code block is detected in the Wiki source, The translation begins by writing out the ``CONV_CODE_BLOCK_PREAMBLE`` variable. It then processes each line and writes out something resembling  the following:

..  code-block:: none

    <CONV_CODE_LINE_ON> Line of code <CONV_CODE_LINE_OFF>

The next line will be read from the input file, and processed as above until a line is read that does not constitute a code line. On this detection, the ``CONV_CODE_BLOCK_POSTAMBLE`` variable is written out to the translated file.

In the example HTML translation, this code:

..  code-block:: none

    <space>1000 CLS <EOL>
    <space>1005 REPeat Madness<EOL>
    <space>1010 PRINT !'Hello World! '!<EOL>
    <space>1015 END REPeat Madness<EOL>

will be translated to the following HTML:

..  code-block:: html

    <pre>
    1000 CLS
    1005 REPeat Madness
    1010 PRINT !'Hello World! '!
    1015 END REPeat Madness
    </pre>

Individual lines of code in HTML do not need a start and stop tag.
    
Table Rows
~~~~~~~~~~

Table rows are found in the Wiki source, and resemble the following:

..  code-block:: none

    | Cell 1 | Cell 2 | Cell 3 <EOL>
    | Cell 4 | Cell 5 | Cell 6 <EOL>
    | Cell 7 | Cell 8 | Cell 9 <EOL>
    ...
    
One leading pipe character is all it takes to start a table row in the Wiki source. It is unknown how the Wiki handles two or more consecutive pipe characters in a page's source, but ``rwapWiki`` creates a blank cell as if that is what the author of the Wiki page intended.

There are 6 separate translation variables for table rows. These are:

..  code-block:: none

    CONV_TABLE_PREAMBLE
    CONV_TABLE_POSTAMBLE

    CONV_TABLE_ROW_PREAMBLE
    CONV_TABLE_ROW_POSTAMBLE

    CONV_TABLE_CELL_PREAMBLE
    CONV_TABLE_CELL_POSTAMBLE
    
The first two start and stop a full table in the output file. The middle two start and stop a single row in the table, while the latter two, define the start and end of a single cell within a table row.

When the first line of a table row is detected in the Wiki source, The translation begins by writing out the ``CONV_TABLE_PREAMBLE`` variable. It then processes each line, parses it into separate cells, processes each cell's text for any of the *Embedded Formatting* characters, and writes out something resembling  the following:

..  code-block:: none

    <CONV_TABLE_ROW_PREAMBLE> <CONV_TABLE_CELL_PREAMBLE> Cell 1 <CONV_TABLE_CELL_POSTAMBLE>  <CONV_TABLE_CELL_PREAMBLE> Cell 2 <CONV_TABLE_CELL_POSTAMBLE> ... <CONV_TABLE_ROW_POSTAMBLE>

The next line will be read from the input file, and processed as above until a line is read that does not constitute a table row. On this detection, the ``CONV_TABLE_POSTAMBLE`` variable is written out to the translated file.

In the HTML example translation, the following code:

..  code-block:: none

    | Cell 1 | Cell 2 | Cell 3 <EOL>
    | Cell 4 | Cell 5 | Cell 6 <EOL>
    | Cell 7 | Cell 8 | Cell 9 <EOL>

Will be translated to this HTML:

..  code-block:: html

    <table border="1">
    <tr><td>Cell 1</td><td>Cell 2</td><td>Cell 3</td></tr>
    <tr><td>Cell 4</td><td>Cell 5</td><td>Cell 6</td></tr>
    <tr><td>Cell 7</td><td>Cell 8</td><td>Cell 9</td></tr>
    </table>


Other Line Includes
~~~~~~~~~~~~~~~~~~~

The remainder of the potential *Line Includes* codes and processing are also valid for those lines which have been processed as *Line Starts*, so those are discussed below in the *Embedded Formatting* section.


Embedded Formatting
-------------------

Lines with special characters at the beginning, see *Line Starts* above, may also contain some form of embedded formatting too. The utility processes each line start line, after replacing whatever was necessary with conversion variables, to check for and process the following embedded formats:

-   Bold
-   Italic
-   Inline Code
-   Links
-   Forced Line Feed
-   Citations
-   References
-   Anchors
-   Acronyms
-   Images

Not all are valid for all types of line start lines, horizontal lines don't have any embedded formatting for example, but they are checked for anyway. Each type of embedded formatting is discussed below.

Bold
~~~~

Bold text is created as follows:

..  code-block:: none

    Normal text __ bold stuff __ normal text again.

This can occur anywhere in the line. In addition, bold text may start on one line of a paragraph, extend over one or more lines, and then end on a subsequent line later on in the paragraph. Bold text may also be embedded within some other formatting, which itself may span multiple lines, italic for example. There may also be multiple bold texts on a line. ``RwapWiki`` correctly handles these situations.    

Bold text is converted to the following:

..  code-block:: none

    ... <CONV_BOLD_ON> bold stuff <CONV_BOLD_OFF> ...
    
Italic
~~~~~~

Italic text is created as follows:

..  code-block:: none

    Normal text '' italic stuff '' normal text again.

This can occur anywhere in the line. In addition, italic text may start on one line of a paragraph, extend over one or more lines, and then end on a subsequent line later on in the paragraph. Italic text may also be embedded within some other formatting, which itself may span multiple lines, bold for example.  There may also be multiple italic texts on a line. ``RwapWiki`` correctly handles these situations.    

Italic text is converted to the following:

..  code-block:: none

    ... <CONV_ITALIC_ON> italic stuff <CONV_ITALIC_OFF> ...
    
Inline Code
~~~~~~~~~~~

Inline code text is created as follows:

..  code-block:: none

    Normal text @@ Some code stuff @@ normal text again.

This can occur anywhere in the line. In addition, code text may start on one line of a paragraph, extend over one or more lines, and then end on a subsequent line later on in the paragraph.  There may also be multiple embedded code sections on a single line. ``RwapWiki`` correctly handles these situations.    

Inline code text is converted to the following:

..  code-block:: none

    ... <CONV_INLINE_CODE_ON> Some code stuff <CONV_INLINE_CODE_OFF> ...
    
Links
~~~~~

There are three types of link in a Wiki page. These are:

-   Links to other Wiki Pages;
-   Links to the Internet;
-   Links to allow embedding of You Tube videos.

These are discussed below.

Be aware that there can be more than one link on a single line of text from the input file. However, links must be fully contained within the same line - they cannot wrap onto subsequent lines.

Wiki Page Links
"""""""""""""""

These are the simplest links to process. The format is:

..  code-block:: none

    [Page Name]
    
There is of course a problem here. Page titles which have any punctuation or spaces in might not be valid of some output formats, so the translation file allows two substitution texts to be defined, these are:

-   %PAGE_NAME% which is the page name exactly as defined in the Wiki source.
-   %COMPRESSED_NAME% which is the same as %PAGE_NAME% but with all punctuation and spaces replaced by hyphens. 

Either, or both, can be used in the translation variable used, which is ``CONV_WIKI_LINK``.

In the translation, all occurrences of the text '%PAGE_NAME%' will be replaced by the actual page name, punctuation and spaces included, as is, while all occurrences of '%COMPRESSED_NAME%' will be replaced by the slightly massaged page title, where hyphens abound.

The example HTML translation file for the following text:

..  code-block:: none

    [Dilwyn Jones]
    
Will output the following HTML:

..  code-block:: html
  
    <a href="Dilwyn-Jones.html">Dilwyn Jones</a>

Thus creating a link to the page with the title 'Dilwyn Jones'. The file that the 'Dilwyn Jones' page lives in is assumed to be 'Dilwyn-Jones.html' in *exactly* that letter case. However, some pages in the Wiki have interesting characters in their titles, so the generated filenames are a bit off, at least in Windows. 

Also problematical is the fact that some page links in the Wiki source assume case insensitivity, and as filenames on Linux are case sensitive, that can cause links not to work.

See the section on *Problem Areas Identified* for more details.

See also, the section on *WikiPager* for details of how that utility splits the database content into separate page files, each named after the page name in the database.

HTTP Links
""""""""""

An HTTP link, in the Wiki source, is defined as follows:

..  code-block:: none

    [link_text | url | language | title_text]
    
Of these, only the first two fields are mandatory, the rest are optional. The link_text is what will appear in the Wiki page and can be clicked on to open the link specified in the url field. Language is used by the Wiki itself and is not normally used on "normal" web pages and the title_text is what appears in a pop-up when you hover over the link_text.  

The translation variable used to convert the above into the output format is named ``CONV_URL_LINK`` and the following substitution variables are permitted:

-   %LINK_TEXT% which is the link_text on the generated page.
-   %URL% which is the url to be opened when the link_text is clicked.
-   %LANGUAGE% which is the language code, and unlikely to be useful.
-   %TITLE_TEXT% which is the pop-up text when hovered over.


The following link in the Wiki source:

..  code-block:: none

    [Quanta|http://www.quanta.org.uk|en|Quanta User Group]

will be output as follows by the example HTML translation file:

..  code-block:: html

    <a href="http://www.quanta.org.uk" title="Quanta User Group">Quanta</a>
    
While the following source text, which only has two of the four fields:

..  code-block:: none

    [QL Forum|http://www.qlforum.co.uk]
    
Will be output as:

..  code-block:: html

    <a href="http://www.qlforum.co.uk" title="%TITLE_TXT%">QL Forum</a>

Which is not exactly wonderful, but does work.  (Just don't hover over the link too long!)  

You Tube Video Links
""""""""""""""""""""

A You Tube video can be embedded in a Wiki page by using the following text in a page:

..  code-block:: none

    (vid) URL of Video (/vid)
    
The entire link must fit on one line, no continuation is permitted.

Translation of the above format into the desired output is done using the ``CONV_YOUTUBE_LINK`` variable, which permits two separate substitution variables:

-   %URL% which is the full URL to the full text of the video's URL as per the Wiki page. Basically, everything between ``(vid)`` and ``(/vid)``.
-   %VIDEO_ID% which is populated with everything in the URL that occurs *after* the text ``?v=`` in the URL, if it is found, otherwise it is blank.

So far, all the video links in the Wiki are of the format:

..  code-block:: none

    (vid)http://www.youtube.com/watch?v=AO5BUIKykMM(/vid)

And these are converted to the following, by the translation variable named ``CONV_YOUTUBE_LINK``:

..  code-block:: html

    <iframe width="30%" height="30%" src="https://youtube.com/embed/%VIDEO_ID%" frameborder="1" allowfullscreen></iframe>
    
You can see that the full URL text is not used in the above substitution, however, it is available for use if desired. The above code is based pretty much 100%, on what You Tube generates for you when you click on the "get embed code" for a particular video.

Forced Line Feed
~~~~~~~~~~~~~~~~

A forced line feed, in the Wiki, is created thus:

..  code-block:: none

    yada yada %%% text after the linefeed ...
    
and is converted to the following:


..  code-block:: none

    yada yada <CONV_FORCE_LINE_FEED_ON><CONV_FORCE_LINE_FEED_OFF> text after the linefeed ...
    
There may be more than one forced line in a single Wiki source line, so all of them are replaced. In the example HTML conversion file, ``CONV_FORCE_LINE_FEED_ON`` is set to ``<br>`` and ``CONV_FORCE_LINE_FEED_OFF`` is blank.

Citations
~~~~~~~~~

Citations are identified in the Wiki source as follows:

..  code-block:: none

    ^^citation|source link^^

or this:

..  code-block:: none

    ^^citation^^


``RwapWiki`` treats this as one or two separate parts, '%CITATION%' and, optionally, '%SOURCE%'. Hopefully, it is obvious from the above as to which is which. The utility allows each part to be substituted into the translated line, if required.

The utility processes these and converts them by reading the variables ``CONV_CITATION_LINK``  and ``CONV_CITATION_NOSOURCE_LINK`` and scanning both for the text '%CITATION%' and, optionally, '%SOURCE%', and for each occurrence found, replaces the substitution text with the appropriate part of the Wiki source line's citation text.  

The example HTML conversion uses the following as the ``CONV_CITATION_LINK`` and ``CONV_CITATION_NOSOURCE_LINK`` translation variables, for the two permitted translation options:

..  code-block:: none

    CONV_CITATION_LINK=<abbr title="%SOURCE%">%CITATION%</abbr>
    CONV_CITATION_NOSOURCE_LINK=<blockquote>%CITATION%</blockquote>
    
And so, decrees that a citation in the Wiki shall be created as an abbreviation in the HTML if it has a source, or it will be a block quote if not. This is easily changed of course. 

The reason for having two different citation translation variables is down to the fact that the Wiki code (written in PHP) allows citations to have either just a citation part, or to have both citation and source parts - although this is not documented. The QL Wiki uses both forms of citation.

Obviously, if a citation in the Wiki is found to have no source part, then the ``CONV_CITATION_NOSOURCE_LINK`` will be used in the output, otherwise the ``CONV_CITATION_LINK`` translation will be used. 

There can be more than one citation per line, but they must fully exist on a single line - continuations are not permitted.

References
~~~~~~~~~~

References are created in the Wiki as follows:

..  code-block:: none

    ... {{reference}} ...
    
There is, as can be seen, one part to a reference. ``RwapWiki`` processes this as '%REFERENCE%' and converts it by reading the variable ``CONV_REFERENCE_LINK`` and scanning it for the text '%REFERENCE%', and for each occurrence found, replaces the text '%REFERENCE%' with the appropriate part of the Wiki source's reference code. 

The example HTML conversion uses the following as the ``CONV_REFERENCE_LINK`` replacement text:

..  code-block:: none

    CONV_REFERENCE_LINK=<u>%REFERENCE%</u>
    
And so, decrees that a reference in the Wiki shall be created simply as underlined text in the generated HTML. This is easily changed of course.  

There can be more than one reference per line, but they must fully exist on a single line as continuations are not permitted.

Anchors
~~~~~~~

I could never seem to get an anchor to work in a Wiki sandbox page! Maybe they don't. They certainly didn't seem to do much. However ``rwapWiki`` will attempt to translate them.

Anchors are created in the Wiki as follows:

..  code-block:: none

    ... ~~anchor|title~~ ...
    
There are, as can be seen, two parts to an anchor, the anchor text and the title text. The conversion utility, ``rwapWiki`` processes these and converts them by reading the variable ``CONV_ANCHOR_LINK`` and scanning it for the text '%ANCHOR%' and '%TITLE_TEXT%', and for each occurrence, replaces the found text with the appropriate part of the Wiki source's anchor code. 

The example HTML conversion uses the following as the ``CONV_ANCHOR_LINK`` replacement text:

..  code-block:: none

    <abbr title="%TITLE_TEXT%">%ANCHOR%</abbr>
    
And so, decrees that an anchor in the Wiki shall be created as an abbreviation in the HTML. This is easily changed of course.  

There can be more than one anchor per line, but they must fully exist on a single line as continuations are not permitted.

If the replacement text does not have one, or both, of the replacement text "macros", then those parts of the anchor text from the input file will be missing from the output.    

Acronyms
~~~~~~~~

Acronyms are used in the Wiki to display some text, normally underlined, and when that text is hovered over with the cursor, some explanatory text is displayed in a pop-up window.

This translates nicely to the HTML``<abbr>`` (abbreviation) tag, which has exactly the same purpose.

Acronyms are created in the Wiki as follows:

..  code-block:: none

    ... ??acronym|Explanation Text?? ...
    
There are, as can be seen, two parts to a acronym. ``RwapWiki`` processes this as '%ACRONYM%' and '%TITLE_TEXT%' and converts them by reading the variable ``CONV_ACRONYM_LINK`` and scanning it for the text '%ACRONYM%' and '%TITLE_TEXT%', and for each occurrence found, replaces it with the appropriate part of the Wiki source's acronym code. 

The example HTML conversion uses the following as the ``CONV_REFERENCE_LINK`` replacement text:

..  code-block:: none

    CONV_ACRONYM_LINK=<abbr title="%TITLE_TEXT%">%ACRONYM%</abbr>
    
And so, decrees that an acronym in the Wiki shall be created simply as an HTML abbreviation.  

There can be more than one acronym per line, but acronyms must fully exist on a single line, continuations are not permitted.

Images
~~~~~~
An image, in the Wiki source, is defined as follows:

..  code-block:: none

    ((source | alt | align | long_description | width | height | ???))
    
Of these, only the first field is mandatory, the rest are optional. 

The source is the URL, somewhere on the internet, where the actual image file can be found. Images in the Wiki are all links to a URL and are not loaded from local (to the server) storage. Alt is the alt text for the image, align is a single character that defines left or right alignment, case is ignored and valid values are taken from 'lLgG' for left and 'rRdD' for right. The Wiki author is French and allows French 'Droit' and 'Gauche' as well as English.

    **NOTE**: It appears that some images in the Wiki are aligned with 'i' or 'I'. These are not documented.

The long_description is either some text the describes the image, or the URL of a text file, somewhere on the internet, where the descriptive text is to be found. Width and height define how the image will be displayed on the Wiki page, when viewed in a browser, and ??? is an undocumented field that appears to accept the text "POPUP" and opens the image, if clicked, in a separate window. This field is unused in ``rwapWiki``.

The translation variables used to convert the above into the output format are:


-   ``CONV_IMAGE_LINK``
-   ``CONV_IMAGE_LINK_LEFT``
-   ``CONV_IMAGE_LINK_RIGHT``
-   ``CONV_IMAGE_LINK_CENTRE``

The latter three are required for some Wiki systems that use different ways to align images left, right or centred. If your desired output already caters for alignment, HTML for example, then any of the latter three which are left blank, will ignore that variable, and use the first one, ``CONV_IMAGE_LINK`` instead.

The following substitution variables are permitted:

-   %SRC% which is the url where the actual image file is found.
-   %ALT_TEXT% which is the alt text for the image, if present.
-   %ALIGN% which is the single character alignment text, if present. 
-   %LONG_DESC% which is the URL or the actual text for the long_description of the image, if present.
-   %WIDTH% which is the requested width of the image, if present.
-   %HEIGHT% which is the requested height of the image, if present.
-   %ALIGN_EXPAND% which is the expansion of the %ALIGN% into "left" or "right" as appropriate.

The following link in the Wiki source:

..  code-block:: none

    ((http://www.rwapadventures.com/images/hardware/sinclair_ql.jpg|Sinclair QL Home Computer|align="L"|A Sinclair QL Home Computer|240|180|POPUP))

will be output as follows by the example HTML translation file:

..  code-block:: html

    <a href="http://www.rwapadventures.com/images/hardware/sinclair_ql.jpg" title="A Sinclair QL Home Computer"><img src="http://www.rwapadventures.com/images/hardware/sinclair_ql.jpg" alt="A Sinclair QL Home Computer" width="240" height="180" border="0" align="L"></a>

You can hopefully see that in the example translation file, the image in the Wiki is translated into an HTML ``<IMG>`` tag, embedded within an ``<A>`` tag, to give a clickable image in the generated HTML, which simply displays the image on a page of it's own if clicked.    

Lists
-----

There are three different types of lists available in the Wiki, however, only two of them actually work! ``RwapWiki`` on the other hand, will convert the three different types correctly, even if the Wiki itself cannot.

Because of the way that the Wiki does not have a "here is a list" indicator, List processing is carried out differently from everything else which assumes that a single line will be processed. List processing repeats the required processing for as long as it reads another line from the input file that starts with a list entry indicator character.

These characters are:

-   ``*`` or ``-`` for unordered lists;
-   ``#`` for orderd lists;
-   ``;`` for definition lists.

The definition list is the non-working one on the Wiki.

Ordered and unordered lists can be nested, and the nesting level is indicated by the number of *consecutive* list indicator characters found at the start of the line.

Lists, in many other formats *do* have a "here is a list" indicator, HTML for example, has ``<ul>`` for an unordered list. They also have an "end of list" indicator too, in HTML this is ``</ul>`` for the unordered list. Because of this, ``rwapWiki`` also has a list begin and list end variable in case the output format requires one.

Each *item* in a list also has a preamble and postamble, so that individual list items can be delimited. These can be different for each list type.

The list start and end variables for unordered, ordered and definition lists are:

..  code-block:: none

    CONV_LIST_ON
    CONV_LIST_OFF

    CONV_NUM_LIST_ON
    CONV_NUM_LIST_OFF

    CONV_DEFN_LIST_PREAMBLE
    CONV_DEFN_LIST_POSTAMBLE        

Yes, I know, it *would* have been a good idea to keep the naming convention the same for all three list types wouldn't it?        

List processing starts by writing out the list start variable, as above, depending on the list type. There are no substitution variables in these that will be processed.

Once the list processing code has read an input line that is not part of a list, it completes list processing by writing out the list end variable, as above, depending again on the list type.

The list processing for each list *item* is explained below. It should be noted that list items can contain embedded formatting, so these are checked for in each list item line, and processed accordingly.

Nesting of lists is correctly handled by the simple task of going recursive in the code and calling  the list processing code again, from within itself.       
        
Unordered List
~~~~~~~~~~~~~~

The format of the lines in the Wiki source, which make up an unordered list, are as follows:

..  code-block:: none

    * List Item Text <EOL>
    
or:

..  code-block:: none

    - List Item Text <EOL>

Each line representing a list item is converted to remove the list item start character(s) - depending on the nesting level, and writing out something like the following:

..  code-block:: none

    <CONV_LIST_PREAMBLE> List Item Text <CONV_LIST_POSTAMBLE>
    
The next line is read from the input file, and the processing starts again but without writing out a list start variable, unless the list item is found to be nested.
    

Ordered List
~~~~~~~~~~~~

The format of the lines in the Wiki source, which make up an ordered list, are as follows:

..  code-block:: none

    # List Item Text <EOL>
    
Each line representing an ordered list item is converted to remove the list item start character(s) - depending on the nesting level, and writing out something like the following:

..  code-block:: none

    <CONV_NUM_LIST_PREAMBLE> List Item Text <CONV_NUM_LIST_POSTAMBLE>
    
The next line is read from the input file, and the processing starts again but without writing out a list start variable, unless the list item is found to be nested.

Definition List
~~~~~~~~~~~~~~~

Definition lists come it two parts. There's a term and a definition. Again, each item in a definition list might have embedded formatting, so this is catered for.

Definition lists cannot be nested.

A definition list item is found in the Wiki source as follows:

..  code-block:: none

    ; Term Text : Definition Text <EOL>
    
The two parts are separated by a colon. The definition text extends to the end of the line.

Each line representing a definition list item is converted to remove the list item start character(s) - depending on the nesting level, and writing out something like the following:

..  code-block:: none

    <CONV_DEFN_LIST_TERM_ON> Term Text <CONV_DEFN_LIST_TERM_OFF><CONV_DEFN_LIST_DESC_ON> Definition Text <CONV_DEFN_LIST_DESC_OFF>
    
The next line is read from the input file, and the processing starts again but without writing out a list start variable.


Building a Translation File
===========================

Translation Variables
---------------------

In order to convert the Wiki source code from one format, WiClear, to the desired output, a translation file is used. These files contain a number of translation variables, each holding specific translation text to be used when writing the output file.

The following table lists all the currently used translation variables and gives a brief description of each.

+-----------------------------+-------------------------------------------------------+
| Variable Name               | Description                                           |
+=============================+=======================================================+
| CONV_ACRONYM_LINK           | Converts an acronym.                                  |
+-----------------------------+-------------------------------------------------------+
| CONV_ANCHOR_LINK            | Converts an anchor.                                   |
+-----------------------------+-------------------------------------------------------+
| CONV_BLOCK_QUOTE_LINE_OFF   | Starts a line of block quoted text.                   |
+-----------------------------+-------------------------------------------------------+
| CONV_BLOCK_QUOTE_LINE_ON    | Ends  a line of block quoted text.                    |
+-----------------------------+-------------------------------------------------------+
| CONV_BLOCK_QUOTE_POSTAMBLE  | Ends a block quoted text section/block.               |
+-----------------------------+-------------------------------------------------------+
| CONV_BLOCK_QUOTE_PREAMBLE   | Starts a block quoted text section/block.             |
+-----------------------------+-------------------------------------------------------+
| CONV_BOLD_OFF               | Turns bold off.                                       |
+-----------------------------+-------------------------------------------------------+
| CONV_BOLD_ON                | Turns bold on.                                        |
+-----------------------------+-------------------------------------------------------+
| CONV_CITATION_LINK          | Converts a citation with a source present.            |
+-----------------------------+-------------------------------------------------------+
| CONV_CITATION_NOSOURCE_LINK | Converts a citation with no source present.           |
+-----------------------------+-------------------------------------------------------+
| CONV_CODE_BLOCK_POSTAMBLE   | Ends a code block section/block.                      |
+-----------------------------+-------------------------------------------------------+
| CONV_CODE_BLOCK_PREAMBLE    | Starts a code block section/block.                    |
+-----------------------------+-------------------------------------------------------+
| CONV_CODE_LINE_OFF          | Ends a single line of a code block.                   |
+-----------------------------+-------------------------------------------------------+
| CONV_CODE_LINE_ON           | Starts a single line of a code block.                 |
+-----------------------------+-------------------------------------------------------+
| CONV_DEFN_LIST_DESC_OFF     | Ends the definition part of a definition list item.   |
+-----------------------------+-------------------------------------------------------+
| CONV_DEFN_LIST_DESC_ON      | Starts the definition part of a definition list item. |
+-----------------------------+-------------------------------------------------------+
| CONV_DEFN_LIST_POSTAMBLE    | Ends a definition list.                               |
+-----------------------------+-------------------------------------------------------+
| CONV_DEFN_LIST_PREAMBLE     | Starts a definition list.                             |
+-----------------------------+-------------------------------------------------------+
| CONV_DEFN_LIST_TERM_OFF     | Ends the term part of a definition list item.         |
+-----------------------------+-------------------------------------------------------+
| CONV_DEFN_LIST_TERM_ON      | Starts the term part of a definition list item.       |
+-----------------------------+-------------------------------------------------------+
| CONV_FORCE_LINE_FEED_OFF    | Ends a forced line feed.                              |
+-----------------------------+-------------------------------------------------------+
| CONV_FORCE_LINE_FEED_ON     | Starts a forced line feed.                            |
+-----------------------------+-------------------------------------------------------+
| CONV_H1_POSTAMBLE           | Ends a level 1 header.                                |
+-----------------------------+-------------------------------------------------------+
| CONV_H1_PREAMBLE            | Starts a level 1 header.                              |
+-----------------------------+-------------------------------------------------------+
| CONV_H2_POSTAMBLE           | Ends a level 2 header.                                |
+-----------------------------+-------------------------------------------------------+
| CONV_H2_PREAMBLE            | Starts a level 2 header.                              |
+-----------------------------+-------------------------------------------------------+
| CONV_H3_POSTAMBLE           | Ends a level 3 header.                                |
+-----------------------------+-------------------------------------------------------+
| CONV_H3_PREAMBLE            | Starts a level 3 header.                              |
+-----------------------------+-------------------------------------------------------+
| CONV_HR_OFF                 | Ends a horizontal rule.                               |
+-----------------------------+-------------------------------------------------------+
| CONV_HR_ON                  | Starts a horizontal rule.                             |
+-----------------------------+-------------------------------------------------------+
| CONV_IMAGE_LINK             | Converts an image link.                               |
+-----------------------------+-------------------------------------------------------+
| CONV_INLINE_CODE_OFF        | Ends an inline code section.                          |
+-----------------------------+-------------------------------------------------------+
| CONV_INLINE_CODE_ON         | Starts an inline code section.                        |
+-----------------------------+-------------------------------------------------------+
| CONV_ITALIC_OFF             | Turns italic off.                                     |
+-----------------------------+-------------------------------------------------------+
| CONV_ITALIC_ON              | Turns italic on.                                      |
+-----------------------------+-------------------------------------------------------+
| CONV_LIST_OFF               | Ends an unordered list item.                          |
+-----------------------------+-------------------------------------------------------+
| CONV_LIST_ON                | Starts an unordered list item.                        |
+-----------------------------+-------------------------------------------------------+
| CONV_LIST_POSTAMBLE         | Ends an unordered list.                               |
+-----------------------------+-------------------------------------------------------+
| CONV_LIST_PREAMBLE          | Starts an unordered list.                             |
+-----------------------------+-------------------------------------------------------+
| CONV_NUM_LIST_OFF           | Ends an ordered list item.                            |
+-----------------------------+-------------------------------------------------------+
| CONV_NUM_LIST_ON            | Starts an ordered list item.                          |
+-----------------------------+-------------------------------------------------------+
| CONV_NUM_LIST_POSTAMBLE     | Ends an ordered list.                                 |
+-----------------------------+-------------------------------------------------------+
| CONV_NUM_LIST_PREAMBLE      | Starts an ordered list.                               |
+-----------------------------+-------------------------------------------------------+
| CONV_PARAGRAPH_POSTAMBLE    | Ends a single paragraph.                              |
+-----------------------------+-------------------------------------------------------+
| CONV_PARAGRAPH_PREAMBLE     | Starts a single paragraph.                            |
+-----------------------------+-------------------------------------------------------+
| CONV_POSTAMBLE              | Ends the entire file.                                 |
+-----------------------------+-------------------------------------------------------+
| CONV_PREAMBLE               | Starts the entire file.                               |
+-----------------------------+-------------------------------------------------------+
| CONV_REFERENCE_LINK         | Converts a reference.                                 |
+-----------------------------+-------------------------------------------------------+
| CONV_TABLE_CELL_POSTAMBLE   | Ends a single table cell.                             |
+-----------------------------+-------------------------------------------------------+
| CONV_TABLE_CELL_PREAMBLE    | Starts a single table cell.                           |
+-----------------------------+-------------------------------------------------------+
| CONV_TABLE_POSTAMBLE        | Ends an entire table.                                 |
+-----------------------------+-------------------------------------------------------+
| CONV_TABLE_PREAMBLE         | Starts an entire table.                               |
+-----------------------------+-------------------------------------------------------+
| CONV_TABLE_ROW_POSTAMBLE    | Ends a single table row.                              |
+-----------------------------+-------------------------------------------------------+
| CONV_TABLE_ROW_PREAMBLE     | Starts a single table row.                            |
+-----------------------------+-------------------------------------------------------+
| CONV_URL_LINK               | Converts an http link.                                |
+-----------------------------+-------------------------------------------------------+
| CONV_WIKI_LINK              | Converts a wiki page link.                            |
+-----------------------------+-------------------------------------------------------+
| CONV_YOUTUBE_LINK           | Converts a link to a You Tube video.                  |
+-----------------------------+-------------------------------------------------------+

In order to create (or edit) a translation file, you must define all of the above. To make life easier for you, there is a blank template file which has all of these present, and has brief explanations of each one.

The substitution text must follow an equal sign, and must (currently) be all on a single line, as per the following examples:

..  code-block:: none

    CONV_PREAMBLE=<html><head><title>Sinclair QL Wiki</title><meta charset="UTF-8"></head><body><h1>%TITLE%</h1>
    CONV_POSTAMBLE=</body></html>
    
These examples are used at the very beginning of a translation, and at the very end. They are taken from the HTML translation file which needs all of these fields to be present in a page. You can see that the preamble uses a substitution variable named ``%TITLE%`` which gets created by ``rwapWiki`` from the input file name by replacing all hyphens in the file name with spaces - which is the opposite (almost) of how the file was created from the original page title anyway. (It works - *most* of the time!)

Substitution Variables
----------------------

Some, but not all, translation variables allow certain parts of the Wiki source text to be extracted and used in the translation text, perhaps in a different place or order. The following table lists all current substitution variables and shows the translation variables that are permitted to use them.

In the following table, please be aware that the substitution variables are surrounded by percent signs (%) when used in the translation files. The percent signs are not shown below.

+-------------------+---------------------------------------------------------------------+
| Variable Name     | Description                                                         |
+===================+=====================================================================+
| ACRONYM           | The acronym part of a Wiki acronym. Used in CONV_ACRONYM_LINK.      |
+-------------------+---------------------------------------------------------------------+
| ALIGN             | Alignment code for an image, one letter from 'lLgG' or 'rRdD'.      |
|                   | Used in CONV_IMAGE_LINK.                                            |
+-------------------+---------------------------------------------------------------------+
| ALIGN_EXPAND      | Expanded alignment for an image, left, right.                       |
|                   | Used in CONV_IMAGE_LINK.                                            |
+-------------------+---------------------------------------------------------------------+
| ALT_TEXT          | ALT text for an image. Used in CONV_IMAGE_LINK.                     |
+-------------------+---------------------------------------------------------------------+
| ANCHOR            | The anchor text for a Wiki anchor. Used in CONV_ANCHOR_LINK.        |
+-------------------+---------------------------------------------------------------------+
| CITATION          | The citation text in a citation with source text. Used in           |
|                   | CONV_CITATION_LINK and CONV_CITATION_NOSOURCE_LINK.                 |
+-------------------+---------------------------------------------------------------------+
| COMPRESSED_NAME   | The Wiki page name with spaces and punctuation replaced by hyphens. |
|                   | Used in CONV_WIKI_LINK.                                             |
+-------------------+---------------------------------------------------------------------+
| HEIGHT            | Height of an image. Used in CONV_IMAGE_LINK.                        |
+-------------------+---------------------------------------------------------------------+
| LANGUAGE          | The language code, two letters, for a URL. Not likely to be used.   |
|                   | Used in CONV_URL_LINK.                                              |
+-------------------+---------------------------------------------------------------------+
| LINK_TEXT         | The text to be displayed as a clickable link in a URL. Used in      |
|                   | CONV_URL_LINK.                                                      |
+-------------------+---------------------------------------------------------------------+
| LONG_DESC         | Popup text when image hovered over. Long description of an image.   |
|                   | Used in CONV_IMAGE_LINK.                                            |
+-------------------+---------------------------------------------------------------------+
| PAGE_NAME         | The Wiki page name. Used in CONV_WIKI_LINK.                         |
+-------------------+---------------------------------------------------------------------+
| REFERENCE         | The text of a Wiki reference. Used in CONV_REFERENCE_LINK.          |
+-------------------+---------------------------------------------------------------------+
| SOURCE            | The source of a citation. May not always be present. Used in        |
|                   | CONV_CITATION_LINK.                                                 |
+-------------------+---------------------------------------------------------------------+
| SRC               | Source URL of an image. Used in CONV_IMAGE_LINK.                    |
+-------------------+---------------------------------------------------------------------+
| TITLE             | The title of a Wiki Page file. Taken from the input filename. Used  |
|                   | in CONV_PREAMBLE.                                                   |
+-------------------+---------------------------------------------------------------------+
| TITLE_TEXT        | The explanation text of a Wiki acronym. Used in CONV_ACRONYM_LINK.  |
+-------------------+---------------------------------------------------------------------+
| TITLE_TEXT        | The title text for a Wiki anchor. Used in CONV_ANCHOR_LINK.         |
+-------------------+---------------------------------------------------------------------+
| TITLE_TEXT        | The popup text for a hovered URL link. Used in CONV_URL_LINK.       |
+-------------------+---------------------------------------------------------------------+
| URL               | The web address to be linked to in a URL.  Used in CONV_URL_LINK.   |
+-------------------+---------------------------------------------------------------------+
| URL               | The URL for a You Tube video.  Used in CONV_YOUTUBE_LINK.           |
+-------------------+---------------------------------------------------------------------+
| VIDEO_ID          | The video id extracted from a You Tube URL after ``?v=``. Used in   |
|                   | CONV_YOUTUBE_LINK.                                                  |
+-------------------+---------------------------------------------------------------------+
| WIDTH             | Width of an image. Used in CONV_IMAGE_LINK.                         |
+-------------------+---------------------------------------------------------------------+


Wiki Source Formatting
----------------------

So that you can relate the above variables to what the code in the Wiki source requires, the following table should make things clear. It shows the actual formatting required by the Wiki source code in order to generate a particular feature of the rendered Wiki pages when viewed in your browser. Where permitted, the substitution variables in the table above, will be shown.

In the following:

-   The text ``'(*)'`` indicates an optional repetition of the previous character. ``'===(*)='`` means a minimum of 3 equal signs, then zero or more equal signs, followed by one equal sign, giving, a minimum of 4 and a maximum of however many you like. 
-   <EOL> indicates the end of the line.
-   Anything that does not have <EOL> implies that the feature can wrap onto subsequent lines. Bold text, for example.
-   Forced linefeeds obviously cannot wrap!
-   Image Galleries and redirections are not able to be converted, at present, but do not exist in the Wiki anyway.
-   Table rows can have as many cells as required, the final cell is terminated by <EOL> and not by a '|' character.
-   Where multiple fields exists for a feature, and some fields are optional, those are marked as :sup:`OPT`. Where optional fields exists, the separator character preceding that field must not be present if the field is missing. All following fields are missing too, where more than one exist. (there is no option for positional fields, only in-line.)
-   Where multiple fields are available, but some are unused by the translation utility, these are marked as :sup:`unused`.
-   Wiki Features that are not supported by the utility are marked as :sup:`NS`\ .
-   Images also have an additional variable named ``%ALIGN_EXPAND%`` which is not shown below.
-   Wiki Pages also have an additional variable named ``%COMPRESSED_NAME%`` which is not shown below.

..  COMMENT TO SELF. In the following table, I'm able to use pipe characters (|) even though they are required by the table formatting text. This can be done by escaping only the FIRST one on a line of table text, which seems to work, but it also works if I escape all of the ones that should not be deemed to be cell separators. Just to be explicit though, I escaped them all! If you miss one, it throws the table formatting right out the window in PDF output from Pandoc.
    

+--------------------------+-------------------------------------------------------------+
| Wiki Feature             | Usage in Wiki Source                                        |
+==========================+=============================================================+
| Acronym                  | ?? %ACRONYM% \| %TITLE_TEXT% ??                             |
+--------------------------+-------------------------------------------------------------+
| Anchor                   | --- %ANCHOR% \| %TITLE_TEXT% --- (Two hyphens)              |
+--------------------------+-------------------------------------------------------------+
| Block Quote              | > Text line <EOL>                                           |
+--------------------------+-------------------------------------------------------------+
| Bold Text                | __ Text to embolden __                                      |
+--------------------------+-------------------------------------------------------------+
| Citation                 | ^^ %CITATION% \| %SOURCE%\ :sup:`OPT` ^^                    |
+--------------------------+-------------------------------------------------------------+
| Code Block               | <SPACE> One code line <EOL>                                 |
+--------------------------+-------------------------------------------------------------+
| Definition List          | ; term : definition <EOL>                                   |
+--------------------------+-------------------------------------------------------------+
| Forced Linefeed          | Text before linefeed %%% Text after                         |
+--------------------------+-------------------------------------------------------------+
| HTTP Link                | [%LINK_TEXT% \| %URL% \| %LANGUAGE%\ :sup:`OPT`             |
|                          | \| %TITLE_TEXT%\ :sup:`OPT`]                                |
+--------------------------+-------------------------------------------------------------+
| Heading 1                | !!! Heading text <EOL>                                      |
+--------------------------+-------------------------------------------------------------+
| Heading 2                | !! Heading text <EOL>                                       |
+--------------------------+-------------------------------------------------------------+
| Heading 3                | ! Heading text <EOL>                                        |
+--------------------------+-------------------------------------------------------------+
| Horizontal Rule          | ====(*)= <EOL>                                              |
+--------------------------+-------------------------------------------------------------+
| Image                    | ((%SRC% \| %ALT_TEXT%\ :sup:`OPT` \| %ALIGN%\ :sup:`OPT` \| |
|                          | %LONG_DESC%\ :sup:`OPT` \| %WIDTH%\ :sup:`OPT` \|           |
|                          | %HEIGHT%\ :sup:`OPT` \| popup\ :sup:`unused`))              |
+--------------------------+-------------------------------------------------------------+
| Image Gallery\ :sup:`NS` | %% folder \| title \| width \| height %%                    |
+--------------------------+-------------------------------------------------------------+
| Inline Code              | @@ code text @@                                             |
+--------------------------+-------------------------------------------------------------+
| Italic Text              | __ Text to italicise __                                     |
+--------------------------+-------------------------------------------------------------+
| Ordered List             | #(*) List Item <EOL>                                        |
+--------------------------+-------------------------------------------------------------+
| Paragraph                | Lines of text, separated by a double <EOL>.                 |
+--------------------------+-------------------------------------------------------------+
| Redirection\ :sup:`NS`   | & url &                                                     |
+--------------------------+-------------------------------------------------------------+
| Reference                | {{ %REFERENCE% }}                                           |
+--------------------------+-------------------------------------------------------------+
| Table Row                | \| Cell Text \| Cell Text \| ... \| Cell Text <EOL>         |
+--------------------------+-------------------------------------------------------------+
| Unordered List           | \*(*) List Item <EOL>                                       |
+--------------------------+-------------------------------------------------------------+
| Unordered List           | -(*) List Item <EOL>                                        |
+--------------------------+-------------------------------------------------------------+
| Wiki Page Link           | [%PAGE_NAME%]                                               |
+--------------------------+-------------------------------------------------------------+
| You Tube Video           | (vid) %URL% (/vid)                                          |
+--------------------------+-------------------------------------------------------------+

A Worked Example - DokuWiki
---------------------------

`DokuWiki <https://www.dokuwiki.org/>`_ is a well known, and supported, Wiki system that uses plain text files as it's "database". These files contain different features to those in WiClear, but may of them are similar. Let's create a file named convert.DOKUWIKI.txt to translate our WiClear page files to (hopefully) DokuWiki format.

DokuWiki's syntax is documented, on a DokuWiki wiki of course, `here <https://www.dokuwiki.org/wiki:syntax>`_.

Page Preamble & Postamble
~~~~~~~~~~~~~~~~~~~~~~~~~

DokuWiki pages do not normally need any preamble or postamble, so these can be left blank, as follows:

..  code-block:: none

    CONV_PREAMBLE=
    CONV_POSTAMBLE=
    
However, any page with more than three headings in it, will generate a small table of contents at the top of the page. If you do not want these TOCs to be created, there is a special code to disable them, and this can be set in ``CONV_PREAMBLE`` as follows:

..  code-block:: none

    CONV_PREAMBLE=~~NOTOC~~ 
    
I have noticed that *most* of the pages in the QL Wiki do not have a heading that explains the page name. The WiClear software presumably took care of this. DokuWiki won't display any headings if there are none in the original QL Wiki, so, in order to see something, we can use the ``%TITLE%%`` substitution variable for each page. To do this, configure as follows:

..  code-block:: none

    CONV_PREAMBLE= ====== %TITLE% ======

This will give each page a level 1 heading with the original QL Wiki page name.

Acronym
~~~~~~~
DokuWiki Acronyms are interesting beasts as they are not embedded in the text of a Wiki page as WiClear's are, they are instead held in a configuration file located in your web server's document root directory, under ``conf/acronyms.conf``. The acronym itself is still required in the Wiki page, but the description is not, so the following configuration:

..  code-block:: none

    CONV_ACRONYM_LINK=%ACRONYM%
    
will suffice to make sure that the acronym is still present in the output file. You will, however, need to make sure that your acronym and it's description are added to the configuration file for DokuWiki to make the acronym's work.  

Anchor
~~~~~~
In WiClear, an anchor is made up of two parts, the anchor and the title part. In WiClear, all I can see on a page is that the anchor part is written with a red font, and there's no sign of the title part - very weird!

This behaviour can be reproduced in DokuWiki with a little fiddling about using direct HTML, which gets embedded in the generated page when viewed in the browser, so:

..  code-block:: none

    CONV_ANCHOR_LINK=<html><span style="color:red;">%ANCHOR%</span></html>

Please note that DokuWiki does not allow embedded HTML by default as it can pose a security risk by allowing cross site scripting attacks, however, if your Wiki is private, then you are safe to use the *admin* section of DokuWiki to enable Direct HTML. If direct HTML is not enabled, the HTML code will simply be listed in your wiki page as plain text.

Block Quote
~~~~~~~~~~~
Block quote text is the same in DokuWiki as it is in WiClear. The following will allow translation to take place:

..  code-block:: none

    CONV_BLOCK_QUOTE_PREAMBLE=
    CONV_BLOCK_QUOTE_POSTAMBLE=
    CONV_BLOCK_QUOTE_LINE_ON=>
    CONV_BLOCK_QUOTE_LINE_OFF=
    
You should just about be able to see a '>' at the end of the third line.    

Bold Text
~~~~~~~~~
Italic text simply wraps the text to be emboldened in a pair of asterisks. We can define our bold text settings as follows:

..  code-block:: none

    CONV_BOLD_ON=**
    CONV_BOLD_OFF=**

Citation
~~~~~~~~
Citations in WiClear are interesting - for certain values of interesting! They have two parts, a citation and a source and the generated page appears only to wrap the citation in double quotes, while apparently doing nothing with  the source part - very strange. This behaviour can be replicated in a DokuWiki page as follows:

..  code-block:: none

    CONV_CITATION_LINK="%CITATION%"
    CONV_CITATION_NOSOURCE_LINK="%CITATION%"

Code Block
~~~~~~~~~~
For best results, use the DokuWiki ``<code>`` and ``</code>`` tags to delimit code blocks. You only need one of each, the code lines themselves do not need any special formatting. This gives us the following in our translation file:

..  code-block:: none

    CONV_CODE_BLOCK_PREAMBLE=<code>
    CONV_CODE_BLOCK_POSTAMBLE=</code>
    CONV_CODE_LINE_ON=
    CONV_CODE_LINE_OFF=

You can also use a pair of spaces on each code line to format them as a code block, so in that case, your translation file would need the following, where the spaces are replaced by underscores to make them visible:

..  code-block:: none

    CONV_CODE_BLOCK_PREAMBLE=
    CONV_CODE_BLOCK_POSTAMBLE=
    CONV_CODE_LINE_ON=__
    CONV_CODE_LINE_OFF=

The underscores, representing spaces, are on the third line only, the rest are blank.

Having used DokuWiki for many years, I can thoroughly recommend the use of ``CONV_CODE_BLOCK_PREAMBLE`` and ``CONV_CODE_BLOCK_POSTAMBLE`` instead of doing things with leading spaces.

DokuWiki can also highlight source code when you use the ``<code>`` form of rendering code lines. There are a lot of supported highlighting styles as listed on the `syntax page <https://www.dokuwiki.org/wiki:syntax#syntax_highlighting>`_. You might wish to edit t he generated file(s) to add highlighting, or if you know that all code examples are the same, just set something like this:

..  code-block:: none

    CONV_CODE_BLOCK_PREAMBLE=<code cobol>
    CONV_CODE_BLOCK_POSTAMBLE=</code>
    CONV_CODE_LINE_ON=
    CONV_CODE_LINE_OFF=

Yes, I know, COBOL! Well, it's what I started my IT professional life doing and it's still in use - just not much on the QL Wiki! There are 257 different languages that can be styled at the time of writing, and there is even one for 'zxbasic'.

Definition List
~~~~~~~~~~~~~~~
Definition Lists are not supported by DokuWiki in the default install, however, there is a plugin named `definitions <https://www.dokuwiki.org/plugin:definitionlist>`_ which can be downloaded and installed. 

If you have this plugin, then the syntax is almost exactly the same as WiClear's, so the following will suffice:

..  code-block:: none

    CONV_DEFN_LIST_PREAMBLE=
    CONV_DEFN_LIST_POSTAMBLE=
    CONV_DEFN_LIST_TERM_ON=__;
    CONV_DEFN_LIST_TERM_OFF=:
    CONV_DEFN_LIST_DESC_ON=
    CONV_DEFN_LIST_DESC_OFF=

Spaces are displayed above as underscores, to make them visible. The third line turns on a definition by introducing the term part with two spaces and a semi-colon. It is turned off in the fourth line by a colon. The definition part has no settings as it is simply the text from the colon to the end of the line, similar to WiClear.

As with the You Tube 'vshare' plugin - see below - I do not have the 'definitions' plugin installed, so I'm unable to test it.

Forced Linefeed
~~~~~~~~~~~~~~~

DokuWiki uses a double backslash to force a linefeed, but this has a foible in that it must be either:

-   At the end of a line; or
-   Followed by at least one white space character. (space, tab etc).


..  code-block:: none

    CONV_FORCE_LINE_FEED_ON=\\
    CONV_FORCE_LINE_FEED_OFF=_

There's a single space at the end of the ``CONV_FORCE_LINE_FEED_OFF``, however, I'm showing it above as an underscore to make it visible.  That should make sure that all our forced linefeeds do, at least, have a following space.
    
HTTP Link
~~~~~~~~~
External links, which are links to other web pages, are similar in DokuWiki to the WiClear syntax. The only difference is that DokuWiki uses '[[' and ']]' rather than '[' and ']'. In addition, only the URL and the link text are permitted in DokuWiki. The following will set up external links correctly:

..  code-block:: none

    CONV_URL_LINK=[[%URL%"|%LINK_TEXT%]]

Heading 1
~~~~~~~~~
Level 1 headings appear on a line surrounded by 6 equals signs, so we need to set up the following:

..  code-block:: none

    CONV_H1_PREAMBLE=======
    CONV_H1_POSTAMBLE=======
    
Yes, it looks like there are 7 equals signs, but the first will be used as the separator. You can add spaces if you wish, as follows:

..  code-block:: none

    CONV_H1_PREAMBLE= ======
    CONV_H1_POSTAMBLE= ======
    
Heading 2
~~~~~~~~~
Level 2 headings appear on a line surrounded by 5 equals signs, so we need to set up the following:

..  code-block:: none

    CONV_H2_PREAMBLE======
    CONV_H2_POSTAMBLE======
    
Yes, it looks like there are 6 equals signs, but the first will be used as the separator.  You can add spaces if you wish, as follows:

..  code-block:: none

    CONV_H2_PREAMBLE= =====
    CONV_H2_POSTAMBLE= =====

Heading 3
~~~~~~~~~
Level 1 headings appear on a line surrounded by 4 equals signs, so we need to set up the following:

..  code-block:: none

    CONV_H3_PREAMBLE======
    CONV_H3_POSTAMBLE======
    
Yes, it looks like there are 5 equals signs, but the first will be used as the separator.  You can add spaces if you wish, as follows:

..  code-block:: none

    CONV_H3_PREAMBLE= ====
    CONV_H3_POSTAMBLE= ====


Horizontal Rule
~~~~~~~~~~~~~~~
DokuWiki uses 4 (or more) hyphens at the start of a line to create a horizontal rule. This means that we simply do the following to get a valid translation:

..  code-block:: none

    CONV_HR_ON=----
    CONV_HR_OFF=

As with headings, a leading space is permitted, so the above could be specified as follows, to make reading easier on the eye:

..  code-block:: none

    CONV_HR_ON= ----
    CONV_HR_OFF=
    
Image
~~~~~
Images in DokuWiki can be embedded as actual images - but these need to be uploaded first, or as links in a manner similar to the WiClear wiki. Sadly, however, alignment of images is a little difficult in DokuWiki as it uses spaces to align left, right or centre (which WiClear doesn't appear to do). The following methods are available for embedding an image:

..  code-block:: none

    {{URL}}
    {{URL|Tooltip Text}}
    {{URL?width x height|Tooltip Text}}
    
There are others, you can read about them on the `DokuWiki syntax page <https://www.dokuwiki.org/wiki:syntax>`_ if you need additional features.

The first option simply embeds the image and if hovered over, will display the URL of the image. The second does the same, but when hovered over, displays a descriptive text in a pop-up tooltip window. The final option does the same, but resizes the image to the supplied width and height. Note, there should be no spaces between the width, the 'x' and the height.

To set those up in our translation file, we would use the following three options, to correspond to the above:

..  code-block:: none

    CONV_IMAGE_LINK={{%SRC%}}
    CONV_IMAGE_LINK={{%SRC%|%LONG_DESC%}}
    CONV_IMAGE_LINK={{%SRC%?%WIDTH%x%HEIGHT%|%LONG_DESC%}}
    
As mentioned, image alignment is done using white space before and/or after the '{{' or '}}' characters. As with table cells (See below), the image is aligned by putting the spaces on the side the image should be 'padded' on. So add spaces on the left to align right, add  spaces on the right to align left, and add them on both sides to align centrally.

..  code-block:: none

    CONV_IMAGE_LINK={{%SRC%?%WIDTH%x%HEIGHT%|Left Aligned Image  }}
    CONV_IMAGE_LINK={{  %SRC%?%WIDTH%x%HEIGHT%|Right Aligned Image}}
    CONV_IMAGE_LINK={{  %SRC%?%WIDTH%x%HEIGHT%|Centred Image  }}

However, from testing, it appears that a centrally aligned image, which has a tooltip present, will actually align on the right. Scaling and sizing still work though.Perchance, a bug! The following works to centre an image, but loses the tooltip text:

..  code-block:: none

    CONV_IMAGE_LINK={{  %SRC%?%WIDTH%x%HEIGHT% }}

If, as in the HTML example translation file, you want to embed an image as a link to a page displaying the image at its full size, then 

..  code-block:: none

    CONV_IMAGE_LINK={{  %SRC%?%WIDTH%x%HEIGHT%&direct }}

Image Gallery
~~~~~~~~~~~~~
Image galleries are permitted in DokuWiki, but as ``rwapWiki`` currently doesn't support them, and they are not used in the QL Wiki, this is not a valid option for a translation file.

Inline Code
~~~~~~~~~~~
DokuWiki has supposedly got the '%%' markers for inline code, which is supposed to format in a mono-spaced font and ignore any further formatting on the enclosed text, but doesn't! 

It also has a pair of single quotes (''), to format the text following in a mono-spaced font, which *does* work, but will still interpret italic, bold etc in whatever is between the two pairs of quotes. 

In order to get our inline code mono-spaced and not to be interpreted as Wiki markup, if the code resembles that, we have to use both of the above, as follows:

..  code-block:: none

    CONV_INLINE_CODE_ON=''%%
    CONV_INLINE_CODE_OFF=%%''

Italic Text
~~~~~~~~~~~
Italic text simply wraps the text to be italicised in double slashes. We can define our italic text settings as follows:

..  code-block:: none

    CONV_ITALIC_ON=//
    CONV_ITALIC_OFF=//

Ordered List
~~~~~~~~~~~~
An ordered list, in DokuWiki, is defined by a two spaces and a hyphen (-) rather than a hash (#) as in WiClear. So the following will permit unordered lists to be translated:

..  code-block:: none

    CONV_NUM_LIST_PREAMBLE=
    CONV_NUM_LIST_POSTAMBLE=
    CONV_NUM_LIST_ON=__-
    CONV_NUM_LIST_OFF=
    
You should be able to see the spaces - which I've shown as underscores - and the hyphen on the third line above. None of the others have anything after the equal sign as we do not need them.    

And here we find a problem, WiClear allows indented lists, and so does DokuWiki, but while WiClear uses a number of hash characters to indicate the indent level, DokuWiki needs two spaces for each additional indent. Hmmm, I feel a program amendment coming on!

Paragraph
~~~~~~~~~
Paragraphs don't need any special handling in DokuWiki, they are lines of text, separated by linefeeds, as in WiClear. The following code will work fine:

..  code-block:: none

    CONV_PARAGRAPH_PREAMBLE=
    CONV_PARAGRAPH_POSTAMBLE=

Redirection
~~~~~~~~~~~
Redirection does not appear possible in DokuWiki. This is not a problem, really, as ``rwapWiki`` also doesn't support them!


Reference
~~~~~~~~~
DokuWiki doesn't support references, but WiClear does. You could, I suppose, set the translation file to use underlining (or strike-through text) if you need to cater for these.

To use underlined text, you would configure ``CONV_REFERENCE_LINK`` as follows:

..  code-block:: none

    CONV_REFERENCE_LINK=__%REFERENCE%__
    
or, to use strike-through text instead:

..  code-block:: none

    CONV_REFERENCE_LINK=<del>%REFERENCE%</del>
    
Or, perhaps, references could have a multiple set of formatting styles applied, say bold, italic and underlined:

..  code-block:: none

    CONV_REFERENCE_LINK=**__//%REFERENCE%//__**
 
If you want a reference in DokuWiki to resemble one in WiClear, then the matter is quite simple, you wrap the reference text in italics, as follows:

..  code-block:: none

    CONV_REFERENCE_LINK=//%REFERENCE%//
 

Table Row
~~~~~~~~~
DokuWiki uses a similar syntax for tables as WiClear, however, while WiClear terminates the final cell on a row with an <EOL>, DokuWiki requires a closing pipe character. In addition, DockuWiki does not require any table or row indicators, so the syntax to convert a WiClear table to a DokuWiki one is as follows:

..  code-block:: none

    CONV_TABLE_PREAMBLE=
    CONV_TABLE_POSTAMBLE=
    CONV_TABLE_ROW_PREAMBLE=
    CONV_TABLE_ROW_POSTAMBLE=
    CONV_TABLE_CELL_PREAMBLE=|
    CONV_TABLE_CELL_POSTAMBLE=|
    
There's a pipe character (|) at the end of the two final lines above - only the cells are indicated and DokuWiki works out the rest.

WiClear doesn't have any ability to indicate which cells span rows or columns, nor can it show table headings in bold text, for example. DokuWiki can do all of these and also, it allows the cell contents to be aligned left, right or centred. See the `DokuWiki syntax page <https://www.dokuwiki.org/wiki:syntax>`_ for details. 

For cell alignment, you use two or more of additional spaces on the side you want to "pad" the text. Left aligned text is padded on the right. In the following examples, I'm using underscores (_) to make them visible, they should be spaces in the actual translation file.

-   To left align (aka padded right) you would use:

    ..  code-block:: none

        # No spaces before the cell text.
        CONV_TABLE_CELL_PREAMBLE=|
        # Two spaces after the cell text.
        CONV_TABLE_CELL_POSTAMBLE=|__

-   To right align (aka padded left) you would use:

    ..  code-block:: none

        # Two spaces before the cell text.
        CONV_TABLE_CELL_PREAMBLE=|__
        # No spaces after the cell text.
        CONV_TABLE_CELL_POSTAMBLE=|

-   To centre align (aka padded left and right) you would use:

    ..  code-block:: none

        # Two spaces before the cell text.
        CONV_TABLE_CELL_PREAMBLE=|__
        # Two spaces after the cell text.
        CONV_TABLE_CELL_POSTAMBLE=__|  

Unordered List
~~~~~~~~~~~~~~
An unordered list, in DokuWiki, is defined by two spaces and an asterisk as in WiClear, but the WiClear hyphen (-) is not permitted here. So the following will permit unordered lists to be translated:

..  code-block:: none

    CONV_LIST_PREAMBLE=
    CONV_LIST_POSTAMBLE=
    CONV_LIST_ON=__*
    CONV_LIST_OFF=
    
You should be able to see the asterisk and the two spaces, displayed above as underscores, on the third line above. None of the others have anything after the equal sign as we do not need them.    

And here we find a problem, WiClear allows indented lists, and so does DokuWiki, but while WiClear uses a number of asterisk or hyphen characters to indicate the indent level, DokuWiki needs two spaces for each additional indent.

Wiki Page Link
~~~~~~~~~~~~~~
Internal page links, which are links to other wiki pages, are very similar in DokuWiki to the WiClear syntax. The only difference is that DokuWiki uses '[[' and ']]' rather than just '[' and ']'. The following will set up internal links correctly:

..  code-block:: none

    CONV_WIKI_LINK=[[%PAGE_NAME%]]
    
The bonus feature of DokuWiki here is that the page name is converted to lower case and spaces etc, are replaced by underscores. This means that, hopefully, page links should work regardless of the Operating System in use. Additionally, when a non-existent page is linked to in a DokuWiki setup, the link appears red until the page is create, whereupon it becomes green. This can help identify problems and/or missing pages.  

However, rather than having all your pages in one place, DokuWiki allows the use of 'namespaces' to allow some separation of your Wiki topics. You could have a Linux namespace as well as a Qdos one. Obviously all the pages from the QL Wiki would go into a namespace of QL Wiki - wouldn't they?

All you need to do there is set up the following for a Wiki Page Link:

..  code-block:: none

    CONV_WIKI_LINK=[[qlwiki:%PAGE_NAME%]]

And that's all there is to it. All your Wiki pages are now deemed to live in the 'qlwiki' namespace.    


You Tube Video
~~~~~~~~~~~~~~
Video embedding in a DokuWiki page is, by default, only supported for the following video formats:

-   webm
-   ogv
-   mp4

Which means that You Tube videos cannot be embedded in a DokuWiki page. Therefore it's probably best to simply embed the ``%VIDEO_ID%`` of the ``%URL%`` of the You Tube video, in the URL of You Tube's "play a video" page (https://youtu.be/), as follows:

..  code-block:: none

    CONV_YOUTUBE_LINK={{https://youtu.be/%VIDEO_ID%|Watch this video on You Tube}}

You should note that the full URL is not valid here, so don't use ``%URL%`` if you do, all that will happen is that you will get a link that opens http://youtube.com rather than playing  the video.
    
Having said that, by default, DokuWiki cannot embed a You Tube video, `the vshare plugin <https://www.dokuwiki.org/plugin:vshare>`_ looks very promising *and* is up to date with the latest version of DokuWiki. If you have this plugin downloaded and installed, then the following might be of use:

..  code-block:: none

    CONV_YOUTUBE_LINK={{youtube>%VIDEO_ID%?medium|Watch this video on You Tube}}   
    
Which *should* display the embedded video in a medium sized frame - but as I do not have the plugin installed, I have not been able to test this. Other values are 'small' and 'large'.

The Final DokuWiki Translation File
-----------------------------------

The File Contents
~~~~~~~~~~~~~~~~~

Pulling all of the above together, we have ended up with the following translation file which is set up to use a default DokuWiki installation, with no plugins for You Tube or Definition Lists. The former is easily worked around, but the latter is not, however, as definition lists do not work on WiClear anyway, it's not a huge problem.

..  code-block:: none

    CONV_PREAMBLE= ====== %TITLE% ======
    CONV_POSTAMBLE=
    CONV_ACRONYM_LINK=%ACRONYM%
    CONV_ANCHOR_LINK=<html><span style="color:red;">%ANCHOR%</span></html>
    CONV_BLOCK_QUOTE_PREAMBLE=
    CONV_BLOCK_QUOTE_POSTAMBLE=
    CONV_BLOCK_QUOTE_LINE_ON=>
    CONV_BLOCK_QUOTE_LINE_OFF=
    CONV_BOLD_ON=**
    CONV_BOLD_OFF=**
    CONV_CITATION_LINK="%CITATION%"
    CONV_CITATION_NOSOURCE_LINK="%CITATION%"
    CONV_CODE_BLOCK_PREAMBLE=<code>
    CONV_CODE_BLOCK_POSTAMBLE=</code>
    CONV_CODE_LINE_ON=
    CONV_CODE_LINE_OFF=
    CONV_DEFN_LIST_PREAMBLE=
    CONV_DEFN_LIST_POSTAMBLE=
    # There's two spaces and a semi-colon on the next line.
    CONV_DEFN_LIST_TERM_ON=  ;
    CONV_DEFN_LIST_TERM_OFF=:
    CONV_DEFN_LIST_DESC_ON=
    CONV_DEFN_LIST_DESC_OFF=
    CONV_FORCE_LINE_FEED_ON=\\
    # There's a space at the end of the next line.
    CONV_FORCE_LINE_FEED_OFF= 
    CONV_URL_LINK=[[%URL%"|%LINK_TEXT%]]
    CONV_H1_PREAMBLE= ======
    CONV_H1_POSTAMBLE= ======
    CONV_H2_PREAMBLE= =====
    CONV_H2_POSTAMBLE= =====
    CONV_H3_PREAMBLE= ====
    CONV_H3_POSTAMBLE= ====
    CONV_HR_ON= ----
    CONV_HR_OFF=
    CONV_IMAGE_LINK={{%SRC%?%WIDTH%x%HEIGHT%|%LONG_DESC%}}
    CONV_INLINE_CODE_ON=''%%
    CONV_INLINE_CODE_OFF=%%''
    CONV_ITALIC_ON=//
    CONV_ITALIC_OFF=//
    CONV_NUM_LIST_PREAMBLE=
    CONV_NUM_LIST_POSTAMBLE=
    # There's two spaces and a hyphen on the next line.
    CONV_NUM_LIST_ON=  -
    CONV_NUM_LIST_OFF=
    CONV_PARAGRAPH_PREAMBLE=
    CONV_PARAGRAPH_POSTAMBLE=
    CONV_REFERENCE_LINK=//%REFERENCE%//
    CONV_TABLE_PREAMBLE=
    CONV_TABLE_POSTAMBLE=
    CONV_TABLE_ROW_PREAMBLE=
    CONV_TABLE_ROW_POSTAMBLE=
    CONV_TABLE_CELL_PREAMBLE=|
    CONV_TABLE_CELL_POSTAMBLE=|
    CONV_LIST_PREAMBLE=
    CONV_LIST_POSTAMBLE=
    # There's two spaces and an '*' on the next line.
    CONV_LIST_ON=  *
    CONV_LIST_OFF=
    CONV_WIKI_LINK=[[qlwiki:%PAGE_NAME%]]
    CONV_YOUTUBE_LINK={{https://youtu.be/%VIDEO_ID%|Watch this video on You Tube}}

Points to Note for DokuWiki Conversions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following points should be kept in mind when converting to DokuWiki format:

-   Wiki pages are stored as separate plain text files and do not live in any form of database. Wiki namespaces are represented as folders. So pages in a namespace are simply text files within a directory.
-   Pages *should* be kept in a namespace. In the above example file, I'm using the 'qlwiki' namespace for all my Wiki Page links. This means that in my web server's document root, I need to create a directory named ``data\pages\qlwiki`` and make sure that it is owned by the web server user (``apache``) and group (also ``apache``) and has the required read and write permissions to the user and group.
-   The top level page on any Wiki is named 'start' and lives outside of all namespaces. It lives, therefore in the folder ``data/pages/start.txt`` under the web server's document root directory.
-   Pages should be created with the name of the page, in lower case, and with spaces replaced by underscores. Make sure that you run the ``rwapWiki`` utility with the appropriate output filename. The 'Dilwyn Jones' page, for example, needs to be named 'dilwyn_jones.txt'. And obviously, needs to be copied to the document root ``data/pages/qlwiki`` directory as well.

Once the files are all converted, we need edit the existing 'start' page to create a link to the newly converted 'Sinclair QL Home Computer' page, which was the old Wiki's main page, as per the following examples:

..  code-block:: none

    Click [[qlwiki:Sinclair QL Home Computer|here]] to access the QL Wiki's main page.
    
Or something similar!

You don't have to worry about spaces, capitals etc, the DokuWiki will cater for this.
