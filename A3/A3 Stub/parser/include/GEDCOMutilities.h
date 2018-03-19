#ifndef GEDCOMUTILITIES_H
#define GEDCOMUTILITIES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"


//Represents a single line of a GEDCOM file
typedef struct {
	
	//number of line, used for error reporting
	int lineNum;
	
	//Level number of line
	int level;
    //GEDCOM tag of line
    char* tag;
    
    //Optional reference ID 
    char* ref_ID;
    
    //Optional value
    char* value;
    
} GEDCOMLine;

 //Holds a void pointer to some record data and it's respective cross-reference ID
typedef struct {
	    
    //Reference ID
    char* ref_ID;
    
    //Void pointer to data associated with the reference ID
    void* data;
    
} GEDCOMreference;


/** Function to free a field 
 *@pre Field must exist
 *@post Memory allocated to field member and field are freed
 *@return none
 *@param toBeFreed - pointer to the field data we want to free
 **/
void freeField(void* toBeFreed);
/** Function to free an Individual
 *@pre Individual must exist
 *@post Memory allocated to individual member and individual are freed
 *@return none
 *@param toBeFreed - pointer to the inidividual data we want to free
 **/
void freeIndividual(void* toBeFreed);
/** Function to free an Family
 *@pre Individual must exist
 *@post Memory allocated to family is freed
 *@return none
 *@param toBeFreed - pointer to the family data we want to free
 **/
void freeFamily(void* toBeFreed);
/** Function to free an Event
 *@pre Event must exist
 *@post Memory allocated to event is freed
 *@return none
 *@param toBeFreed - pointer to the event data we want to free
 **/
void freeEvent(void* toBeFreed);
/** Function to format a stirng date into on that is comparable with strcmp
 *@pre date string must exist
 *@post return a string represnting the date in numbers only
 *@return string of date
 *@param toBeFormated - pointer to the date data we want to format
 **/
char* formatDate(char* toBeFormated);

/** Function to convert a string to a GEDCOMLine structure
 *@pre line must be read from file
 *@post return a GEDCOMLine structure ready for processing 
 *@return GEDCOMLine
 *@param string of data from file
 **/
GEDCOMLine* convertStringToGEDCOMLine(char* toConvert, int lineNum, int previousLevel);

/** Function to check validity of a string-tokenized level 
 *@pre lstring must be non-null
 *@post return true or false based on the level's validity
 *@return boolean value
 *@param string token of level
 *@param level of previous GEDCOM line
 **/
bool validateLevel(char* toValidate, int previousLevel);
/** Function to check validity of a string-tokenized refernce ID 
 *@pre string must be non-null
 *@post return true or false based on the reference ID's validity
 *@return boolean value
 *@param string token of reference ID
 **/
bool validateReferenceID(char* toValidate);
/** Function to delete GEDCOM Line
 *@pre GEDCOM line must exist
 *@post free a gedcom line and all it's members
 *@return none
 *@param void pointer to gedcom line data 
 **/
void deleteGEDCOMLine(void * toBeDeleted);
/** Function to check validity of a string-tokenized GEDCOM tag
 *@pre string must be non-null
 *@post return true or false based on the tag's validity
 *@return boolean value
 *@param string token of reference ID
 **/
bool validateTag(char* toValidate);
/** Function to copy a GEDCOM Line
 *@pre GEDCOM line must exist
 *@post new GEDCOM Line will be returned
 *@return GEDCOMLine with identical data as the source
 *@param void pointer to gedcom line data to be copied
 **/
GEDCOMLine* copyGEDCOMLine(void * toBeCopied);
/** Function to parse an array of GEDCOMLines for information to construct a Header object 
 *@pre GEDCOM Line array must exist
 *@post new Header structure will be returned
 *@return Header structure
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
Header* createHeader(GEDCOMLine ** record, int numLines);
/** Function to parse an array of GEDCOMLines for information to construct a Submitter object 
 *@pre GEDCOM Line array must exist
 *@post new Submitter structure will be returned
 *@return Submitter structure
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
Submitter* createSubmitter(GEDCOMLine ** record, int numLines);
/** Function to parse an array of GEDCOMLines for information to construct a individual object 
 *@pre GEDCOM Line array must exist
 *@post new Individual structure will be returned
 *@return Individual structure
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
Individual* createIndividual(GEDCOMLine ** record, int numLines);
/** Function to parse an array of GEDCOMLines for information to construct a Family object 
 *@pre GEDCOM Line array must exist
 *@post new family structure will be returned
 *@return family structure
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
Family* createFamily(GEDCOMLine ** record, int numLines);
/** Function to parse an array of GEDCOMLines and concatenate lines tagged to do so
 *@pre GEDCOM Line array must exist
 *@post lines with CONC and CONT tags will be respectively processed
 *@return none
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
 
void appender(GEDCOMLine ** record, int numLines);
/** Function to tajke a string representation of a character set and return the respective enumerated type
 *@pre charset string must exist
 *@post return a charSet representing the character set
 *@return charSet
 *@param point to string to be converted to the enumerated type
 **/
CharSet decodeCharSet(char* toBeConverted);
/** Function to check validity of a header tag
 *@pre string must be non-null
 *@post return true or false based on the tag's validity
 *@return boolean value
 *@param string token of tag
 **/
bool validateHeaderTag(char* toValidate);

/** Function to handle parsing and allocation of a  
 *@pre line must be allocated
 *@post return a field object with the data that the line contained
 *@return Field object
 *@param GEDCOMLine object to be parsed
 **/
Field* createField(GEDCOMLine* line);
/** Function to read a line in from the file, basically fgets with added functionality for carriage returns
 *@pre  file pointer needs to exist
 *@post return an charcater array read in from file
 *@return pointer to character array
 *@param destination to place the parsed line into
 *@param maximum number of characters to read in
 *@param pointer to file stream that is to be read from
 **/
char * getLine(char *destination, int maxLength, FILE *fp);
/** Function to parse an array of GEDCOMLines for information to construct an event object
 *@pre GEDCOM Line array must exist
 *@post new Event structure will be returned
 *@return Event structure
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
Event* createEvent(GEDCOMLine ** record, int numLines);
/** Function to create a reference structure that contains the ref ID and a pointer to the respective data
 *@pre data must be allocated, ref_ID must be stored
 *@post new reference structure will be returned
 *@return GEDCOMreference structure
 *@param string representing ref IDs
 *@param void data pointer to point the reference at 
 **/
GEDCOMreference* createReference(char* ref_ID, void * data);
/** Empty delete function to help handle freeing circular dependencies 
 *@pre data must be allocated
 *@post Literally nothing
 *@return Still nothing
 *@param void data for nothing to be done with
 **/
void dummyDelete(void* toBeDeleted);
/** Function to check validity of the GEDCOM filename to be parsed
 *@pre string must be non-null
 *@post return true or false based on the file's validity
 *@return boolean value
 *@param string token of file name
 **/
bool validateFileName(char* toValidate);
/** Function to create a string representing an enumerated character set encoding
 *@pre string must be non-null
 *@post allocate and return a string represting the encoding
 *@return string with encoding type
 *@param CharSet to be read
 **/
char* printEncoding(CharSet encoding);


/** Function to take an individual and a list and recursively add that
 * individual's children to the list, then all of their childen's children, etc...
 *@pre Individual must be non null, list must be initialized with dummyDelete function
 *@post the pointer list that gets passed in will be populated with the person's descendents 
 *@return none
 *@param pointer to descendent list, which is to be populated
 *@param pointer to individual whose descendants are to be found
 **/

void recursivelyAddDescendants(List *descendantList, const Individual* currentPerson);
/** Function to write a header to a file stream.
 *@pre Header must exist, file pointer must be opened and non-NULL
 *@post The header is written to the file 
 *@return GEDCOMerror representing success of file write
 *@param pointer to file to be written to
 *@param pointer to header whose information is to be written
 **/
GEDCOMerror writeHeader(FILE * fp, Header* header);
/** Function to convert a CharSet enum to a string
 *@pre Header must exist
 *@post A string version of the encoding is returned
 *@return string version of encoding
 *@param CharSet to encode
 **/
char* encodeCharSet(CharSet toEncode);
/** Function to write a Submitter to a file stream.
 *@pre Submitter must exist, file pointer must be opened and non-NULL
 *@post The Submitter is written to the file 
 *@return GEDCOMerror representing success of file write
 *@param pointer to file to be written to
 *@param pointer to Submitter whose information is to be written
 **/
GEDCOMerror writeSubmitter(FILE * fp, Submitter* submitter);
/** Function to write an individual to a file stream
 *@pre Individual must exist, file pointer must be opened and non-NULL
 *@post The individual is written to the file 
 *@return GEDCOMerror representing success of file write
 *@param pointer to file to be written to
 *@param pointer to individual whose information is to be written
 *@param array of GEDCOMreference objects reperesenting all of the references in the file
 *@param number of entries in reference array
 **/
GEDCOMerror writeIndividual(FILE* fp, Individual * individual, GEDCOMreference** referenceArray, int referenceCount);
/** Function to write a header to a file stream.
 *@pre Header must exist, file pointer must be opened and non-NULL
 *@post The header is written to the file 
 *@return GEDCOMerror representing success of file write
 *@param pointer to file to be written to
 *@param pointer to header whose information is to be written
 **/
GEDCOMerror writeFamily(FILE * fp, Family* family,GEDCOMreference ** referenceArray, int referenceCount);
/** Function to write an event into a file for writing individuals or families
 *@pre Event must exist, file pointer must be opened and non-NULL
 *@post The event is written to the file 
 *@return GEDCOMerror representing success of file write
 *@param pointer to file to be written to
 *@param pointer to header whose information is to be written
 **/
GEDCOMerror writeEvent(FILE * fp, Event* event);
/** Function to take an individual and a list and recursively add that
 * individual's children to the list in the form of a generation list, up to a number of generations
 * specified by maxGen
 *@pre Individual must be non null, list must be initialized with dummyDelete function
 *@post the pointer list that gets passed in will be populated with the person's descendents 
 *@return none
 *@param pointer to descendent list, which is to be populated
 *@param pointer to individual whose descendants are to be found
 *@param counter representing the current generation count
 *@param maximum number of generations to collect data on
 **/
void recursivelyAddDescendantsN(List *descendantList,List* currentGeneration,  const Individual* currentPerson, int counter, int maxGen);
/** Function to check if an individual is a child in that family
* @pre Individual must be in that family
* @post function indicates whether individual is a child in the family or parent
* @return true or false based 
* @param family that individual is in
* @param individual
**/
bool isChild(Family* family, const Individual* individual);
bool isEmpty(List* list);
void recursivelyAddAncestorsN(List *ancestorList,List * currentGeneration ,const Individual* currentPerson, int counter,int  maxGen);


bool listContains(List* list, void* target );
int testFunc();
char* GEDCOMtoJSON(char* fileName);
char* getIndividualsFromGEDCOM(char* fileName);
char * createNewGEDCOM(char* JSONstring, char* fileName);
char* addIndividualToGEDCOM(char* fileName, char* indData );
//int compareIndividualsLists(const void* first,const void* second);
//char* printIndividualList(void* toBePrinted);
//void deleteIndividualList(void * toBeDeleted);

#endif
