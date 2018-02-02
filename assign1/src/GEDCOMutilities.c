
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"


void freeField(void* toBeFreed){
	Field* toFree = (Field*)(toBeFreed);
	if(toFree==NULL){
		return;
	}

	if(toFree->tag!=NULL){
		free(toFree->tag);
	}
	if(toFree->value!=NULL){
		free(toFree->value);
	}
	
	free(toFree);
	
	return;
}

void freeIndividual(void* toBeFreed){

	Individual* toFree = (Individual*)(toBeFreed);
	if(toFree==NULL)return;
	if(toFree->surname!=NULL)free(toFree->surname);
	if(toFree->givenName!=NULL)free(toFree->givenName);
	free(toFree);
	return;
}

void freeFamily(void* toBeFreed){

	Family* toFree = (Family*)(toBeFreed);
	if(toFree==NULL)return;
	if(toFree->wife!=NULL)toFree->wife = NULL;
	if(toFree->husband!=NULL)toFree->husband = NULL;
	free(toFree);
	return;
}

void freeEvent(void* toBeFreed){

	Event* toFree = (Event*)(toBeFreed);
	if(toFree==NULL)return;
	if(toFree->date!=NULL)free(toFree->date);
	if(toFree->place!=NULL)free(toFree->place);	
	free(toFree);
	return;
}

char* formatDate(char* toBeFormatted){

	char* formattedDate = malloc(sizeof(char)*12);
	char* TBFcopy = malloc(sizeof(char)*100);
	strcpy(TBFcopy,toBeFormatted);

	const char delim[2] = " ";
	char* token;
	
	token = strtok(TBFcopy,delim);
	
	
	if(token == NULL){
		free(formattedDate);
		free(TBFcopy);
		return NULL;	
		
	}else{
		strcpy(formattedDate, token);
	}
	
	
	token = strtok(NULL,delim);
	
	if(token == NULL){
		free(formattedDate);
		free(TBFcopy);		
		return NULL;	
		
	}else if(strcmp(token,"JAN")==0){
		
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
	
	if(token == NULL){
		free(TBFcopy);		
		free(formattedDate);
		return NULL;	
		
	}	
	strcat(formattedDate,token);
	free(TBFcopy);
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
		line->level = -2;
		return(line);
	}
	
	

	if(token[0]=='@'){
				
	
		if(validateReferenceID(token)){
			line->ref_ID = malloc(sizeof(char)*22);
			strcpy(line->ref_ID, token);		
		}
		 
		token = strtok(NULL," \r\n");
	
		if(validateTag(token)){
			line->tag = malloc(sizeof(char)*31);
			for(i = 0; i<strlen(token);i++){
				token[i] = toupper(token[i]);
			}
			strcpy(line->tag, token);	

		}
			
	}else{		
		
		if(validateTag(token)){
			
			for(i = 0; i<strlen(token);i++){
				
				token[i] = toupper(token[i]);
				
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
	

	int i  = 0;
	
	if(toValidate == NULL ||strlen(toValidate) > 2){
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
	

	return true;
		
}

bool validateReferenceID(char* toValidate){

	
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
	
	bool sourFound = false;
	bool versionFound = false;
	bool charsetFound = false;
	bool submitterFound = false;
	header->otherFields = initializeList(printField, deleteField, compareFields);
	

	for(i = 0; i< numLines; i++){

		if((record[i]->level == -1 )||(record[i]->tag == NULL)||(i>0 &&(record[i]->level - record[i-1]->level)>1 )){

			strcpy(header->source, "ERROR");
			header->gedcVersion = record[i]->lineNum;	
			return header;
						
		}else if(strcmp(record[i]->tag, "SOUR")==0){
			
			sourFound = true;
			
			if(record[i]->value == NULL){

				strcpy(header->source, "ERROR");
				header->gedcVersion = record[i]->lineNum;	
				return header;	
			
			}else{			

				strcpy(header->source, record[i]->value);		

			}
	
		}else if((strcmp(record[i]->tag, "VERS")==0) ){
			
			if(!(i==0 || i==1)){
				
				if((strcmp(record[i-1]->tag, "GEDC")==0 ||strcmp(record[i-2]->tag, "GEDC")==0)){
					
				
					if(record[i]->value == NULL){

						strcpy(header->source, "ERROR");
						header->gedcVersion = record[i]->lineNum;	
						return header;		
											
					}else{
						
						versionFound = true;
						header->gedcVersion = atof(record[i]->value); 				
					}
				}
			}
		}else if(strcmp(record[i]->tag, "CHAR")==0){
			
			charsetFound = true;
			
			if(record[i]->value == NULL){			

				strcpy(header->source, "ERROR");
				header->gedcVersion = record[i]->lineNum;	
				return header;						
				
			}else{
				
				header->encoding = decodeCharSet(record[i]->value);
				
				if (header->encoding == -1){			

					strcpy(header->source, "ERROR");
					header->gedcVersion = record[i]->lineNum;	
					return header;	
					
				}	
			}				
		}else if(strcmp(record[i]->tag, "SUBM")==0){
			
			submitterFound = true;
			
			if(record[i]->value == NULL){			

				strcpy(header->source, "ERROR");
				header->gedcVersion = record[i]->lineNum;	
				return header;					
				
			}else{
				insertSorted(&(header->otherFields), createField(record[i]));
						
			}			
						
		}else if(!(strcmp(record[i]->tag, "GEDC")==0)&&(!(strcmp(record[i]->tag,"CONC")==0)||!(strcmp(record[i]->tag,"CONT")==0)) && i!=0){
			
			insertSorted(&(header->otherFields), createField(record[i]));
			
		}
		
	}
		

	if(sourFound == false || versionFound == false || charsetFound == false || submitterFound == false){
		
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
		
		if(record[i]->tag == NULL) break;
		
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
	
	Field *newField = NULL;
	newField = malloc(sizeof(Field));
	newField->tag = NULL;
	newField->value = NULL;
	newField->tag = calloc(sizeof(char) , 31);	
	newField->value = calloc(sizeof(char) , 500);
		
	if(line->tag !=NULL){
				
		strcpy(newField->tag, line->tag);		
		
	}else{
		
		strcpy(newField->tag, " ");
	}
	
	if(line->value!=NULL){
			
		strcpy(newField->value, line->value);		
	}else{
		strcpy(newField->value, " ");
		
	}
	
	return newField;
}
Submitter* createSubmitter(GEDCOMLine ** record, int numLines){

	Submitter* submitter = malloc(sizeof(Submitter) + 5 * sizeof(char));
	strcpy(submitter->submitterName,"");
	strcpy(submitter->address,"");		
	int i = 0;
	submitter->otherFields = initializeList(printField, deleteField, compareFields);
	

	for(i = 0; i< numLines; i++){		
		
		if((record[i]->level == -1 )||(record[i]->tag == NULL)){

			clearList(&(submitter->otherFields));
			sprintf(submitter->submitterName, "ERROR");
			return submitter;
						
		}else if(strcmp(record[i]->tag, "NAME")==0){

			if(record[i]->value == NULL || strlen(record[i]->value) > 60){
						
				clearList(&(submitter->otherFields));
				sprintf(submitter->submitterName, "ERROR");
				sprintf(submitter->address, "%d", record[i]->lineNum);
				return submitter;	
			
			}else{			
				
				//nameFound = true;
				strcpy(submitter->submitterName, record[i]->value);		

			}
	
		}else if((strcmp(record[i]->tag, "ADDR")==0) ){
							
			if(record[i]->value == NULL){
												
				clearList(&(submitter->otherFields));
				sprintf(submitter->submitterName, "ERROR");
				sprintf(submitter->address, "%d", record[i]->lineNum);
				return submitter;	
										
			}else{
					
				submitter = realloc(submitter, sizeof(Submitter) + sizeof(char) * (strlen(record[i]->value)+1));
				strcpy(submitter->address, record[i]->value);
						
			}
				
			
		}else if(!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0)){
			
			insertSorted(&(submitter->otherFields), createField(record[i]));
			
		}	
	
	}

	return submitter;
		
}

char * getLine(char *destination, int maxLength, FILE *fp){
	
	int character;
	char *ptr;

	for (ptr = destination, maxLength--; maxLength > 0; maxLength--) {
		
		character = fgetc(fp);
		if (character == EOF)break;		
		*ptr++ = character;		
		if (character== '\r' )break;				
		if (character == '\n' )break;
	}

	if (ptr == destination || character == EOF){
		
		return NULL;
		
	}

	if(character == '\r'){
		
		if(!((character = fgetc(fp))=='\n')){
			
			ungetc(character, fp);	
			
		}		
	}
	if(character == '\n'){
		
		if(!((character = fgetc(fp))=='\r')){
			
			ungetc(character, fp);	
			
		}		
	}		
	
	*ptr = 0;
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
	individual->families = initializeList(printFamily, dummyDelete, compareFamilies);		
	Event * tempEvent;		
	char* token;	
	int i = 0;
	int k = 0;
	int eventCounter= 0;
	

	for(i = 0; i< numLines; i++){
								
		
		if((record[i]->level == -1 )||(record[i]->tag == NULL)||(i>0 &&(record[i]->level - record[i-1]->level)>1 )){
		
			clearList(&(individual->otherFields));
			clearList(&(individual->events));	
			sprintf(individual->givenName, "ERROR");
			sprintf(individual->surname, "%d", record[i]->lineNum);	
			free(currentEvent);		
			return individual;
								
		}else if(strcmp(record[i]->tag, "NAME")==0){

			if(record[i]->value == NULL ){
				
				strcpy(individual->givenName, "");				
				strcpy(individual->surname, "");				
			
			}else{						
					
				token = strtok(record[i]->value,"/");	
				
				if(token!=NULL){
					
					token[strlen(token)-1] = '\0';
					strcpy(individual->givenName, token);	
					
					
				}	
													
				strcpy(individual->givenName, token);		
				token = strtok(NULL, "/");
				
				if(token!=NULL){
					
					strcpy(individual->surname, token);	
					
				}
			}
	
		}else if(record[i]->level == 1 && (record[i]->value == NULL || strcmp(record[i]->value, "Y")==0) ) {
			
			k = i+1;
			eventCounter++;
			memcpy(&currentEvent[eventCounter-1], &record[i], sizeof(GEDCOMLine*));	
					
			while(record[k]->level>=2){
				
				eventCounter++;
				currentEvent = realloc(currentEvent, sizeof(GEDCOMLine*) * (eventCounter+1));
				memcpy(&currentEvent[eventCounter-1], &record[k], sizeof(GEDCOMLine*));
							
				k++;
			}

			tempEvent = createEvent(currentEvent, eventCounter);
			eventCounter = 0;
			
			if(strcmp(tempEvent->date, "ERROR")==0){										

				clearList(&(individual->otherFields));
				clearList(&(individual->events));					
				sprintf(individual->givenName, "ERROR");
				sprintf(individual->surname, "%s", tempEvent->place);												
				free(tempEvent->date);
				free(tempEvent->place);
				clearList(&(tempEvent->otherFields));
				free(tempEvent);
				free(currentEvent);
				return individual;
								
			}
												
			insertSorted(&(individual->events),tempEvent);				
			tempEvent = NULL;								
					
		}else if((!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0))&& i>0){
			
			insertSorted(&(individual->otherFields), createField(record[i]));
			
		}	
	
	}
	
	free(currentEvent);
	return individual;
		
}
Event* createEvent(GEDCOMLine ** record, int numLines){
	
	Event* event = malloc(sizeof(Event));
	
	event->date = malloc(sizeof(char)*120);
	event->place = malloc(sizeof(char)*120);	
	strcpy(event->date,"");
	strcpy(event->place,"");
	strcpy(event->type,record[0]->tag);				
	int i = 0;
	bool dateFound = false;
	bool placeFound = false;
	event->otherFields = initializeList(printField, deleteField, compareFields);


	for(i = 1; i< numLines; i++){

		
		if((record[i]->level == -1 )||(record[i]->tag == NULL) ||((record[i]->level - record[i-1]->level)>1 )){

		
			clearList(&(event->otherFields));
			sprintf(event->date, "ERROR");
			sprintf(event->place, "%d", record[i]->lineNum);						
			return event;
						
		}else if(strcmp(record[i]->tag, "DATE")==0 && dateFound == false){
			
			if(record[i]->value == NULL ||strcmp(record[i]->value, " ")==0){
										
				clearList(&(event->otherFields));
				sprintf(event->date, "ERROR");
				sprintf(event->place, "%d", record[i]->lineNum);
				return event;	
			
			}else{			
				dateFound = true;
				strcpy(event->date,record[i]->value);

			}
	
		}else if((strcmp(record[i]->tag, "PLAC")==0) && placeFound == false){
							
			if(record[i]->value == NULL){				
								
				clearList(&(event->otherFields));
				sprintf(event->date, "ERROR");
				sprintf(event->place, "%d", record[i]->lineNum);
				return event;	
										
			}else{		
							
				placeFound = true;
				strcpy(event->place, record[i]->value);
						
			}
				
			
		}else if(!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0)){
			
			insertSorted(&(event->otherFields), createField(record[i]));
			
		}	

	}

	return event;	
}
Family* createFamily(GEDCOMLine ** record, int numLines){
	
	Family* family = malloc(sizeof(Family));
	
	family->wife = NULL;
	family->husband = NULL;
	
	family->otherFields = initializeList(printField, deleteField, compareFields);
	family->children = initializeList(printIndividual, dummyDelete, compareIndividuals);
	family->events = initializeList(printEvent, deleteEvent, compareEvents);
	GEDCOMLine ** currentEvent = malloc(sizeof(GEDCOMLine*));	
	Field* errorField = NULL;

	
	Event* tempEvent;
	int i = 0;
	int k = 0;
	int eventCounter =0;	
	

	for(i = 0; i< numLines; i++){								

		if((record[i]->level == -1 )||(record[i]->tag == NULL)||(i>0 &&(record[i]->level - record[i-1]->level)>1 )){								
			printf("Dies because of line error\n");
			errorField = calloc(sizeof(Field),1);
			errorField->tag = malloc(sizeof(char) * 10);
			errorField->value = malloc(sizeof(char) * 10);						
			strcpy(errorField->tag, "ERROR");
			sprintf(errorField->value, "%d", record[i]->lineNum);
			
			clearList(&(family->otherFields));
			insertFront(&(family->otherFields), errorField);						
			clearList(&(family->events));				
			free(currentEvent);		
			return family;
								
		}else if(record[i]->level == 1 && (record[i]->value == NULL || strcmp(record[i]->value, "Y")==0) ) {

			k = i+1;
			eventCounter++;
			memcpy(&currentEvent[eventCounter-1], &record[i], sizeof(GEDCOMLine*));		
							
			while(k < numLines && record[k]->level >=2){
	
				eventCounter++;
				currentEvent = realloc(currentEvent, sizeof(GEDCOMLine*) * (eventCounter+1));
				memcpy(&currentEvent[eventCounter-1], &record[k], sizeof(GEDCOMLine*));							
				k++;
				
			}
			
			tempEvent = createEvent(currentEvent, eventCounter);
			eventCounter = 0;
			
			if(strcmp(tempEvent->date, "ERROR")==0){										

		
				errorField = malloc(sizeof(Field));
				errorField->tag = malloc(sizeof(char) * 10);
				errorField->value = malloc(sizeof(char) * 10);			
				strcpy(errorField->tag, "ERROR");
				sprintf(errorField->value, "%d", record[i]->lineNum);
				clearList(&(family->otherFields));
				insertSorted(&(family->otherFields), errorField);						
				clearList(&(family->events));				
				free(currentEvent);		
				free(tempEvent->date);
				free(tempEvent->place);
				free(tempEvent);				
				return family;
			
			}else{		
												
				insertSorted(&(family->events),tempEvent);
				
			}	
			
			tempEvent = NULL;								
					
		}else if((!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0)) &&i > 0){
			
			insertSorted(&(family->otherFields), createField(record[i]));
			
		}	
	
	}

	free(currentEvent);
	return family;	
		
}
GEDCOMreference* createReference(char* ref_ID, void * data){
	
	GEDCOMreference * reference = malloc(sizeof(GEDCOMreference));	
	reference->ref_ID = malloc(sizeof(char)*22);
	strcpy(reference->ref_ID, ref_ID);
	reference->data = data;		
	return reference;	
	
}
void dummyDelete(void* toBeDeleted){
	
	return;
	
}

bool validateFileName(char* toValidate){
	
	if(toValidate == NULL||strcmp(toValidate,"")==0){
		
		return false;
		
	}
	
	char* token;
	char* tempFileName = malloc(sizeof(char)* 50);	
	strcpy(tempFileName, toValidate);
	token = strtok(tempFileName, ".");
	token = strtok(NULL," \n");
	
	
	if(token == NULL ||strcmp(token,"ged")!=0){
		
		free(tempFileName);
		return false;
		
	}
	FILE* fp= fopen(toValidate, "r");
	if(fp==NULL){
		
		fclose(fp);
		return false;
			
	}
	
	fclose(fp);
	free(tempFileName);
	return true;
	
}


char* printEncoding(CharSet encoding){
	
	char* encodingString = malloc(sizeof(char)*20);
	
	if(encoding == 0){
		strcpy(encodingString, "ANSEL");
		
	}else if(encoding == 1){
		strcpy(encodingString, "UTF-8");
		
	}else if(encoding == 2){
		strcpy(encodingString, "UNICODE");
		
	}else if(encoding == 3){
		strcpy(encodingString, "ASCII");
		
	}
	
	return encodingString;
	
	
}

bool testCompare(const void * first, const void* second){
	
	
	
	
	Individual* p1 = (Individual*)first;
	Individual* p2 = (Individual*)second;	
	
	if(p1==NULL || p2==NULL){
	
		return false;
		
	}	
	
	char* name1 = p1->givenName;
	char* name2 = p2->givenName;	
	
	
	if(strcmp(name1, name2)==0){	
	
		return true;
		
	}else{
		
		return false;
			
	}	
	
	
}
