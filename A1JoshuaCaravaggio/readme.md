
#  CIS2750 Assignment1 Documentation
## GEDCOM Parser
## Joshua Caravaggio
## 0963429



### Program Description


###createGEDCOM

The createGEDCOM function handles the parsing of a the gedcom file whose name is passed
in as the parameter fileName. If the file is not a gedcom file, the file is invalid or cannot
be opened the parser returns an INV_FILE error.	
	
The first portion of the parser's functionality revolves around a while loop that uses the
getString helper function, which is effectively a revision of fgets that accounts for the specific
line terminators that GEDCOM files written on different operating systems. The loop grabs a string 
of max length 256 and attempts parse it into a struct of type GEDCOMline, which is a structure I
implemented in order to handle base-level grammar errors in line immediately after reading them in,
as well as making the line's level, tag, cross-reference ID and value accessible as part of a structure
instead of having to store them in character arrays. This does, however, come at the cost of a great
deal of memory allocation, but makes the error detection substantially more stable and predictable.

After the line is converted into a GEDCOMline structure, its level is checked. If the level is not 0,
the recordCounter integer is incremented, which keeps track of how many records are in the current one being
read (a record being a subsequent series of GEDCOM lines from the 0th level line up until the 0th line of the
next record). If the level is zero, except for on the first line in the file, all of the lines since the last
zero are memcpy'd into an array of GEDCOMlines called currentRecord (to save memory & not have to free both 
copies). 

After the currentRecord array is loaded, the first line of the record is analyzed to figure out what 
type of record it is. Based on its tag, the record is parsed into either a header, submitter, individual or family,
unless an error is encountered while parsing. In the case of an error being found, all the data read in up to that
point will be free'd and an error will be constructed and returned from the function. If an individual, family or
submitter record is found, a structure called a GEDCOMreference is created which contains the record's cross-reference
id and a pointer to the data itself. This GEDCOMreference is used later in the parser for linking the structures,
and is passed into an array of references called referenceArray. After the end of the file is reached, an if block
checks that a header and trailer record were found, and if not returning the appropriate INV_GEDCOM error. 

After the record construction, a series of while loops pass through the reference array and link the header
to the submitter, each family with their respective wife, husband and children and each individual with their
respective family structures, which is imperative to the getDescendants funtion. After the linkages are performed,
the function's memory free'd and the OK error is created and returned.

 
### printGEDCOM

The printGEDCOM function takes a GEDCOMobject and works its way through all of it's members and lists,
concatenating data as it goes to a string that eventually contains a hunman-readable version of all of the 
data in the object. After the header, submitter and every element in the individuals and families lists 
are converted to strings and concatenated, the string is returned.


### deleteGEDCOM
	
The deleteGEDCOM function frees all of the data contained by the object that gets passed into it
	
### printERROR

Allocates and returns a human-readable version of the GEDCOMerror object passed into it
	
### findPerson

Attempts to find the person passed into the structure in the parametrized GEDCOMobject's individual list.
Individuals are compared based on the predicate compare function that is passed into the function.

### getDescendents

The getDescendents function recursively searches through the linkages of families in order to populate a 
list (intialized with a soft delete function as to not cause double frees) with all of the descendants of the
Individual passed into the function. The function calls the helper recurseivelyAddDescendants, which takes a
list of individuals to pass the descendants into and a current person. The function goes into the individual's
family list, and if the individual is a parent in that family, all of the children in the family are added to
the getDescendants list, and the function is recursed passing each of the children into the function.


	


