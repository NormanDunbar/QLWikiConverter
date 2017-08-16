=======================================
Converting the QL Wiki to Other Formats
=======================================


Introduction
============

For full details, see `this topic <http://qlforum.co.uk/viewtopic.php?f=12&t=1832>`_ on `The QL Forum <http://qlforum.co.uk>`_.

Currently the QL Wiki, amongst others, is hosted and maintained by Rich of RWAP Services. It has been noted that other Wikis etc have foundered on the basis of "something" happening to the one single maintainer. Rich wants to avoid this happening.

To this end, a number of suggestions have been put forward on the matter (read the whole topic at the link above) and conversion to another format was suggested. Where and how (or who) would be hosting the converted Wiki is/was/has yet to be decided, as has the format - there are Wikis aplenty out there!

To make the task of converting the existing, and not really well known, *WiClear* Wiki (`http://wiclear.free.fr/ <http://wiclear.free.fr/>_`) source code from the format used by WiClear to some other format, I wrote this utility. It is possibly more important as the WiClear software has not been updated since 2007 and who knows what bugs or security problems exist?

*What other format* being the question. There have been a few suggested and a couple of trials have seemingly taken place. The utility, ``rwapWiki`` attempts to solve this problem by allowing ahem, *almost* any other format you desire - provided a translation file can be created for it.

This document attempts to explain how the system works, what it converts and how. This might make writing new translation files a little easier. The utility is supplied with a single, text, HTML translation file which works *reasonably* well - see the section on *Problem Areas Identified* below.


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

..  code-block:: none

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

..  code-block:: none

    @echo off
    
    set INFILE=%1
    set OUTFILE=%INFILE:.en.txt=.html%
    rwapWiki\rwapWiki Pages\%INFILE% rwapWiki\convert.HTML.txt >HTML\%OUTFILE%

We need this file because a ``FOR /F`` loop in Windows doesn't seem to be able to cope with being able to run more than one command. There are no errors, but the second and subsequent commands can no longer see the value passed in ``%F``.

Now we need a loop to read the files from the ``Pages`` directory, and convert each one by calling the ``PageIt.cmd`` file created above. **Warning**: The following *only* works if you are typing directly into a command window:

..  code-block:: none

    for /F "usebackq tokens=1" %F in (`dir /B Pages\*.en.txt`) do (PageIt.cmd %F)

The single quotes used above are back ticks (`) and not the familiar single quotes (') we know and love for strings in the QL. They don't tend to work the same! 

Did I mention that ``%F`` above is case sensitive? You must use the same case each time you type it. Also, it can only be a single letter long - Anyone remember ZX-81 variables? I'm sure that they were the same! Progress eh?
    
If you want to do the above in a command file, then create one, ``PageAll.cmd`` for example, with the following command in it:

..  code-block:: none

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

..  code-block:: none

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

    <CONV_BLOCK_QUOTE_PREAMBLE><CONV_BLOCK_QUOTE_LINE_ON> ... <CONV_BLOCK_QUOTE_LINE_OFF><CONV_BLOCK_QUOTE_POSTAMBLE>
    
Where '...' represents the block quoted text.

    **NOTE**: The double arrow at the start of the second line above is *not* part of the conversion, it merely indicates that the code line has had to be split to fit it onto the page and that it should be all on a single line.

We have two prefix variables and two suffix variables to content with as some output formats may require an prefix/suffix for the block quote, and an prefix/suffix for each line of text within. HTML requires this, as follows:

..  code-block:: none

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

..  code-block:: none

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

..  code-block:: none

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

..  code-block:: HTML
  
    <a href="Dilwyn-Jones.html">Dilwyn Jones</a>

Thus creating a link to the page with the title 'Dilwyn Jones'. The file that the 'Dilwyn Jones' page lives in is assumed to be 'Dilwyn-Jones.html' in *exactly* that letter case. However, some pages in the Wiki have interesting characters in their titles, so the generated filenames are a bit off, at least in Windows. 

Also problematical is the fact that some page links in the Wiki source assume case insensitivity, and as filenames on Linux are case sensitive, that can cause links not to work.

See the section on *Problem Areas Identified* for more details.

See also, the section on *WikiPager* for details of how that utility splits the database content into separate page files, each named after the page name in the database.

URL Links
"""""""""

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

..  code-block:: HTML

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



Substitution Variables
----------------------

Some, but not all, translation variables allow certain parts of the Wiki source text to be extracted and used in the translation text, perhaps in a different place or order. The following table lists all current substitution variables and shows the translation variables that are permitted to use them.

+-------------------+-----------------------------+---------------------------------------------------------------------+
| Variable Name     | Used in                     | Description                                                         |
+===================+=============================+=====================================================================+
| %ACRONYM%         | CONV_ACRONYM_LINK           | The acronym part of a Wiki acronym.                                 |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %ALIGN%           | CONV_IMAGE_LINK             | Alignment code for an image, one letter from 'lLgG' or 'rRdD'.      |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %ALIGN_EXPAND%    | CONV_IMAGE_LINK             | Expanded alignment for an image, left, right.                       |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %ALT_TEXT%        | CONV_IMAGE_LINK             | ALT text for an image.                                              |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %ANCHOR%          | CONV_ANCHOR_LINK            | The anchor text for a Wiki anchor.                                  |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %CITATION%        | CONV_CITATION_LINK          | The citation text in a citation with source text.                   |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %CITATION%        | CONV_CITATION_NOSOURCE_LINK | The citation text in a citation with no source text.                |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %COMPRESSED_NAME% | CONV_WIKI_LINK              | The Wiki page name with spaces and punctuation replaced by hyphens. |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %HEIGHT%          | CONV_IMAGE_LINK             | Height of an image.                                                 |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %LANGUAGE%        | CONV_URL_LINK               | The language code, two letters, for a URL. Not likely to be used.   |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %LINK_TEXT%       | CONV_URL_LINK               | The text to be displayed as a clickable link in a URL.              |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %LONG_DESC%       | CONV_IMAGE_LINK             | Popup text when image hovered over. Long description of an image.   |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %PAGE_NAME%       | CONV_WIKI_LINK              | The Wiki page name.                                                 |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %REFERENCE%       | CONV_REFERENCE_LINK         | The text of a Wiki reference.                                       |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %SOURCE%          | CONV_CITATION_LINK          | The source of a citation. May not always be present.                |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %SRC%             | CONV_IMAGE_LINK             | Source URL of an image.                                             |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %TITLE%           | CONV_PREAMBLE               | The title of a Wiki Page file. Taken from the input filename.       |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %TITLE_TEXT%      | CONV_ACRONYM_LINK           | The explanation text of a Wiki acronym.                             |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %TITLE_TEXT%      | CONV_ANCHOR_LINK            | The title text for a Wiki anchor.                                   |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %TITLE_TEXT%      | CONV_URL_LINK               | The popup text for a hovered URL link.                              |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %URL%             | CONV_URL_LINK               | The web address to be linked to in a URL.                           |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %URL%             | CONV_YOUTUBE_LINK           | The URL for a You Tube video.                                       |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %VIDEO_ID%        | CONV_YOUTUBE_LINK           | The video id extracted from a You Tube URL after ``?v=``.           |
+-------------------+-----------------------------+---------------------------------------------------------------------+
| %WIDTH%           | CONV_IMAGE_LINK             | Width of an image.                                                  |
+-------------------+-----------------------------+---------------------------------------------------------------------+


+-------------------+---------------------------------------------------------------------+
| Variable Name     | Description,                                                        |
+===================+=====================================================================+
| %ACRONYM%         | The acronym part of a Wiki acronym. Used in                         |
|                   | CONV_ACRONYM_LINK.                                                  |
+-------------------+---------------------------------------------------------------------+
| %WIDTH%           | Width of an image. Used in CONV_ACRONYM_LINK.                       |
+-------------------+---------------------------------------------------------------------+







