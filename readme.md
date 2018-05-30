# GEDCOM Web App
## by Josh Caravaggio for CIS2750 W18

## Overview

Final Course Grade: 87%
Node.js web application with a C backend for data processing, as well as integration with a 
MySQL DB for storing genealogical family & individual data

### Purpose

GEDCOM is a standard for geneaological data that allows information about families, the individuals
those families are comprised of, the relationships between them and other information to be held
in a bytestream or file. Geanealogicaol data standards like GEDCOM are commonly used in services such as
Ancestry.com or other family tree tracing infrastructures, and as such systems capable of processing this
data are required. 

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


