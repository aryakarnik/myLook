MyLook.c

GitHub Link: https://github.com/aryakarnik/myLook
-------------------------------------------------------------------------

OVERVIEW:
MyLook.c is an implementation of the "look" unix command.
The main function of this command is to help the user spell by "looking" through a dictionary for the word he/she wants.
The user just has to type the first few letters of the word and then a list of all the words that start with that prefix 
will be printed. 

The default "dictionary" the command looks through is "/usr/share/dict/words".
However if the user chooses, he/she can specify a self made "dictionary" which can be any sorted file.
-------------------------------------------------------------------------

COMPILATION:
In order to compile myLook.c, the user can use the Makefile provided by typing in "make myLook"
or can manually compile the program by typing in "gcc myLook.c -O3 -Werror -Wall -o myLook".

This will create an executable you can run named "myLook".
-------------------------------------------------------------------------

RUN:

myLook string [-d] [-f] [-t termchar] [file]

1) myLook prints any lines in "file" that contain "string" as a prefix. If there is not specified "file",
   the default dictionary "/usr/share/dict/words" is used.
2) There are three flags that can be used to help the user look more specifically:
    a) -d: This flag ignores any characters that are not alphanumeric. 
           So "hi" and "h.i." will be seen as the "same string" and both will be printed if the string the user is looking
           for is "hi".
    b) -f: This flag ignores the case of the characters.
           So "HI" and "hi" will be seen as the "same string" and both will be printed if the string the user is looking
           for is "hi".
    c) -t: This flag takes the next command line argument and uses that as the ending character of the string you are comparing.
           So if the string the user is trying to find is "hello" and the user uses "-t e"
           The string that myLook looks for is changed from "hello" to "he".
