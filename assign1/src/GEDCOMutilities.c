
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
	if(toFree->date!=NULL)free(toFree->date);
	if(toFree->place!=NULL)free(toFree->place);	
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
	
	
	
	if(validateLevel(token, level)){
		level = atoi(token);	
		line->level = level;		
	}else{

		line->level = -1;		
	}
		
	token = strtok(NULL," \r\n");
	
	
	if(token==NULL){
		return(line);
	}
	
	

	if(token[0]=='@'){
		
		
	
		if(validateReferenceID(token)){
			line->ref_ID = malloc(sizeof(char)*22);
			strcpy(line->ref_ID, token);		
		}
		 
		token = strtok(NULL,"\r\n");
	
		if(validateTag(token)){
			line->tag = malloc(sizeof(char)*31);
			for(i = 0; i<strlen(token);i++){
				toupper(token[i]);
			}
			strcpy(line->tag, token);	

		}
			
	}else{		
		
		if(validateTag(token)){
			for(i = 0; i<strlen(token);i++){
				toupper(token[i]);
			}
			line->tag = malloc(sizeof(char)*31);
			strcpy(line->tag, token);
		}
		
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


Header* createHeader(GEDCOMLine ** record, int numLines){
	
	Header* header = malloc(sizeof(Header));
	header->submitter = NULL;
	
	int i = 0;
	int j = 0;
	float version =  0;	
	bool sourFound = false;
	bool versionFound = false;
	bool charsetFound = false;
	bool submitterFound = false;
	
	header->otherFields = initializeList(printField, deleteField, compareFields);
	

	for(i = 0; i< numLines; i++){

		if((record[i]->level == -1 )||(record[i]->tag == NULL)){
			printf("Invalid level or tag\n");
			clearList(&(header->otherFields));
			strcpy(header->source, "ERROR");
			header->gedcVersion = record[i]->lineNum;	
			return header;
						
		}else if(strcmp(record[i]->tag, "SOUR")==0){
			sourFound = true;
			if(record[i]->value == NULL){
				printf("Invalid source value\n");				
				clearList(&(header->otherFields));
				strcpy(header->source, "ERROR");
				header->gedcVersion = record[i]->lineNum;	
				return header;	
			
			}else{			

				strcpy(header->source, record[i]->value);		

			}
	
		}else if((strcmp(record[i]->tag, "VERS")==0) ){
			
			if(!(i==0 || i==1)){
				if((strcmp(record[i-1]->tag, "GEDC")==0 ||strcmp(record[i-2]->tag, "GEDC")==0)){
					versionFound = true;
				
					if(record[i]->value == NULL){
						printf("Invalid gedcom version\n");					
						clearList(&(header->otherFields));
						strcpy(header->source, "ERROR");
						header->gedcVersion = record[i]->lineNum;	
						return header;							
					}else{
					
						header->gedcVersion = atof(record[i]->value); 				
					}
				}
			}
		}else if(strcmp(record[i]->tag, "CHAR")==0){
			charsetFound = true;
			if(record[i]->value == NULL){			
				printf("Invalid charset\n");		
				clearList(&(header->otherFields));
				strcpy(header->source, "ERROR");
				header->gedcVersion = record[i]->lineNum;	
				return header;						
			}else{
				header->encoding = decodeCharSet(record[i]->value);
				if (header->encoding == -1){			
					printf("Invalid encoding\n");
					clearList(&(header->otherFields));
					strcpy(header->source, "ERROR");
					header->gedcVersion = record[i]->lineNum;	
					return header;	
				}	
			}				
		}else if(strcmp(record[i]->tag, "SUBM")==0){
			submitterFound = true;
			if(record[i]->value == NULL){			
				printf("Invalid submitter value\n");
				clearList(&(header->otherFields));
				strcpy(header->source, "ERROR");
				header->gedcVersion = record[i]->lineNum;	
				return header;					
				
			}
		}else if(!(strcmp(record[i]->tag,"CONC")==0)||!(strcmp(record[i]->tag,"CONT")==0)){
			
			insertSorted(&(header->otherFields), createField(record[i]));
			
		}
		
	}
		
	/**Node * ptr;
	ptr = (header->otherFields).head;
	char* tempString;
	printf("Other Fields\n");
	while(ptr!=NULL){
		tempString = printField(ptr->data);
		printf("%s\n",tempString);
		free(tempString);
		ptr = ptr->next;
	}
	**/
	
	if(sourFound == false || versionFound == false || charsetFound == false || submitterFound == false){
		
		printf("Invalid header\n");
		clearList(&(header->otherFields));
		strcpy(header->source, "ERROR");
		header->gedcVersion = -1;	
		return header;					
		
	}
	return header;
		
}

void appender(GEDCOMLine ** record, int numLines){
	int destLength = 0;
	int sourceLength = 0;
	int i = 0;
	
	for(i = (numLines-1); i> 0; i--){
		if(strcmp(record[i]->tag, "CONC")==0){
			
			destLength = strlen(record[i-1]->value);
			sourceLength = strlen(record[i]->value);														
			record[i-1]->value = realloc(record[i-1]->value, sizeof(char) * (destLength + sourceLength+2));		
			strcat(record[i-1]->value, " ");			
			strcat(record[i-1]->value, record[i]->value);
			destLength = 0;
			sourceLength = 0;
				
		}else if(strcmp(record[i]->tag, "CONT")==0){
						
			destLength = strlen(record[i-1]->value);
			sourceLength = strlen(record[i]->value);				
			record[i-1]->value = realloc(record[i-1]->value, sizeof(char) * (destLength + sourceLength+2));
			strcat(record[i-1]->value, "\n");			
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



Field *createField(GEDCOMLine* line){
	Field *newField = malloc(sizeof(Field));
	newField->tag = NULL;
	newField->value = NULL;
	
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
Submitter* createSubmitter(GEDCOMLine ** record, int numLines){

	Submitter* submitter = malloc(sizeof(Submitter) + 5 * sizeof(char));
	strcpy(submitter->submitterName,"");
	strcpy(submitter->address,"");
		
	int i = 0;
	int j = 0;
	bool nameFound = false;
	submitter->otherFields = initializeList(printField, deleteField, compareFields);
	

	for(i = 0; i< numLines; i++){
		
		
		if((record[i]->level == -1 )||(record[i]->tag == NULL)){
			printf("Invalid level or tag\n");
			clearList(&(submitter->otherFields));
			sprintf(submitter->submitterName, "ERROR");
			return submitter;
						
		}else if(strcmp(record[i]->tag, "NAME")==0){

			if(record[i]->value == NULL || strlen(record[i]->value) > 60){
				printf("Invalid name value\n");				
				clearList(&(submitter->otherFields));
				sprintf(submitter->submitterName, "ERROR");
				sprintf(submitter->address, "%d", record[i]->lineNum);
				return submitter;	
			
			}else{			
				nameFound = true;
				strcpy(submitter->submitterName, record[i]->value);		

			}
	
		}else if((strcmp(record[i]->tag, "ADDR")==0) ){
							
			if(record[i]->value == NULL){
				
				printf("Invalid address value\n");					
				clearList(&(submitter->otherFields));
				sprintf(submitter->submitterName, "ERROR");
				sprintf(submitter->address, "%d", record[i]->lineNum);
				return submitter;	
										
			}else{
					
				submitter = realloc(submitter, sizeof(Submitter) + sizeof(char) * (strlen(record[i]->value)+1));
				strcpy(submitter->address, record[i]->value);
				//printf("Submitter Address: %s\n", submitter->address);
						
			}
				
			
		}else if(!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0)){
			
			insertSorted(&(submitter->otherFields), createField(record[i]));
			
		}	
	
	}
	/**printf("Submitter");
	Node * ptr;
	ptr = (submitter->otherFields).head;
	char* tempString;
	printf("Other Fields\n");
	
	while(ptr!=NULL){
		tempString = printField(ptr->data);
		printf("%s\n",tempString);
		free(tempString);
		ptr = ptr->next;
	}
	**/

	return submitter;
		
}

char * getLine(char *destination, int maxLength, FILE *fp){
	int character;
	char *ptr;

	for (ptr = destination, maxLength--; maxLength > 0; maxLength--) {

		if ((character = fgetc (fp)) == EOF)break;		
		*ptr++ = character;
		if (character== '\r' )break;		
		if (character == '\n' )break;
	}
	*ptr = 0;
	if (ptr == destination || character == EOF){
		return NULL;
	}
	return (ptr);
}
Individual* createIndividual(GEDCOMLine ** record, int numLines){

	Individual* individual = malloc(sizeof(Individual));
	GEDCOMLine ** currentEvent = malloc(sizeof(GEDCOMLine*));
	
	individual->givenName = malloc(sizeof(char)*120);
	individual->surname = malloc(sizeof(char)*120);	
	strcpy(individual->givenName, "");
	strcpy(individual->surname, "");
	individual->events = initializeList(printEvent, deleteEvent, compareEvents);
	individual->otherFields = initializeList(printField, deleteField, compareFields);	
	
	Event * tempEvent;		
	char* token;	
	int i = 0;
	int j = 0;
	int k = 0;
	int eventCounter= 0;
	
	bool nameFound = false;

	for(i = 0; i< numLines; i++){
								
		
		if((record[i]->level == -1 )||(record[i]->tag == NULL)){
			
			printf("Invalid level or tag\n");
			clearList(&(individual->otherFields));
			clearList(&(individual->events));	
			sprintf(individual->givenName, "ERROR");
			sprintf(individual->surname, "%d", record[i]);			
			return individual;
								
		}else if(strcmp(record[i]->tag, "NAME")==0){

			if(record[i]->value == NULL ){
				strcpy(individual->givenName, "");				
				strcpy(individual->surname, "");				
			
			}else{							
				token = strtok(record[i]->value,"/");	
				if(token!=NULL){
					strcpy(individual->givenName, token);	
				}										
				strcpy(individual->givenName, token);		
				token = strtok(NULL, "/");
				if(token!=NULL){
					strcpy(individual->surname, token);	
				}
			}
	
		}else if(record[i]->level == 1 && record[i]->value == NULL && !(strcmp(record[i]->tag, "SEX")==0) ){
			k = i + 1;
			
			while(record[k]->level==2){
				eventCounter++;
				currentEvent = realloc(currentEvent, sizeof(GEDCOMLine*) * (eventCounter+1));
				memcpy(&currentEvent[eventCounter-1], &record[k], sizeof(GEDCOMLine*));
							
				k++;
			}
			eventCounter = 0;
			tempEvent = createEvent(currentEvent, eventCounter);
			strcpy(tempEvent->type, record[i]->tag);
			if(strcmp(tempEvent->date, "ERROR")==0){
										
				printf("Type: %s Date: %s Place: %s\n", tempEvent->type, tempEvent->date, tempEvent->place);	
				
				printf("Invalid Event\n");
				clearList(&(individual->otherFields));
				clearList(&(individual->events));
					
				sprintf(individual->givenName, "ERROR");
				sprintf(individual->surname, "%d", atoi(tempEvent->place));												
				free(tempEvent->date);
				free(tempEvent->place);
				clearList(&(tempEvent->otherFields));
				free(tempEvent);
				return individual;				
			}else{
										
				//insertSorted(&(individual->events),tempEvent);
				//tempEvent = NULL;								
			}		
		}/**else if(!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0)){
			
			insertSorted(&(submitter->otherFields), createField(record[i]));
			
		}**/	
	
	}
	/**printf("Submitter");
	Node * ptr;
	ptr = (submitter->otherFields).head;
	char* tempString;
	printf("Other Fields\n");
	
	while(ptr!=NULL){
		tempString = printField(ptr->data);
		printf("%s\n",tempString);
		free(tempString);
		ptr = ptr->next;
	}
	**/
	free(currentEvent);
	return individual;
		
}
Event* createEvent(GEDCOMLine ** record, int numLines){
	
	Event* event = malloc(sizeof(Event));
	
	event->date = malloc(sizeof(char)*120);
	event->place = malloc(sizeof(char)*120);	
	strcpy(event->date,"");
	strcpy(event->place,"");
	strcpy(event->type," ");
				
	int i = 0;
	int j = 0;

	event->otherFields = initializeList(printField, deleteField, compareFields);
	
	
	for(i = 0; i< numLines; i++){

		
		if((record[i]->level == -1 )||(record[i]->tag == NULL)){
			printf("Invalid level or tag\n");
			clearList(&(event->otherFields));
			sprintf(event->date, "ERROR");
			sprintf(event->place, "%d", record[i]->lineNum);			
			return event;
						
		}/**else if(strcmp(record[i]->tag, "NAME")==0){

			if(record[i]->value == NULL || strlen(record[i]->value) > 60){
				printf("Invalid name value\n");				
				clearList(&(submitter->otherFields));
				sprintf(submitter->submitterName, "ERROR");
				sprintf(submitter->address, "%d", record[i]->lineNum);
				return submitter;	
			
			}else{			
				nameFound = true;
				strcpy(submitter->submitterName, record[i]->value);		

			}
	
		}else if((strcmp(record[i]->tag, "ADDR")==0) ){
							
			if(record[i]->value == NULL){
				
				printf("Invalid address value\n");					
				clearList(&(submitter->otherFields));
				sprintf(submitter->submitterName, "ERROR");
				sprintf(submitter->address, "%d", record[i]->lineNum);
				return submitter;	
										
			}else{
					
				submitter = realloc(submitter, sizeof(Submitter) + sizeof(char) * (strlen(record[i]->value)+1));
				strcpy(submitter->address, record[i]->value);
				//printf("Submitter Address: %s\n", submitter->address);
						
			}
				
			
		}else if(!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0)){
			
			insertSorted(&(submitter->otherFields), createField(record[i]));
			
		}	
	**/
	}


	//free(event->date);
	//free(event->place);	
	//free(event);

	/**printf("Submitter");
	Node * ptr;
	ptr = (submitter->otherFields).head;
	char* tempString;
	printf("Other Fields\n");
	
	while(ptr!=NULL){
		tempString = printField(ptr->data);
		printf("%s\n",tempString);
		free(tempString);
		ptr = ptr->next;
	}
	**/

	return event;
	
	
	
	
}
