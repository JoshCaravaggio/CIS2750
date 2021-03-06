# GEDCOM Web Application
## by Josh Caravaggio for CIS2750 W18

### Overview

Final Course Grade: 87%

Node.js web application with a C backend for data processing, as well as integration with a MySQL DB for storing genealogical family & individual data. Easily the most complex/difficult of software I've had to write in my university career, but at the same time the most rewarding by far.

GEDCOM is a standard for geneaological data that allows information about families, the individuals
those families are comprised of, the relationships between them and other information to be held
in a bytestream or file. Geanealogicaol data standards like GEDCOM are commonly used in services such as
Ancestry.com or other family tree tracing infrastructures, and as such systems capable of processing this
data are required. You can see some examples of GEDCOM data in the GEDCOMParserApp/uploads directory


### Functionality

The data management part of the system works by using C-based library (/parser/src/GEDCOMparser.c) to read in a specified GEDCOM file and create Individual and Family
structures in memory. The development of the parser required a whole lot of rigourous memory work, including managing circular references and 
ensuring all of the memory is freed properly. Hours and hours of segfaults. The parser manages all data in memory, but was capable of interfacing with javascript through a series of data-to-JSON inteferacing functions we had to implement as well.

The web application side of the project was done in Node.js and requests are handled with express. The front end allows users to upload their own GEDCOM format files and then use the parser library to manipulate the data in them and present the Individuals and their familial relationships in an easy-to-follow interface. The parser library is interfaced with through a foreign-function interface library, which is used to retrieve family information in JSON format. 

After the application was developed and joined with the parser, the stack was integrated with a MYSQL DB that was hosted on our school's linux server. The app is designed to throw a popup for DB credentials, which was an easy way of making sure we didn't have credentials in our code. 

Maybe one day I'll go back and document all of the JS and C functions, but as this was the first real piece of software I wrote, my documentation and coding skills were not what they are now and it might take weeks to go back and understand all of the aspects of the modules. Also, some of those functions in the parser are enourmous and I would want to make it a lot more modular before writing actual documentation.


## Startup
The following command in the GEDCOMParserApp/parser/ directory will build the required libaries for the parser

> make

Nice and simple, right? This will build the object files for the linked list library that is used as well as the parser
library that I built.

The top level is meant to run as a Node.js application, so you'll need NodeModules in the root directory of the application.
Once you get your hands on that, a simple

> node app.js

Will start it up. The DB access is limited as it was built to run on our school's linux server DB instance which has since then been altered.
Nevertheless, you can check out some of the queries we had to implement in the app.js 


### Assignment Description
The application was split into 4 assignments, each pertaining to a different aspect of the completed
web applcation.

#### A1

The purpose of the first assignment was to develop the majority of the parser, capable of taking a filename
and reading the file line by line, producing data structures with the individuals, families and other information
stored in the files. The parser was written completely in C, and was responsible for most of the data processing in
the application, returning data in JSON format to the Node server. The relationships between individual datastructures, 
as well as between individual and family structures are held in memory as well, making dealing with circular references 
just a bundle of joy to deal with.

#### A2 

The purpose of the second assignment was to finish the C parser, adding the functionality of turning data structures into
JSON objects for interfacing with Node, as well as the functionality for writing GEDCOM file objects in memory back to files.

#### A3 

The purpose of the the third assignment was to develop the Node.js application that would act as the UI for parser, allowing 
users to upload files to be parsed and turned into data structures, returning the information back in a user-friendly format.
From the UI, users could manipulate files on the server, create new files and look at individuals and families held in said files

#### A4 

The purpose of the fourth assignment was to integrate the web app with a MySQL database, used for storing individual and family information
in a remote DB. With this integration, additional functionality was added to the UI allowing users to send custom queries 
to the DB and returning the results to the UI



