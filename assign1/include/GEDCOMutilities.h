#ifndef GEDCOMUTILITIES_H
#define GEDCOMUTILITIES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#

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
GEDCOMLine* convertStringToGEDCOMLine(char* toConvert, int lineNum);

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
/** Function to parse an array of GEDCOMLines for information to construct a Header object with
 *@pre GEDCOM Line array must exist
 *@post new Header structure will be returned
 *@return Header structure
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
Header* createHeader(GEDCOMLine ** headerRecord, int numLines);
/** Function to parse an array of GEDCOMLines for information to construct a Submitter object with
 *@pre GEDCOM Line array must exist
 *@post new Submitter structure will be returned
 *@return Submitter structure
 *@param pointer to array of GEDCOM Line pointers
 *@param number of lines in the array 
 **/
Submitter* createSubmitter(GEDCOMLine ** headerRecord, int numLines);
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
char * getLine(char *dst, int max, FILE *fp);

#endif
