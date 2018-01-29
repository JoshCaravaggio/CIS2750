
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"


void freeField(void* toBeFreed){
	//Find out what to do if the tag or value is NULL
	Field* toFree = (Field*)(toBeFreed);
	if(toFree->tag!=NULL){
		free(toFree->tag);
	}
	if(toFree->value!=NULL){
		free(toFree->value);
	}
	if(toFree!=NULL){
		free(toFree);
	}
	return;
}

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

void freeEvent(void* toBeFreed){
	//Find out what to do if the tag or value is NULL
	Event* toFree = (Event*)(toBeFreed);
	free(toFree->date);
	free(toFree->place);
	free(toFree);
	return;
}

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

GEDCOMLine* convertStringToGEDCOMLine(char* toConvert, int lineNum){
	
	int i = 0;
	char* token;
	int level = 0;
	
	GEDCOMLine* line = malloc(sizeof(GEDCOMLine));
	line->lineNum = lineNum;
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


GEDCOMLine* copyGEDCOMLine(void * toBeCopied){

	GEDCOMLine * source = (GEDCOMLine*)toBeCopied;
	GEDCOMLine* copy = malloc(sizeof(GEDCOMLine));
	copy->tag = NULL;
	copy->ref_ID = NULL;
	copy->value = NULL;
	
	
	if(source == NULL){
		return NULL;
	}
	copy->level = source->level;
	
	if(source->ref_ID!=NULL){
		copy->ref_ID = malloc(sizeof(char)*22);
		strcpy(copy->ref_ID,source->ref_ID);
		
	}
	if(source->tag!=NULL){
		copy->tag = malloc(sizeof(char)*31);
		strcpy(copy->tag,source->tag);
		
	}
	if(source->value!=NULL){
		copy->value = malloc(sizeof(char)*255);
		strcpy(copy->value,source->value);
	}		
	return copy;
	
}

Header* createHeader(GEDCOMLine ** record, int numLines){
	
	Header* header = malloc(sizeof(Header));
	int i = 0;
	int j = 0;
	float version =  0;	
	bool sourFound = false;
	bool versionFound = false;
	bool charsetFound = false;
	int errorLine;
	header->otherFields = initializeList(printField, deleteField, compareFields);
	
	
	for(i = 0; i< numLines; i++){
		if(strcmp(record[i]->tag, "SOUR")==0){
			sourFound = true;
			if(record[i]->value == NULL){
				errorLine = record[i]->lineNum;	
			}else{
				
				for(j = 0; j<strlen(record[i]->value);j++){
					header->source[j] = record[i]->value[j];
				}

			}
	
		}else if((strcmp(record[i]->tag, "VERS")==0) && (strcmp(record[i-1]->tag, "GEDC")==0 ||strcmp(record[i-2]->tag, "GEDC")==0)){
				versionFound = true;
				if(record[i]->value == NULL){
					errorLine = record[i]->lineNum;						
				}else{
					header->gedcVersion = atof(record[i]->value); 				
				}
				
		}else if(strcmp(record[i]->tag, "CHAR")==0){
			charsetFound = true;
			if(record[i]->value == NULL){
				errorLine = record[i]->lineNum;						
			}else{
				header->encoding = decodeCharSet(record[i]->value);
				if (header->encoding = -1){
					errorLine = record[i]->lineNum;
				}	
			}				
		}else if(validateHeaderTag(record[i]->tag)){
			insertSorted(&(header->otherFields), createField(record[i]));
						
		}else if(!(strcmp(record[i]->tag,"CONC")==0)||!(strcmp(record[i]->tag,"CONT")==0)){
			errorLine = record[i]->lineNum;
			
		}				
		
	}
	Node * ptr;
	ptr = (header->otherFields).head;
	char* tempString;
	printf("Other Fields\n");
	while(ptr!=NULL){
		tempString = printField(ptr->data);
		printf("%s\n",tempString);
		free(tempString);
		ptr = ptr->next;
	}
	clearList(&(header->otherFields));
	free(header);
	
	return NULL;
		
}

void appender(GEDCOMLine ** record, int numLines){
	int destLength = 0;
	int sourceLength = 0;
	int i = 0;
	
	for(i = (numLines-1); i>= 0; i--){
		if(strcmp(record[i]->tag, "CONC")==0){
			
			destLength = strlen(record[i-1]->value);
			sourceLength = strlen(record[i]->value);						
			record[i-1]->value = realloc(record[i-1]->value, sizeof(char) * 500);
			record[i-1]->value[destLength] = ' ';
			strcat(record[i-1]->value, record[i]->value);
			destLength = 0;
			sourceLength = 0;
				
		}else if(strcmp(record[i]->tag, "CONT")==0){

			destLength = strlen(record[i-1]->value);
			sourceLength = strlen(record[i]->value);						
			record[i-1]->value = realloc(record[i-1]->value, sizeof(char) * 500);
			record[i-1]->value[destLength] = '\n';			
			strcat(record[i-1]->value, record[i]->value);
			destLength = 0;
			sourceLength = 0;
				
				
		}			
	}
		
}

CharSet decodeCharSet(char* toBeConverted){


	if(strcmp(toBeConverted,"ANSEL")==0){
		
		return ANSEL;	
			
	}else if(strcmp(toBeConverted,"UTF-8")==0){
		
		return UTF8;
		
	}else if(strcmp(toBeConverted,"UNICODE")==0){

		return UNICODE;
		
	}else if(strcmp(toBeConverted,"ASCII")==0){

		return ASCII;
		
	}else{
		return -1;
	}

}


bool validateHeaderTag(char* toValidate){
	
	if(strcmp(toValidate,"HEAD")==0){
		return true;	
	}else if(strcmp(toValidate,"SOUR")==0){
		return true;
	}else if(strcmp(toValidate,"NAME")==0){
		return true;
	}else if(strcmp(toValidate,"VERS")==0){
		return true;
	}else if(strcmp(toValidate,"CORP")==0){
		return true;
	}else if(strcmp(toValidate,"DATA")==0){
		return true;
	}else if(strcmp(toValidate,"DATE")==0){
		return true;
	}else if(strcmp(toValidate,"COPR")==0){
		return true;
	}else if(strcmp(toValidate,"DEST")==0){
		return true;
	}else if(strcmp(toValidate,"TIME")==0){
		return true;
	}else if(strcmp(toValidate,"SUBM")==0){
		return true;
	}else if(strcmp(toValidate,"SUBN")==0){
		return true;
	}else if(strcmp(toValidate,"FILE")==0){
		return true;
	}else if(strcmp(toValidate,"GEDC")==0){
		return true;
	}else if(strcmp(toValidate,"FORM")==0){
		return true;
	}else if(strcmp(toValidate,"CHAR")==0){
		return true;
	}else if(strcmp(toValidate,"LANG")==0){
		return true;
	}else if(strcmp(toValidate,"PLAC")==0){
		return true;
	}else if(strcmp(toValidate,"NOTE")==0){
		return true;
	}else{
		return false;
	}
	
}

Field *createField(GEDCOMLine* line){
	Field *newField = malloc(sizeof(Field));
	newField->tag = NULL;
	newField->value = NULL;
	
	//printf("Tag: %s Value: %s\n", line->tag, line->value);
	if(line->tag !=NULL){
		newField->tag = malloc(sizeof(char) * 5);
		strcpy(newField->tag, line->tag);		
	}
	if(line->value!=NULL){
		newField->value = malloc(sizeof(char) * 500);	
		strcpy(newField->value, line->value);
	}
	
	return newField;
}
