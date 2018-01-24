#ifndef GEDCOMPARSER_H
#define GEDCOMPARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "LinkedListAPI.h"

//For simplicity, the examples we will use will only use the ASCII subset of these encodings 
typedef enum cSet {ANSEL, UTF8, UNICODE, ASCII} CharSet;

//error code enum
typedef enum eCode {OK, INV_FILE, INV_GEDCOM, INV_HEADER, INV_RECORD, OTHER} ErrorCode;

//Represents a single line of a GEDCOM file
typedef struct {
	
	//Level number of line
	int level;
    //GEDCOM tag of line
    char* tag;
    
    //Optional reference ID 
    char* ref_ID;
    
    //Optional value
    char* value;
    
} GEDCOMLine;

//Represents a generic event, e.g. individual event, family event, etc.
typedef struct {
    //The max length of this field is known from the GEDCOM spec, so we can use a statically allocated array
    char type[5];
    
    //Empty string if not provided
    char* date;
    
    //Empty string if not provided
    char* place;
    
    //All other event fields. All objects in the list will be of type Field.  It may be empty.
    List    otherFields;
    
} Event;

//Represents a generic field.  This will be used in A2.
typedef struct {
    //Field tag.  Must not be NULL/empty.
    char* tag;
    
    //Field value.  Must not be NULL/empty.
    char* value;
} Field;

//Represents a submitter record.  This is a separate type/struct, in case we decide to expand it in later assignments
typedef struct {
    //Submitter name has a max length and only appears once, so we can hardcode it
    char    submitterName[61];
    
    //All other submitter fields. All objects in the list will be of type Field.  It may be empty.
    List    otherFields;
    
    //Submitted address.  We use a C99 flexible array member, which we will discuss in class.
    char    address[];
} Submitter;

/*
 Represents the GEDCOM header
 Only includes required fields ("line values" in GEDCOM terminology)
 Note that while GEDCOM_FORM is required, but for us it will always be Lineage-Linked
 */
typedef struct {
    //Header source - i.e. software that produced the GEDCOM file
    char        source[249];
  
    //GEDCOM version
    float       gedcVersion;
    
    //Encoding.  We use an enum, since there are only 4 possible values.
    CharSet     encoding;
    
    //Reference to the submitter record
    Submitter*  submitter;
    
    //All other header fields. All objects in the list will be of type Field.  It may be empty.
    List        otherFields;
    
} Header;

//Represends GEDCOM individual record
typedef struct {
    
    //Set to empty string if not present in file
    char*    givenName;
    
    //Set to empty string if not present in file
    char*    surname;
    
    //Collection of events. All objects in the list will be of type Event.  It may be empty.
    List    events;
    
    //Collection of family references.  All objects in the list will be of type Family.  It may be empty.
    List    families;
    
    //All other individual record fields. All objects in the list will be of type Field.  It may be empty.
    List    otherFields;
    
} Individual;

//Represends GEDCOM family record
typedef struct {
    //Wife reference (can be null)
    Individual* wife;
    
    //Husband reference (can be null)
    Individual* husband;
    
    //List of child references.  All objects in the list will be of type Individual.  It may be empty.
    List        children;
    
    //List of other fields in the family record.  All objects in the list will be of type Field.  It may be empty.
    List        otherFields;
    
} Family;

//Represents a GEDCOM object
typedef struct {

	//Header.  Must not be NULL.
    Header*     header;
    
    //Family records.  All objects in the list will be of type Family.  It may be empty.
    List         families; //Must contain type
    
    //Individual records.  All objects in the list will be of type Individual.  It may be empty.
    List        individuals; //Must contain type Family
    
    //Submitter.  Must not be NULL.
    Submitter*  submitter;
    
    //All other records should be ignored for now
 
} GEDCOMobject;

//Error type
typedef struct {
    
    ErrorCode   type;
    int         line;
    
} GEDCOMerror;


//***************************************** GEDCOOM object functions *****************************************

/** Function to create a GEDCOM object based on the contents of an GEDCOM file.
 *@pre File name cannot be an empty string or NULL.  File name must have the .ged extension.
 File represented by this name must exist and must be readable.
 *@post Either:
 A valid GEDCOM has been created, its address was stored in the variable obj, and OK was returned
 or
 An error occurred, the GEDCOM was not created, all temporary memory was freed, obj was set to NULL, and the
 appropriate error code was returned
 *@return the error code indicating success or the error encountered when parsing the GEDCOM
 *@param fileName - a string containing the name of the GEDCOM file
 *@param a double pointer to a GEDCOMobject struct that needs to be allocated
 **/
GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj);


/** Function to create a string representation of a GEDCOMobject.
 *@pre GEDCOMobject object exists, is not null, and is valid
 *@post GEDCOMobject has not been modified in any way, and a string representing the GEDCOM contents has been created
 *@return a string contaning a humanly readable representation of a GEDCOMobject
 *@param obj - a pointer to a GEDCOMobject struct
 **/
char* printGEDCOM(const GEDCOMobject* obj);


/** Function to delete all GEDCOM object content and free all the memory.
 *@pre GEDCOM object exists, is not null, and has not been freed
 *@post GEDCOM object had been freed
 *@return none
 *@param obj - a pointer to a GEDCOMobject struct
 **/
void deleteGEDCOM(GEDCOMobject* obj);


/** Function to "convert" the GEDCOMerror into a humanly redabale string.
 *@return a string contaning a humanly readable representation of the error code
 *@param err - an error struct
 **/
char* printError(GEDCOMerror err);

/** Function that searches for an individual in the list using a comparator function.
 * If an individual is found, a pointer to the Individual record
 * Returns NULL if the individual is not found.
 *@pre GEDCOM object exists,is not NULL, and is valid.  Comparator function has been provided.
 *@post GEDCOM object remains unchanged.
 *@return The Individual record associated with the person that matches the search criteria.  If the Individual record is not found, return NULL.
 *If multiple records match the search criteria, return the first one.
 *@param familyRecord - a pointer to a GEDCOMobject struct
 *@param compare - a pointer to comparator fuction for customizing the search
 *@param person - a pointer to search data, which contains seach criteria
 *Note: while the arguments of compare() and person are all void, it is assumed that records they point to are
 *      all of the same type - just like arguments to the compare() function in the List struct
 **/
Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person);


/** Function to return a list of all descendants of an individual in a GEDCOM
 *@pre GEDCOM object exists, is not null, and is valid
 *@post GEDCOM object has not been modified in any way, and a list of descendants has been created
 *@return a list of descendants.  The list may be empty.  All list members must be of type Individual, and can appear in any order.
 *All list members must be COPIES of the Individual records in the GEDCOM file.  If the returned list is freed, the original GEDCOM
 *must remain unaffected.
 *@param familyRecord - a pointer to a GEDCOMobject struct
 *@param person - the Individual record whose descendants we want
 **/
List getDescendants(const GEDCOMobject* familyRecord, const Individual* person);


//************************************************************************************************************

//****************************************** List helper functions *******************************************
void deleteEvent(void* toBeDeleted);
int compareEvents(const void* first,const void* second);
char* printEvent(void* toBePrinted);

void deleteIndividual(void* toBeDeleted);
int compareIndividuals(const void* first,const void* second);
char* printIndividual(void* toBePrinted);

void deleteFamily(void* toBeDeleted);
int compareFamilies(const void* first,const void* second);
char* printFamily(void* toBePrinted);

void deleteField(void* toBeDeleted);
int compareFields(const void* first,const void* second);
char* printField(void* toBePrinted);
//************************************************************************************************************

//**************************************** Additional utility functions ***************************************
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
GEDCOMLine* convertStringToGEDCOMLine(char* toConvert);

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
//************************************************************************************************************
#endif
