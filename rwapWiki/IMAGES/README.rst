======================
Images For The QL Wiki
======================

The files in this directory are as follows:

-	Images.txt which is a list of all the image links and their corresponding descriptions, directly extracted by a test run of the wiki conversion software.
-	wget_list.txt which is a list of all the images in a format suitable for the ``wget`` utility to use, as follows:

	..	code-block:: none

		mkdir images
		cd images
		wget -i wget_list.txt

	The resulting output will hopefully be the 600 odd images that are used throughout the QL Wiki.

Someone somewhere might find this useful.


