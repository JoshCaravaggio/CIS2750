
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"

//***************************************** GEDCOM object functions *****************************************

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
GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj){
	
	//Need to do error checking for filename
	FILE * fp = fopen(fileName, "r");
	
	
	char * tempLine = malloc(sizeof(char)*256);
	GEDCOMerror error;
	GEDCOMLine ** fileLines = malloc(sizeof(GEDCOMLine*));
	int currenLevel = 0;
	char* token;
	int i;
	int lineCounter = 0;
	int recordCounter = 0;
	bool newRecord = false;
	bool headFound = false;
	bool trlrFound = false;		
	bool errorFound = false;		
		

	while(fgets(tempLine,256,fp)!=NULL){
	
		if(strcmp(tempLine,"\n")==0){
			error.line = lineCounter;
			error.type = INV_RECORD;
			
			for(i = 0; i<lineCounter; i++){		
				deleteGEDCOMLine(fileLines[i]);
			}			
			
			free(fileLines);
			free(tempLine);
			fclose(fp);
			return error;			
		}		

		if((!(tempLine[strlen(tempLine)-1] == '\n'))&&!(tempLine[strlen(tempLine)-1] =='\r')){
			errorFound = true;	
		}
		
		fileLines = realloc(fileLines, sizeof(GEDCOMLine*) * (lineCounter + 1));
		fileLines[lineCounter] = convertStringToGEDCOMLine(tempLine);
		lineCounter++;
		
		if(strcmp(fileLines[lineCounter-1]->tag,"HEAD")==0){
			headFound = true;
		}
		
		if(strcmp(fileLines[lineCounter-1]->tag,"TRLR")==0){
			trlrFound = true;
		}			
		
	}
	
	if((headFound == false )|| (trlrFound ==false)){
		error.line = -1;
		error.type = INV_GEDCOM;
		
		for(i = 0; i<lineCounter; i++){		
			deleteGEDCOMLine(fileLines[i]);
		}			
			
		free(fileLines);
		free(tempLine);
		fclose(fp);
		return error;		
		
	}
	
	
	error.line = -1;
	error.type = OK;

	for(i = 0; i<lineCounter; i++){
		printf("Line %d ---", i);
		printf("Level: %d ", fileLines[i]->level);
		printf("XRef ID: %s ", fileLines[i]->ref_ID);		
		printf("Tag: %s ", fileLines[i]->tag);		
		printf("Value: %s\n", fileLines[i]->value);
		deleteGEDCOMLine(fileLines[i]);
	}
	
	free(fileLines);
	free(tempLine);		
	fclose(fp);
	return error;
		
}


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


//***********************************************************************************************************

//****************************************** List helper functions *******************************************
void deleteEvent(void* toBeDeleted){
	Event* toDelete = (Event*)toBeDeleted;
	if(toDelete==NULL){
		return;
	}else{
		freeEvent(toDelete);
		return;
	}
	
}
int compareEvents(const void* first,const void* second){
	
	Event* event1 = (Event*)first;
	Event* event2 = (Event*)second;	

	if(event1==NULL ||event2==NULL ){
		return OTHER;
	}
	
	if(event1->date==NULL ||event2->date==NULL ){
		return OTHER;
	}
	
	if(event1->place==NULL ||event2->place==NULL ){
		return OTHER;
	}	

	char* formattedDate1 = NULL;
	char* formattedDate2 = NULL;
		
	
	formattedDate1 = formatDate(event1->date);
	formattedDate2 = formatDate(event2->date);
	int comparison = strcmp(formattedDate1,formattedDate2);
	free(formattedDate1);
	free(formattedDate2);	
	return comparison;

}

char* printEvent(void* toBePrinted){
	
	Event* event = (Event*)toBePrinted;
	
	if(event == NULL){
		return NULL;
	}

	char* eventInfo = malloc(sizeof(char)*240);	
	strcpy(eventInfo,"Type: ");
	strcat(eventInfo, event->type);	
	strcat(eventInfo,"\nDate: ");
	strcat(eventInfo, event->date);	
	strcat(eventInfo,"\nPlace: ");
	strcat(eventInfo, event->place);	
	strcat(eventInfo,"\n");
	return eventInfo;	
}


void deleteIndividual(void* toBeDeleted){
	
	Individual* toDelete = (Individual*)toBeDeleted;
	
	if(toDelete==NULL){
		return; 
	}else{
		freeIndividual(toDelete);
		return;
	}
}
int compareIndividuals(const void* first,const void* second){
	
	Individual* individual1 = (Individual*)first;
	Individual* individual2 = (Individual*)second;	

	if(individual1==NULL ||individual2==NULL ){
		return OTHER;
	}
	
	if(individual1->surname==NULL ||individual2->surname==NULL ){
		return OTHER;
	}
	
	if(individual1->givenName==NULL ||individual2->givenName==NULL ){
		return OTHER;
	}
		
	char* individual1Info = malloc(sizeof(char)*240);
	char* individual2Info = malloc(sizeof(char)*240);	
	strcpy(individual1Info, individual1->surname);
	strcat(individual1Info,",");
	strcat(individual1Info,individual1->givenName);
	strcpy(individual2Info, individual2->surname);
	strcat(individual2Info,",");
	strcat(individual2Info,individual2->givenName);
	int comparison = strcmp(individual1Info,individual2Info);
	free(individual1Info);
	free(individual2Info);
	return comparison;		
	
	
}
char* printIndividual(void* toBePrinted){

	Individual* individual = (Individual*)(toBePrinted);
	char* individualInfo = malloc(sizeof(char)*240);	
	strcpy(individualInfo, individual->surname);
	strcat(individualInfo,",");
	strcat(individualInfo,individual->givenName);
	strcat(individualInfo,"\n");	
	return individualInfo;	
	
}

void deleteFamily(void* toBeDeleted){
	
	Family* toDelete = (Family*)toBeDeleted;
	
	
	if(toDelete==NULL){
		return; 
	}else{
		freeFamily(toDelete);
		return;
	}
	
}
int compareFamilies(const void* first,const void* second){
	
	Family* family1 = (Family*)first;	
	Family* family2 = (Family*)second;
	
	if(family1==NULL || family2 == NULL){
		return OTHER;	
	}
	
	if(family1->wife==NULL || family2->wife == NULL){
		return OTHER;	
	}
	
	if(family1->husband==NULL || family2->husband == NULL){
		return OTHER;	
	}		
	
	int fam1Children = family1->children.length;
	int fam2Children = family2->children.length;	

	if(fam1Children < fam2Children){
		return -1;
	}else if(fam1Children == fam2Children){
		return 0;
	}else{
		return 1;	
	}
	
}
char* printFamily(void* toBePrinted){
	
	Family* family = (Family*)(toBePrinted);
	char* familyInfo = malloc(sizeof(char)*280);	
	strcpy(familyInfo,"Wife: ");
	strcat(familyInfo,family->wife->surname);	
	strcat(familyInfo,",");
	strcat(familyInfo,family->wife->givenName);		
	strcat(familyInfo,"\nHusband: ");
	strcat(familyInfo,family->husband->surname);	
	strcat(familyInfo,",");
	strcat(familyInfo,family->husband->givenName);		
	strcat(familyInfo,"\n");
	return familyInfo;
	
}


void deleteField(void* toBeDeleted){
	
	Field* toDelete = (Field*)(toBeDeleted);
	
	if(toDelete== NULL){
		return; 
	}else{
		freeField(toDelete);
		return;
	}
	
}
int compareFields(const void* first,const void* second){
	
	Field* field1 = (Field*)(first);
	Field* field2 = (Field*)(second);
	

	if(field1 == NULL || field2 == NULL){
		return OTHER;
	}
	if(field1->tag == NULL || field2->tag == NULL){
		return OTHER;
	}
	if(field1->value == NULL || field2->value == NULL){
		return OTHER;
	}		
	
	char* field1Info = malloc(sizeof(char)*240);
	char* field2Info = malloc(sizeof(char)*240);	
	strcpy(field1Info, field1->tag);
	strcat(field1Info," ");
	strcat(field1Info,field1->value);
	strcpy(field2Info, field2->tag);
	strcat(field2Info," ");
	strcat(field2Info,field2->value);
	int comparison = strcmp(field1Info,field2Info);
	free(field1Info);
	free(field2Info);
	return comparison;		
	
}

char* printField(void* toBePrinted){
	Field* field = (Field*)(toBePrinted);
	char* fieldInfo = malloc(sizeof(char)*243);	
	//Find out what to do if value or tag is NULL -> impossible by design?
	
	strcpy(fieldInfo, field->tag);
	strcat(fieldInfo," : ");
	strcat(fieldInfo,field->value);
	strcat(fieldInfo,"\n");
	return fieldInfo;
	
}
//************************************************************************************************************


/** Function to free a field 
 *@pre Field must exist
 *@post Memory allocated to field member and field are freed
 *@return none
 *@param toBeFreed - pointer to the field data we want to free
 **/
void freeField(void* toBeFreed){
	//Find out what to do if the tag or value is NULL
	Field* toFree = (Field*)(toBeFreed);
	free(toFree->tag);
	free(toFree->value);
	free(toFree);
	return;
}
/** Function to free an Individual
 *@pre Individual must exist
 *@post Memory allocated to individual member and individual are freed
 *@return none
 *@param toBeFreed - pointer to the inidividual data we want to free
 **/
void freeIndividual(void* toBeFreed){
	//Find out what to do if the tag or value is NULL
	Individual* toFree = (Individual*)(toBeFreed);
	free(toFree->surname);
	free(toFree->givenName);
	free(toFree);
	return;
}
/** Function to free a Family
 *@pre Family must exist
 *@post Memory allocated to family is freed
 *@return none
 *@param toBeFreed - pointer to the family data we want to free
 **/
void freeFamily(void* toBeFreed){
	//Find out what to do if the tag or value is NULL
	Family* toFree = (Family*)(toBeFreed);
	toFree->wife = NULL;
	toFree->husband = NULL;
	free(toFree);
	return;
}
/** Function to free an Event
 *@pre Event must exist
 *@post Memory allocated to event is freed
 *@return none
 *@param toBeFreed - pointer to the event data we want to free
 **/
void freeEvent(void* toBeFreed){
	//Find out what to do if the tag or value is NULL
	Event* toFree = (Event*)(toBeFreed);
	free(toFree->date);
	free(toFree->place);
	free(toFree);
	return;
}
/** Function to format a stirng date into on that is comparable with strcmp
 *@pre date string must exist
 *@post return a string represnting the date in numbers only
 *@return string of date
 *@param toBeFormated - pointer to the date data we want to format
 **/
char* formatDate(char* toBeFormated){

	char* formattedDate = malloc(sizeof(char)*12);
	//char month = malloc(sizeof(char)*9);
	
	const char delim[2] = " ";
	char* token;
	
	token = strtok(toBeFormated,delim);
	strcpy(formattedDate, token);
	
	token = strtok(NULL,delim);

	if(strcmp(token,"JAN")==0){
		
		strcat(formattedDate,"01");	
			
	}else if(strcmp(token,"FEB")==0){
		
		strcat(formattedDate,"02");
		
	}else if(strcmp(token,"MAR")==0){

		strcat(formattedDate,"03");
		
	}else if(strcmp(token,"APR")==0){

		strcat(formattedDate,"04");
		
	}else if(strcmp(token,"MAY")==0){
		
		strcat(formattedDate,"05");		
		
	}else if(strcmp(token,"JUN")==0){
		
		strcat(formattedDate,"06");		
		
	}else if(strcmp(token,"JUL")==0){
		
		strcat(formattedDate,"07");		
		
	}else if(strcmp(token,"AUG")==0){
		
		strcat(formattedDate,"08");
				
	}else if(strcmp(token,"SEP")==0){

		strcat(formattedDate,"09");		
		
	}else if(strcmp(token,"OCT")==0){
		
		strcat(formattedDate,"10");		
		
	}else if(strcmp(token,"NOV")==0){
		
		strcat(formattedDate,"11");		
		
	}else if(strcmp(token,"DEC")==0){
		strcat(formattedDate,"12");		
	}
	token = strtok(NULL,delim);
	strcat(formattedDate,token);
	
	return formattedDate;
}

/** Function to convert a string to a GEDCOMLine structure
 *@pre line must be read from file
 *@post return a GEDCOMLine structure ready for processing 
 *@return GEDCOMLine
 *@param string of data from file
 **/
GEDCOMLine* convertStringToGEDCOMLine(char* toConvert){
	
	int i = 0;
	char* token;
	int level = 0;
	
	GEDCOMLine* line = malloc(sizeof(GEDCOMLine));
	line->tag = NULL;
	line->ref_ID = NULL;
	line->value = NULL;
	
	token = strtok(toConvert," \r\n");
	
	
	//Make sure the level token is valid
	if(validateLevel(token, level)){
		level = atoi(token);	
		line->level = level;		
	}else{

		line->level = -1;		
	}
		
	token = strtok(NULL," \r\n");
	
	//Ensure there is another token after the level		
	if(token==NULL){
		return(line);
	}
	
	
	//Course of action for LEVEL-REF_ID-TAG line format
	if(token[0]=='@'){
		
		
		//Ensuring reference ID is valid
		if(validateReferenceID(token)){
			line->ref_ID = malloc(sizeof(char)*22);
			strcpy(line->ref_ID, token);		
		}
		 
		token = strtok(NULL,"\r\n");
		//Ensuring tag is valid
		if(validateTag(token)){
			line->tag = malloc(sizeof(char)*31);
			for(i = 0; i<strlen(token);i++){
				toupper(token[i]);
			}
			strcpy(line->tag, token);	

		}
			
	// Course of aciton for LEVEL-TAG-Value line format	
	}else{		
		//Ensuring tag is valid		
		if(validateTag(token)){
			for(i = 0; i<strlen(token);i++){
				toupper(token[i]);
			}
			line->tag = malloc(sizeof(char)*31);
			strcpy(line->tag, token);
		}
		
		//Getting remainder of line for valid, whether empty or not
		token = strtok(NULL,"\r\n");
		if(token != NULL){
			line->value = malloc(sizeof(char)*255);
			strcpy(line->value, token);	
		}	

	}
	return line;
}

/** Function to check validity of a string-tokenized level 
 *@pre lstring must be non-null
 *@post return true or false based on the level's validity
 *@return boolean value
 *@param string token of level
 *@param level of previous GEDCOM line
 **/
bool validateLevel(char* toValidate, int previousLevel){
	int currentLevel;
	int i  = 0;
	if(strlen(toValidate) > 2){
		return false;
	}
	for(i = 0; i< strlen(toValidate); i++){
		if(i == 0){
			if(strlen(toValidate)==2){
				if (((int)(toValidate[i]) < 49)||((int)(toValidate[i]))>57){
					return false;
				}				
			}else{
				if (((int)(toValidate[i]) < 48)||((int)(toValidate[i]))>57){
					return false;
				}				
			}
		}else{
			if (((int)(toValidate[i]) < 48)||((int)(toValidate[i]))>57){
				return false;	
			}		
		}
	}	
	currentLevel = atoi(toValidate);

	return true;
		
}

/** Function to check validity of a string-tokenized refernce ID 
 *@pre string must be non-null
 *@post return true or false based on the reference ID's validity
 *@return boolean value
 *@param string token of reference ID
 **/
bool validateReferenceID(char* toValidate){

	int i  = 0;
	if(strlen(toValidate)<3){
		return false;
	}
	if(toValidate[strlen(toValidate)-1] != '@'){
		return false;
	}else{
		if(strlen(toValidate)>22){
			return false;							
		}else{
			return true;

		}			
	}
		
}
/** Function to check validity of a string-tokenized GEDCOM tag
 *@pre string must be non-null
 *@post return true or false based on the tag's validity
 *@return boolean value
 *@param string token of reference ID
 **/
bool validateTag(char* toValidate){
	if(toValidate == NULL){
		return false;	
	}
	if((strlen(toValidate)>31)){
		return false;							
	}else{
		return true;
	}
	
}
/** Function to delete GEDCOM Line
 *@pre GEDCOM line must exist
 *@post free a gedcom line and all it's members
 *@return none
 *@param void pointer to gedcom line data 
 **/
void deleteGEDCOMLine(void * toBeDeleted){

	GEDCOMLine * toDelete = (GEDCOMLine*)toBeDeleted;
	if(toDelete == NULL){
		return;
	}
	if((toDelete->ref_ID) != NULL){
		free(toDelete->ref_ID);
	}
	
	if((toDelete->value) != NULL){
		free(toDelete->value);
	}
	
	if((toDelete->tag) != NULL){
		free(toDelete->tag);
	}	
	free(toDelete);
	return;
	
}

