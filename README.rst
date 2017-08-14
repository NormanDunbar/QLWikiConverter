QL Wiki Converter
=================

Introduction
------------

In this folder you will find a few others:

**MySQL** - contains a script (and another ``README``) that extracts the Wiki pages from the database, to a single file. Basically, you do this:

..  code-block:: none

	mysql -u root -p <content.sql >content.out

which creates the file ``content.out`` by reading the database as root. The file thus produced can be read by ``rwapWiki`` and a suitable conversion file, to create (almost) any output format you desire.


**WikiPager** - contains a utility, ``wikiPager``, or at least, the C++ source code for same, which reads the file created by the script (mentioned above) and creates IN THE CURRENT FOLDER one individual text file for EACH and EVERY Wiki page found in the file created by the afore mentioned script. This is the best way:

..  code-block:: none

    cd /location/of/content.out
    mkdir temporaryFiles
	cd temporaryFiles
	.path/to/bin/wikipager ../content.out

The pages folder will now be filled with one file named "Page Title"."language".txt for each of the Wiki pages found in the content.out file. The filenames are slightly massaged to remove punctuation and spaces which is handy as it will then match up to what the following utility creates.


**RwapWiki** - contains a utility which reads a single page file, created by WikiPager, and converts it to almost any output format you desire - provided you tell it which conversion format you need it to be in when complete.

There's an example, ``convert.html.txt`` file provided and a blank template to allow you to create your own.

The utility can hopefully be used to output text formats for other Wikis, or whatever format you need the Wiki Pages to be - text? LaTeX? XML? etc. It's up to you to provide the (``printer.dat``!) type file to make the conversions.

**WARNING 1**: It cannot handle Image Galleries, if a Wiki page has one of these, you get a warning to the standard error channel. So far, there are no galleries on the QL Wiki.

**WARNING 2**: Because you need to download images used in the Wiki pages, there is a list of all images found, and their URL, produced on the standard error channel as well. The desired output is written to the standard output channel and should be redirected to a suitable output file at your heart's desire.**




