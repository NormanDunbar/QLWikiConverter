=======================================
Converting the QL Wiki to Other Formats
=======================================


Introduction
============

WorkFlow
========

This section explains the process of converting the *latest* version of the QL Wiki, to some other format.

Briefly
-------

The workflow is simple:

-   Drop the existing wiki database.
-   Import the latest dumpfile.
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

Page names which have accented characters cause no problems on Linux, where such characters are permitted in file names (Assuming UTF8 is in use, which is usually is by default). The filename generated on Windows, however, is different as it doesn't seem to be able to cope with the accented characters. For example, the page entitled *Jurgen Falkenberg* has the 'u' with an umlaut above it. On Linux this is fine, on Windows it generates a file named 'JA1/4rgen-Falkenberg.html' instead where the 'A' has a tilde (~) above it and the '1/4' is a single character (and not 3 as shown here). Links in the generated files correctly go to the file with the accented 'u' as per Linux, but the page cannot be found as it doesn't exist with that name.

The problem exists for the following pages (accented characters removed!):

-   Huthig Verlag
-   Jurgen Falkenberg
-   Janko Mrisc-Flogel
-   Schon
-   Schon KBL128 QL Case

There may be others.

Image Galleries
---------------

These cannot be converted. Sorry. However, as there are none in the QL Wiki, it's not a problem. There is one in the sandbox area, but that's for messing about in, so it's not considered a problem, and is not linked to from any of the QL pages.

Images
------

The original Wiki required that images were set up as a URL linking to images that existed *somewhere on the internet*, there was no apparent ability to load images from local files. The conversion will list all the images it finds, but will not (can not) download them. Image links in the converted files will still refer to the original locations - or as decreed by the conversion requirements.

Equally, the description for an image (the text that appears when you hover over it) will either be some descriptive text, or, a link to a text file *somewhere on the internet* where the descriptive text can be found in the named file. Again, the conversion program will list the descriptive text or filenames as appropriate as part of the conversion.

If you decide to download the individual images locally, you will obviously need to edit the generated HTML files to change the "src=" attribute on the "<img>" tags accordingly. Please look in the IMAGES/wget_list.txt file for a full list of all the images that are linked to in the QL Wiki (as of 28th July 2017.) This file can be used as follows:

..  code-block:: none

    mkdir Images
    cd Images
    wget -i /path/to/wget_list.txt
    
When completed, there should be a whole lot of images in the ``Images`` directory. In testing, I've had a few problems as I'm at work and the proxy server filters out a lot of files, leaving me with a lot of JPEG image files (``*.jpg``) containing a 2 by 1 white pixel in GIF format! Coincidentally, I get a similar image for a lot of the Wiki pages when I attempt to open the image URL directly from my browser, so it's definitely a work problem!   


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

The following features of the Wiki have special characters at the start of a line to indicate what the line is to be used as. ``RwapWiki`` deals with these line starts, and when done, tries to process any embedded features left in the translated line. These are covered below in the *Embedded Formatting* section.

When a line start is processed, we cannot be in a paragraph any longer, so if we were in one, we close it

Headings
~~~~~~~~

Heading lines start with one, two or three exclamation marks (!). These indicate the heading level, with '!!!' indicating level 1 and '!' indicating level 3. Only three levels of heading are permitted. There may be embedded formatting such as bold or italic, so after conversion here, the line is processed for any valid embedded formatting. (See below for details).

A heading from the input file will resemble the following:

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
    
For the example HTML conversion, this will be blank for 'CONV_HR_OFF' and ``<br>`` for 'CONV_HR_ON'.

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

**YOU ARE HERE**

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
----

Bold text is created as follows:

..  code-block:: none

    Normal text __ bold stuff __ normal text again.

This can occur anywhere in the line. In addition, bold text may start on one line of a paragraph, extend over one or more lines, and then end on a subsequent line later on in the paragraph. Bold text may also be embedded within some other formatting, which itself may span multiple lines, italic for example. There may also be multiple bold texts on a line. ``RwapWiki`` correctly handles these situations.    

Bold text is converted to the following:

..  code-block:: none

    ... <CONV_BOLD_ON> bold stuff <CONV_BOLD_OFF> ...
    
Italic
------

Italic text is created as follows:

..  code-block:: none

    Normal text '' italic stuff '' normal text again.

This can occur anywhere in the line. In addition, italic text may start on one line of a paragraph, extend over one or more lines, and then end on a subsequent line later on in the paragraph. Italic text may also be embedded within some other formatting, which itself may span multiple lines, bold for example.  There may also be multiple italic texts on a line. ``RwapWiki`` correctly handles these situations.    

Italic text is converted to the following:

..  code-block:: none

    ... <CONV_ITALIC_ON> italic stuff <CONV_ITALIC_OFF> ...
    
Inline Code
-----------

Inline code text is created as follows:

..  code-block:: none

    Normal text @@ Some code stuff @@ normal text again.

This can occur anywhere in the line. In addition, code text may start on one line of a paragraph, extend over one or more lines, and then end on a subsequent line later on in the paragraph.  There may also be multiple code text on a line.``RwapWiki`` correctly handles these situations.    

Inline code text is converted to the following:

..  code-block:: none

    ... <CONV_INLINE_CODE_ON> Some code stuff <CONV_INLINE_CODE_OFF> ...
    
Links
-----

Wiki Page Links
~~~~~~~~~~~~~~~

URL Links
~~~~~~~~~

You Tube Video Links
~~~~~~~~~~~~~~~~~~~~

Forced Line Feed
----------------

A forced line feed, in the Wiki, is created thus:

..  code-block:: none

    yada yada %%% text after the linefeed ...
    
and is converted to the following:


..  code-block:: none

    yada yada <CONV_FORCE_LINE_FEED_ON><CONV_FORCE_LINE_FEED_OFF> text after the linefeed ...
    
There may be more than one forced line in a single Wiki source line, so all of them are replaced. In the example HTML conversion file, 'CONV_FORCE_LINE_FEED_ON' is set to ``<br>`` and 'CONV_FORCE_LINE_FEED_OFF' is blank.

Citations
---------

#========================================================================================
#
#
#========================================================================================
# Citations.
#
# %CITATION% is the citation text.
# %SOURCE% is who said it.
#
# OOPS! It seems that in the Wiki source code, citations are allowed without the second
# part - who said it (or a ref link etc). The following won't work and we need to just
# sort it out. Sigh.
#----------------------------------------------------------------------------------------
CONV_CITATION_LINK=<abbr title="%SOURCE%">%CITATION%</abbr>
CONV_CITATION_NOSOURCE_LINK=<blockquote>%CITATION%</blockquote>

References
----------

Anchors
-------

I could never seem to get an anchor to work in a Wiki sandbox page! MAybe they don't. They certainly didn't seem to do much. However ...

Anchors are created in the Wiki as follows:

..  code-block:: none

    ... ~~anchor|title~~ ...
    
There are, as can be seen, two parts to an anchor, the anchor text and the title text. The conversion utility, ``rwapWiki`` processes these and converts them by reading the variable ``CONV_ANCHOR_LINK`` and scanning it for the text '%ANCHOR%' and '%TITLE_TEXT%', and for each occurrence, replaces the found text with the appropriate part of the Wiki source's anchor code. 

The example HTML conversion uses the following as the ''CONV_ANCHOR_LINK'' replacement text:

..  code-block:: none

    <abbr title="%TITLE_TEXT%">%ANCHOR%</abbr>
    
And so, decrees that an anchor in the Wiki shall be created as an abbreviation in the HTML. This is easily changed of course.  

There can be more than one anchor per line, but anchors must fully exists on a single line, continuations are not permitted.

If the replacement text does not have one, or both, of the replacement text "macros", then those parts of the anchor text from the input file will be missing from the output.    

Acronyms
--------
CONV_ACRONYM_LINK=<abbr title="%TITLE_TEXT%">%ACRONYM%</abbr>

Images
------


Lists
-----

Unordered List
~~~~~~~~~~~~~~

Ordered List
~~~~~~~~~~~~

Definition List
~~~~~~~~~~~~~~~

Citations
---------

References
----------

Anchors
-------

Acronyms
--------


Code Blocks
-----------

Table Rows
----------


Building a Conversion File
==========================




********************
OLD SHIT BELOW HERE!
********************



Introduction & Disclaimer
=========================

The following information has been extracted from the raw database data for Rich's Sinclair QL Wiki. The Wiki system is WiClear, French\ [#]_, and appears not to have been updated since 2007\ [#]_.

Any errors or mistakes are mine. As they say! I've extracted this on my work computer which is not allowed, nor has access to the WiClear software, or even to the web site! I'm floundering around, a little more blindly than usual, in the dark! Be gentle with me - I don't even have access to a MySQL database, or, to the Sinclair QL Wiki on Rich's domain as it's classified as "games" by our internet "thought police" system. :-(


Formatting Characters
=====================

Line Feed
---------

The line feeds in the Sinclair QL Wiki pages seem to be in Windows format. Thus, a carriage return followed by a line feed. 

::

    CHR$(13); CHR$(10);
    
However, looking in the Wiki's source code, it is seen that a linefeed is either:

-   ``CHR$(13)`` for Mac users;
-   ``CHR$(10)`` for Unix/Linux users;
-   ``CHR$(13); CHR$(10)`` for Windows users;
    

Single line feeds are used to terminate titles etc, double line feeds are used to separate paragraphs.


Smilies
-------

The text ``:-)`` will be rendered into an inline graphic 


Paragraph
---------

Simply a lump of text, with or without embedded formatting, followed by 2 line feeds.


Horizontal Line
---------------

Four, or more, equal signs '=' plus a line feed.

*Possibly* requires to be on a line of it's own?


Lists
-----

Simple Lists
~~~~~~~~~~~~

One or more '*' or '-' per item plus a line feed. For example::

    * Item 1
    * Item 2
    - Item 3
    - Item 4

The more '*' or '-'there are, the more nested the list is. Mixing and matching the special characters, as above, makes no difference, you still hget a list with the same bullet characters on rendering the page.

I suspect most sensible people would use the same character throughout the list!


Numbered Lists
~~~~~~~~~~~~~~

One or more '#' per item, plus a line feed.

::

    # Item 1, numbered 1.
    # Item 2, numbered 2.
    # Item 3, numbered 3.
    
-   Nesting?
-   How to start a list from a specific number?
-   How to reset a list numbering part way through?
-   Only ASCII digits permitted?


Definition Lists
----------------

The format is:

A semi-colon ';' followed by the term, a colon followed by the definition plus a line feed::

    ;Glossary:Something that explains things
    
However, this doesn't appear to render correctly in generating the page. Even the Wiki's own help page shows a failure to render these things. The source code for the 2007 release, does show that the format is accepted, and that it should render as an HTML ``<dl>..</dl>`` set of tags, but in the rendered page, you just get the semi-colon etc.


Tables
------

A pipe symbol, '|' followed by the text of the cell. Multiple cells per line are permitted. A line feed indicates the end of the table row. For example::

    | Cell 1 | Cell 2 | Cell 3 
    | Cell 4 | Cell 5 | Cell 6 
    | Cell 7 | Cell 8 | Cell 9 
    
-   Is a trailing '|' necessary in each row? Or is the trailing linefeed sufficient
-   How do we get heading cells? 
-   Can we have rowspans and/or colspans? If so, how? 
-   What about alignment of cell data?


Titles/Headings
---------------

!!!Heading_text plus a line feed = Heading level 1. For example::

    !!!This Is Heading Level 1
    
!!Heading_text plus a line feed = Heading level 2. For example::

    !!This Is Heading Level 2
    
!Heading_text plus a line feed = Heading level 3. For example::

    !This Is Heading Level 3
    

Preformatted Text/Code Listings
--------------------------------

A space, (Just one? What if there are more?) plus the text, plus a line feed. For example::

     1000 CLS
     1005 REPeat Madness
     1010   PRINT !'Hello World! '!
     1015 END REPeat Madness

Ok, so it's hard to see the single leading space in the above, but trust me, it's there! Just in front of the line numbers.


Block Quotes
------------

A '>' followed by the text, followed by a line feed.

::

    >The boy stood on the burning deck,
    >His hands were all in blisters,
    >Feel free to fill in all the rest,
    >But don't dare mention sisters!


Text Formatting
---------------

Bold/Strong
~~~~~~~~~~~

Two underscores '_' followed by the text to be emboldened, then two terminating underscores. No spaces between the text and the leading or trailing underscores.

::

    This text is __BOLD__ if I got this correct!

Italic
~~~~~~

Two single quotes, followed by the text, then two terminating single quotes. No spaces between the text and the leading or trailing single quotes.

::

    This text is ''ITALIC'' if I got this correct!


Inline Code
~~~~~~~~~~~

Inline code appears, from the Wiki's source code, to be defined as a pair of 'at' signs - ``@@`` - then the code to be inlined, and then another pair of 'at' signs. For example:

::

    The statement @@CLS #1@@ will clear the screen associated with channel 1.
    
However, trying it out in the Wiki sandbox has no effect, so it doesn't actually work. :-(
    

Forced line feed
~~~~~~~~~~~~~~~~

Three percent signs '%%%' together will force a line break at that point. It appears that this doesn't need to be on a line of its own. Embedded works just as well.


Links
-----

HTTP Links
~~~~~~~~~~

A link is an open square bracket '[' followed by up to? 4 fields each separated by a pipe character '|', followed by a closing square bracket ']'. The first field is mandatory, the remainder are optional, and define the following::

    [linkname|link|language|description(title)]
    
-   Linkname: The clickable text that appears in the Wiki page.
-   Link: The URL that the link points to.
-   Language: Language code. Should be a two character language code, as defined in the ``WC_LANG`` table in the database, wrapped in double quotes.

    The following are allowed:

    -   en: English;
    -   fr: French;
    -   de: German;
    -   ru: Russian;
    -   sl: Slovene;
    -   nl: Dutch;
    -   it: Italy;
    -   es: Spanish.
    
    Some of the actual wiki pages appear to have an 'illegal' language code, "eng-uk" for example, I *suspect* those are simply ignored.
-   Description: Pop up text when the link text is hovered over.
    
Examples::

    [Quanta|http://www.quanta.org.uk|eng-uk|Quanta User Group]
    
    [web based forum|http://www.qlforum.co.uk]
    
The latter only has two of the four fields supplied.    
    

Wiki Links
~~~~~~~~~~

Cross page Wiki Links appear to only require the first field in the above. For example::

    [Psion]
    
Will take you to the wiki page with the title of 'Psion'.

Images
------

::

    ((source|alt|align|long description|width|height|???)) 
        
-   A pair of opening round brackets '(('
-   Followed by the source URL for the image and then a pipe character '|' 
-   The 'alt' text for the image and another pipe
-   The text "align=" followed by the double quoted alignment character (see below), then another pipe
-   The long descriptive text for the image and yet another pipe character
-   The image width in pixels as it will be displayed on the wiki page and yet another pipe 
-   Then the image height in pixels as it will be displayed 
-   Finally, a pair of closing brackets '))'.

Except it's not quite 'finally' as some of the examples in the database have shown to have an extra attribute! See below.


Alignment
~~~~~~~~~

Alignment is the word "align" followed by an equal sign '=' followed by a double quoted single character as follows:

-   "r", "R", "d" or "D" = Right/Droit.
-   "l", "L", "g" or "G" = Left/Gauche\ [#]_.

The following example, extracted from the data, shows however an additional attribute, 'POPUP' tagged on at the end. I assume this means that clicking on the image will pop up a full sized image?

::

    ((http://www.rwapadventures.com/images/hardware/sinclair_ql.jpg|Sinclair QL Home Computer|align="L"|A Sinclair QL Home Computer|240|180|POPUP))
    

Videos
------

Only You Tube videos can be linked to, from the Wiki's pages. The format of a video link as as follows::
    
    (vid)youtube_webpage_address(/vid)
    
It's almost looking sort of like perhaps XML or HTML now!    


Image Gallery
-------------

::

    %%dir|title|width|height%%

*UNTESTED*

It appears that a folder full of images can be displayed as an image gallery using the above format. One assumes, or at least I assume, that the directory is named, a title is used on the page, and the images are all displayed in the same width and height, assumed to be in pixels? I wonder what happens with a mixture of portrait and landscape images in the same folder?

    
Citations & References
----------------------

*UNTESTED*

A citation has the following format::

    ^^sentence|source link^^

While a reference will have the following format::    
    
    {{reference}}

What, exactly, is the reference referencing? If it's a citation, how does the reference reference the citation? By the sentence part?    

Maybe it's referring to an anchor? See below.


Acronyms
--------

*UNTESTED*

Acromyms have the following format::

    ??acronym|signification??
    
How one uses them in a Wiki page is as yet, unclear. I assume something like::

    ??SQL|Structured Query Language?? is an unstructured query language that allows us to interrogate relational databases.

I suspect that the text of the acronym, 'SQL' in this case, is inserted into the sentence on the wiki page, but the text 'Structured Query language' will only appear when 'SQL' is hovered over. That's how it works on other wikis anyway.


Anchors
-------

*UNTESTED*

Anchors have the following format::

    ~~anchor|title~~

How one uses them in a Wiki page is as yet, unclear, perhaps a reference is used to reference the anchor?


Redirection
-----------


*UNTESTED*

A redirection is simply this::

    &url&


Raw HTML
--------

Raw HTML can be included in a wiki page simply 'as is':

    <b>This is bold HTML text</b>

    
..  [#] Not that that's a bad thing!
..  [#] This might be a bad thing as there *must surely* have been security problems since 2007 that should be fixed?
..  [#] The author of WiClear is French, hence the use of 'Droit' and 'Gauche' for Left and Right! At least he allowed us English speakers to use 'L' and 'R'!