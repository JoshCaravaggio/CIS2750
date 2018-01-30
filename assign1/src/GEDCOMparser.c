
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"

//***************************************** GEDCOM object functions *****************************************


GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj){
	
	//Need to do error checking for filename
	FILE * fp = fopen(fileName, "r");
	
	
	char * tempLine = malloc(sizeof(char)*256);
	GEDCOMerror error;
	GEDCOMLine ** fileLines = malloc(sizeof(GEDCOMLine*));
	GEDCOMLine ** currentRecord = malloc(sizeof(GEDCOMLine*));

	int currenLevel = 0;
	char* token;
	int i;
	int j;
	int k;
	int lineCounter = 0;
	int recordCounter = 0;
	int numberOfRecords = 0;
	int currentRecordStart = 0;
	Header* header  = NULL;
	Submitter* submitter = NULL;
	bool newRecord = false;
	bool headFound = false;
	bool trlrFound = false;		
	bool errorFound = false;
			
		

	while(getLine(tempLine,256,fp)!=NULL){
		
		//printf("Read in: %s", tempLine);
		if(strcmp(tempLine,"\n")==0){
			error.line = lineCounter;
			error.type = INV_RECORD;
			
			for(i = 0; i<lineCounter; i++){		
				deleteGEDCOMLine(fileLines[i]);
			}			
			
			free(fileLines);
			free(tempLine);
			free(currentRecord);
			fclose(fp);
			return error;			
		}		

		if((!(tempLine[strlen(tempLine)-1] == '\n'))&&!(tempLine[strlen(tempLine)-1] =='\r')){
			errorFound = true;	
		}
		
		fileLines = realloc(fileLines, sizeof(GEDCOMLine*) * (lineCounter + 1));
		fileLines[lineCounter] = convertStringToGEDCOMLine(tempLine, lineCounter);
		lineCounter++;
		
		if(fileLines[lineCounter-1]->level!=0){
			
			recordCounter++;		
			
		}else{
			
			if(lineCounter==1){

				numberOfRecords++;		
			}else{
				
				k = 0;
				
				for(j = currentRecordStart; j <= (currentRecordStart + recordCounter);j++){
					
					currentRecord = realloc(currentRecord, sizeof(GEDCOMLine*) *(k+1));
					memcpy(&currentRecord[k], &fileLines[j], sizeof(GEDCOMLine*));	
					k++;
											
				}
				
				if(strcmp(currentRecord[0]->tag, "HEAD")==0){
				
					appender(currentRecord,k);						
					header = createHeader(currentRecord, k);

					if(strcmp(header->source, "ERROR")==0){
						error.line = header->gedcVersion;
						error.type =INV_HEADER;
											
						printf("%s %f\n", header->source, header->gedcVersion);
				
						for(i = 0; i<lineCounter; i++){
							deleteGEDCOMLine(fileLines[i]);
						}
						if(submitter!=NULL){
							clearList(&(submitter->otherFields));
							free(submitter);							
							
						}
						free(header);
						free(fileLines);
						free(tempLine);		
						free(currentRecord);
						fclose(fp);
						return error;		
					}

					clearList(&(header->otherFields));
					free(header);				
				}	
				
				if(currentRecord[0]->ref_ID != NULL && strcmp(currentRecord[0]->tag, "SUBM")==0){
				
					appender(currentRecord,k);						
					submitter = createSubmitter(currentRecord, k);
					if(!(strcmp(submitter->submitterName, "noName")==0)){
						if(strcmp(submitter->submitterName, "ERROR")==0){
							error.line = atoi(submitter->address);
							error.type = INV_RECORD;
							
																
							for(i = 0; i<lineCounter; i++){
								deleteGEDCOMLine(fileLines[i]);
							}
							//if(header!=NULL){
							//	clearList(&(header->otherFields));
							//	free(header);
							//}
							free(submitter);
							free(fileLines);
							free(tempLine);		
							free(currentRecord);
							fclose(fp);
							return error;		
						}
					}

					clearList(&(submitter->otherFields));
					free(submitter);
								
				}						
						
				k = 0;
				recordCounter = 0;
				numberOfRecords++;
				currentRecordStart = lineCounter-1;			
				
			}
									
		}		
				
		if(strcmp(fileLines[lineCounter-1]->tag,"HEAD")==0){
			headFound = true;
		}
		
		if(strcmp(fileLines[lineCounter-1]->tag,"TRLR")==0){
			trlrFound = true;
		}			
		
		
	}
	
	/**printf("Line %d --", j);
	printf(" Level : %d", fileLines[lineCounter]->level);
	printf(" XRef ID : %s", fileLines[lineCounter]->ref_ID);				
	printf(" Tag : %s", fileLines[lineCounter]->tag);
	printf(" Value : %s\n", fileLines[lineCounter]->value);**/

	if((headFound == false )|| (trlrFound ==false)){
		error.line = -1;
		error.type = INV_GEDCOM;
		
		for(i = 0; i<lineCounter; i++){		
			deleteGEDCOMLine(fileLines[i]);
		}			
			
		free(fileLines);
		free(tempLine);
		free(currentRecord);
		fclose(fp);
		return error;		
		
	}


	error.line = -1;
	error.type = OK;		
		
	

	for(i = 0; i<lineCounter; i++){
		deleteGEDCOMLine(fileLines[i]);
	}
		
	free(fileLines);
	free(tempLine);		
	free(currentRecord);
	fclose(fp);
	return error;
		
}


char* printGEDCOM(const GEDCOMobject* obj);


void deleteGEDCOM(GEDCOMobject* obj);



char* printError(GEDCOMerror err);


Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person);



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
	if(field->value!=NULL){
		strcat(fieldInfo,field->value);
	}
	strcat(fieldInfo,"\n");
	return fieldInfo;
	
}

