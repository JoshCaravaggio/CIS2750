
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"

//***************************************** GEDCOM object functions *****************************************


GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** object){
	
	FILE * fp;	
	GEDCOMerror error;
	
	if(validateFileName(fileName)){
		fp = fopen(fileName, "r");
	}else{
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
		

	GEDCOMLine ** fileLines = malloc(sizeof(GEDCOMLine*));
	GEDCOMLine ** currentRecord = malloc(sizeof(GEDCOMLine*));
	GEDCOMreference **referenceArray = malloc(sizeof(GEDCOMreference*));
	GEDCOMobject* obj = calloc(sizeof(GEDCOMobject), 1);
	
	obj->header = NULL;
	obj->submitter = NULL;
	obj->individuals = initializeList(printIndividual,deleteIndividual,compareIndividuals);
	obj->families = initializeList(printFamily,deleteFamily,compareFamilies);
	 
	

	int i;
	int j;
	int k;
	int lineCounter = 0;
	int recordCounter = 0;
	int numberOfRecords = 0;
	int currentRecordStart = 0;
	int referenceCounter = 0;
	char * tempLine = malloc(sizeof(char)*256);
	Header* header  = NULL;
	Submitter* submitter = NULL;
	Individual* tempIndi = NULL;
	Family* tempFamily = NULL;	
	Field* tempField = NULL;
	bool headFound = false;
	bool trlrFound = false;		

			
		

	while(getLine(tempLine,256,fp)!=NULL){
		
				
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
				
				if(currentRecord[0]->tag != NULL && strcmp(currentRecord[0]->tag, "HEAD")==0){

					appender(currentRecord,k);						
					header = createHeader(currentRecord, k);

					if(strcmp(header->source, "ERROR")==0){
						
						int errorLine = (int)header->gedcVersion;
						error.line =errorLine;
						error.type =INV_HEADER;						
																	
						for(i = 0; i<lineCounter; i++){
							
							deleteGEDCOMLine(fileLines[i]);
							
						}
						
						for(i = 0; i<referenceCounter; i++){
							
							free(referenceArray[i]->ref_ID);
							referenceArray[i]->data = NULL;
							free(referenceArray[i]);
							
						}	
											
						obj->header = header;
						deleteGEDCOM(obj);
						free(referenceArray);
						free(currentRecord);
						free(fileLines);
						free(tempLine);		
						fclose(fp);
						return error;		
						
					}

					(obj->header) = header;				
					
				}	
				
				if(currentRecord[0]->tag != NULL && currentRecord[0]->ref_ID != NULL && strcmp(currentRecord[0]->tag, "SUBM")==0){
				
					appender(currentRecord,k);						
					submitter = createSubmitter(currentRecord, k);
					if(!(strcmp(submitter->submitterName, "noName")==0)){
						if(strcmp(submitter->submitterName, "ERROR")==0){
							error.line = atoi(submitter->address);
							error.type = INV_RECORD;
							
																
							for(i = 0; i<lineCounter; i++){
								deleteGEDCOMLine(fileLines[i]);
							}
							for(i = 0; i<referenceCounter; i++){
								free(referenceArray[i]->ref_ID);
								referenceArray[i]->data = NULL;
								free(referenceArray[i]);
							}	
							free(referenceArray);					
							(obj->submitter) = submitter;					
							deleteGEDCOM(obj);
							free(currentRecord);
							free(fileLines);
							free(tempLine);		
							free(currentRecord);
							fclose(fp);
							return error;		
						}
					}

					obj->submitter = submitter;
					referenceCounter++;
					referenceArray = realloc(referenceArray, sizeof(GEDCOMreference*) * (referenceCounter +1));	
					referenceArray[referenceCounter-1] = createReference(currentRecord[0]->ref_ID, submitter);			
						
								
				}
				
				if(currentRecord[0]->tag != NULL && currentRecord[0]->ref_ID != NULL && strcmp(currentRecord[0]->tag, "INDI")==0){
					
					appender(currentRecord,k);
					tempIndi = createIndividual(currentRecord, k);						
					
					if(strcmp(tempIndi->givenName, "ERROR")==0){
						error.line = atoi(tempIndi->surname);
						error.type = INV_RECORD;
							
																
						for(i = 0; i<lineCounter; i++){
							deleteGEDCOMLine(fileLines[i]);
						}
						for(i = 0; i<referenceCounter; i++){
							free(referenceArray[i]->ref_ID);
							referenceArray[i]->data = NULL;
							free(referenceArray[i]);
						}	
						free(referenceArray);										
						free(tempIndi->givenName);						
						free(tempIndi->surname);						
						free(tempIndi);
						deleteGEDCOM(obj);
						obj = NULL;
						free(fileLines);
						free(tempLine);		
						free(currentRecord);
						fclose(fp);
						return error;		
					}
										
					insertSorted(&(obj->individuals), tempIndi);
					referenceCounter++;
					referenceArray = realloc(referenceArray, sizeof(GEDCOMreference*) * (referenceCounter +1));	
					referenceArray[referenceCounter-1] = createReference(currentRecord[0]->ref_ID, tempIndi);
					tempIndi = NULL;								
					
								
				}

				if(currentRecord[0]->tag != NULL && currentRecord[0]->ref_ID != NULL && (strcmp(currentRecord[0]->tag, "FAM")==0)){
				
					appender(currentRecord,k);
					tempFamily = createFamily(currentRecord, k);						
					
					if((tempFamily->otherFields).head !=NULL){				
						Field * errorField = (Field*)((tempFamily->otherFields).head->data);
						 
						if(strcmp(errorField->tag, "ERROR")==0){			

							error.line = atoi(errorField->value);
							error.type = INV_RECORD;
																								
							for(i = 0; i<lineCounter; i++){
								deleteGEDCOMLine(fileLines[i]);
							}	
							for(i = 0; i<referenceCounter; i++){
								free(referenceArray[i]->ref_ID);
								referenceArray[i]->data = NULL;
								free(referenceArray[i]);
							}
							
							free(referenceArray);	
							clearList(&(tempFamily->events));
							clearList(&(tempFamily->children));	
							clearList(&(tempFamily->otherFields));															
							free(tempFamily);														
							deleteGEDCOM(obj);
							obj = NULL;			
							free(fileLines);
							free(tempLine);		
							free(currentRecord);
							fclose(fp);
							
							return error;		
						}
					}
									
					insertSorted(&(obj->families), tempFamily);
					referenceCounter++;
					referenceArray = realloc(referenceArray, sizeof(GEDCOMreference*) * (referenceCounter +1));	
					referenceArray[referenceCounter-1] = createReference(currentRecord[0]->ref_ID, tempFamily);						
					tempFamily = NULL;
					
								
				}														
						
				k = 0;
				recordCounter = 0;
				numberOfRecords++;
				currentRecordStart = lineCounter-1;			
				
			}
									
		}		
				
		if(fileLines[lineCounter-1]->tag != NULL && strcmp(fileLines[lineCounter-1]->tag,"HEAD")==0){
			headFound = true;
		}
		
		if( fileLines[lineCounter-1]->tag != NULL && strcmp(fileLines[lineCounter-1]->tag,"TRLR")==0){
			trlrFound = true;
		}			
			
	}
	

	if((headFound == false )|| (trlrFound ==false)){
		
		error.line = -1;
		error.type = INV_GEDCOM;
		
		for(i = 0; i<lineCounter; i++){		
			deleteGEDCOMLine(fileLines[i]);
		}
		
		for(i = 0; i<referenceCounter; i++){
			
			free(referenceArray[i]->ref_ID);
			referenceArray[i]->data = NULL;
			free(referenceArray[i]);
			free(referenceArray);		
			
		}					
		
		deleteGEDCOM(obj);
		obj = NULL;	
		free(fileLines);
		free(tempLine);
		free(currentRecord);
		fclose(fp);
		return error;				
	}

	Node* ptr1;
	Node* ptr2;
	
	ptr1 = (header->otherFields).head;
		
	while(ptr1!=NULL){
	
		tempField = (Field*)(ptr1->data);		
		if(strcmp(tempField->tag, "SUBM")==0){
			for(i = 0; i<referenceCounter; i++){
				if(strcmp(tempField->value, referenceArray[i]->ref_ID)==0){
					header->submitter = (Submitter*)(referenceArray[i]->data);
					
				}				
			}						
		}
		ptr1 = ptr1->next;		
	}
	
	ptr1 = (obj->individuals).head;
	while(ptr1!=NULL){
		tempIndi = (Individual*)(ptr1->data);
		ptr2 = (tempIndi->otherFields).head;
		while(ptr2!=NULL){		
			
			tempField = (Field*)(ptr2->data);		
			if(strcmp(tempField->tag, "FAMS")==0||strcmp(tempField->tag, "FAMC")==0){
				for(i = 0; i<referenceCounter; i++){
					if(strcmp(tempField->value, referenceArray[i]->ref_ID)==0){
						
						insertSorted(&(tempIndi->families), (Family*)(referenceArray[i]->data));
						
					}				
				}						
			}
			ptr2 = ptr2->next;		
		}
		ptr1 = ptr1->next;
	}		


	ptr1 = (obj->families).head;
	while(ptr1!=NULL){
		
		tempFamily = (Family*)(ptr1->data);
		ptr2 = (tempFamily->otherFields).head;
		while(ptr2!=NULL){		
			
			tempField = (Field*)(ptr2->data);		
			if(strcmp(tempField->tag, "CHIL")==0||strcmp(tempField->tag, "HUSB")==0||strcmp(tempField->tag, "WIFE")==0){
				for(i = 0; i<referenceCounter; i++){
					if(strcmp(tempField->value, referenceArray[i]->ref_ID)==0 ){
						if(strcmp(tempField->tag, "CHIL")==0){
							
							insertSorted(&(tempFamily->children), (Individual*)(referenceArray[i]->data));
							
						}
						if(strcmp(tempField->tag, "HUSB")==0){
							tempFamily->husband =  (Individual*)(referenceArray[i]->data);
						}						
						if(strcmp(tempField->tag, "WIFE")==0){
							tempFamily->wife =  (Individual*)(referenceArray[i]->data);						
						}
					
					}				
				}						
			}
			ptr2 = ptr2->next;		
		}
		ptr1 = ptr1->next;
	}
	
	error.line = -1;
	error.type = OK;		
		
		
	for(i = 0; i<lineCounter; i++){
		deleteGEDCOMLine(fileLines[i]);
	}
	for(i = 0; i<referenceCounter; i++){
				
		free(referenceArray[i]->ref_ID);
		referenceArray[i]->data = NULL;
		free(referenceArray[i]);
	}		
	free(referenceArray);
	free(fileLines);
	free(tempLine);		
	free(currentRecord);
	fclose(fp);
	*object = obj;
	return error;		
}


char* printGEDCOM(const GEDCOMobject* obj){
	
	if(obj == NULL)return NULL;
	

	Header* header = obj->header;
	Submitter* submitter = obj->submitter;
	Individual* individual = NULL;
	Family* family = NULL;
	List individuals = obj->individuals;
	List families = obj->families;	
	Node * ptr1 =NULL;
	char* gedString = NULL;
	gedString = malloc(sizeof(char)*10);
	char* tempString;	
	int currentLength;
	float tempFloat;
	CharSet tempCharset;
	
	if(obj != NULL && obj->header!=NULL){	
							
		strcpy(gedString, "Header\n");
		tempString = header->source;		
		
		if(tempString!=NULL){			
			gedString = realloc(gedString,sizeof(char)* (20+ (strlen(tempString)) ));
			strcat(gedString, "Source: ");
			strcat(gedString, tempString);
			strcat(gedString, "\n");
			
		}
		currentLength = strlen(gedString);
		
				
		tempCharset = header->encoding;		
		gedString = realloc(gedString,sizeof(char)* (currentLength+ 30 ));		
		strcat(gedString, "Encoding: ");
		currentLength = strlen(gedString);		
		
		tempString = printEncoding(tempCharset);
		gedString = realloc(gedString,sizeof(char)* (currentLength+ (strlen(tempString))+1 ));	
		strcat(gedString, tempString);
		free(tempString);
				
		currentLength = strlen(gedString);
		tempString = malloc(sizeof(char)* 50);
		tempFloat = header->gedcVersion;		
		sprintf(tempString,"\nGedcom Version: %.1f\n", tempFloat);				
		gedString = realloc(gedString,sizeof(char)* (currentLength+ (strlen(tempString))+1 ));
		strcat(gedString, tempString);	
		currentLength = strlen(gedString);
		free(tempString);
				
		tempString = malloc(sizeof(char)* 50);		
		strcpy(tempString, "\nOther Fields: \n");						
		gedString = realloc(gedString,sizeof(char)* (currentLength+ (strlen(tempString))+1 ));					
		strcat(gedString, tempString);
		currentLength = strlen(gedString);
		free(tempString);
		
		if((header->otherFields).head!=NULL){
			
		ptr1 = (header->otherFields).head;

			while(ptr1!=NULL){
				
				tempString = printField(ptr1->data);
				gedString = realloc(gedString,sizeof(char)* (currentLength+ (strlen(tempString)+1) ));				
				strcat(gedString, tempString);
				free(tempString);
				currentLength = strlen(gedString)+1;
				ptr1 = ptr1->next;
			}
		}						
	}
	currentLength = strlen(gedString);
	

	if(obj != NULL && obj->submitter!=NULL){	
		gedString = realloc(gedString,sizeof(char)* (currentLength+ 150 ));										
		strcat(gedString, "\nSubmitter\n");
		strcat(gedString, "Submitter Name: ");
		strcat(gedString, submitter->submitterName);
		strcat(gedString, "\nOther Fields\n");
		currentLength = strlen(gedString);
	
		if((submitter->otherFields).head!=NULL){
		ptr1 = (submitter->otherFields).head;


			while(ptr1!=NULL){			
				tempString = printField(ptr1->data);
				gedString = realloc(gedString,sizeof(char)* (currentLength+ (strlen(tempString)+1) ));				
				strcat(gedString, tempString);
				free(tempString);
				currentLength = strlen(gedString)+1;
				ptr1 = ptr1->next;
			}
		
		}						
	}	

	ptr1 = individuals.head;

	while(ptr1!=NULL){
		
		individual = (Individual*)(ptr1->data);
				
		gedString = realloc(gedString,sizeof(char)* (currentLength+ 30 ));			
		strcat(gedString, "\nIndividual\n");
		strcat(gedString, "Name: ");
		tempString = printIndividual(individual);
		currentLength  = strlen(gedString);
		gedString = realloc(gedString,sizeof(char)* (currentLength+strlen(tempString) +1));			
		strcat(gedString, tempString);
		free(tempString);		
		currentLength = strlen(gedString);
		
		ptr1 = ptr1->next;
	}
	ptr1 = families.head;

	while(ptr1!=NULL){
		
		family = (Family*)(ptr1->data);				
		gedString = realloc(gedString,sizeof(char)* (currentLength+ 30 ));			
		strcat(gedString, "\nFamily\n");				
		tempString = printFamily(family);
		currentLength  = strlen(gedString);
		gedString = realloc(gedString,sizeof(char)* (currentLength+strlen(tempString) +1));			
		strcat(gedString, tempString);
		free(tempString);
		ptr1 = ptr1->next;
		currentLength = strlen(gedString);
	}


	return gedString;	
	
}


void deleteGEDCOM(GEDCOMobject* obj){
		
	if(obj == NULL)return;	
	Header* header = obj->header;
	Submitter* submitter = obj->submitter;
	Individual* tempIndi;
	Family* tempFamily;
	Event* tempEvent;
	
	List indiList = obj->individuals;
	List famList =  obj->families;	
	Node* ptr1;
	Node*  ptr2;	
	
	if(header!=NULL){
		clearList(&(header->otherFields));
		free(header);					
	}
	
	if(submitter!=NULL){
		clearList(&(submitter->otherFields));
		free(submitter);					
	}	
	
	if(indiList.head!=NULL){
		ptr1 = indiList.head;
	
		while(ptr1 !=NULL){
			tempIndi = (Individual*)(ptr1->data);
			clearList(&(tempIndi->otherFields));
			clearList(&(tempIndi)->families);										
			ptr2 = (tempIndi->events).head;				

			while(ptr2!=NULL){
				tempEvent = (Event*)(ptr2->data);
				clearList(&(tempEvent->otherFields));
				ptr2 = ptr2->next;
			}
					
			clearList(&(tempIndi->events));		
			ptr1 = ptr1->next;		
		}
		clearList(&(indiList));	
		ptr1 = famList.head;
	
		
	}
	if(famList.head!=NULL){
		while(ptr1 !=NULL){
			tempFamily = (Family*)(ptr1->data);
			clearList(&(tempFamily->otherFields));	
			clearList(&(tempFamily->children));
											
			ptr2 = (tempFamily->events).head;				

			while(ptr2!=NULL){
				tempEvent = (Event*)(ptr2->data);
				clearList(&(tempEvent->otherFields));
				ptr2 = ptr2->next;
			}
					
			clearList(&(tempFamily->events));		
			ptr1 = ptr1->next;	
		}
		clearList(&(famList));	
	}	
	
	free(obj);
	obj = NULL;
		
}



char* printError(GEDCOMerror err){
	
	
	int lineNum = err.line;
	ErrorCode code = err.type;
	char* errorMsg = malloc(sizeof(char)*30);
	
	if(code == 0){
		sprintf(errorMsg, "OK\n");
		
	}else if(code == 1){
		sprintf(errorMsg, "ERROR: INV_FILE\n");
		
	}else if(code == 2){
		sprintf(errorMsg, "ERROR: INV_GEDCOM\n");
		
	}else if(code == 3){
		sprintf(errorMsg, "ERROR: INV_HEADER Line %d\n", lineNum);
		
	}else if(code == 4){
		sprintf(errorMsg, "ERROR: INV_RECORD Line %d\n", lineNum);
		
	}else{
		sprintf(errorMsg, "OTHER\n");
		
	}

	return errorMsg;
	
	
}


Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person){
	
		

	Node* ptr = NULL;
	Individual* individual = NULL;
	char* tempString;
	
	if(familyRecord==NULL || person == NULL){
		
		return individual;
		
	}	
	
	ptr = (familyRecord->individuals).head;


	while(ptr!=NULL){
		tempString = printIndividual(ptr->data);
	
		free(tempString);

		if(compare(person, ptr->data)==true){
		
			individual = (Individual*) ptr->data;
			return individual;			
			
		}
	
		ptr = ptr->next;
		
	}
	
	return individual;
	
}


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

	if(event1==NULL || event2==NULL ){
		return OTHER;
	}
	
	char* formattedDate1 = NULL;
	char* formattedDate2 = NULL;

	formattedDate1 = formatDate(event1->date);
	formattedDate2 = formatDate(event2->date);
	
	if(formattedDate1==NULL ||formattedDate2==NULL ){
		int comparison = strcmp(event1->type,event2->type);
		free(formattedDate1);
		free(formattedDate2);	
		return comparison;		
	}
	
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
	if(event->date!=NULL){
		strcat(eventInfo,"\nDate: ");
		strcat(eventInfo, event->date);	
	}
	if(event->place !=NULL){
		strcat(eventInfo,"\nPlace: ");
		strcat(eventInfo, event->place);
	}	
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
	Node* ptr;
	char* tempString;
	int currentLength;

	
	if(individual==NULL){

	
		return NULL;
		
	}

	
	char* individualInfo = malloc(sizeof(char)*240);
	
	if(individual->surname != NULL){
			
		strcpy(individualInfo, individual->surname);
		strcat(individualInfo,",");
		
	}
	if(individual->surname ==NULL){
		
		if(individual->givenName!=NULL){
			
			strcpy(individualInfo,individual->givenName);
			
		}
		
	}else{
		
		if(individual->givenName!=NULL){
			
			strcat(individualInfo,individual->givenName);
			
		}		
	}
	
	currentLength = strlen(individualInfo);				
	strcat(individualInfo,"\n");	
	ptr = (individual->events).head;		
	individualInfo = realloc(individualInfo,sizeof(char)* (currentLength+20));						
	strcat(individualInfo,"\nEvents\n");
	currentLength = strlen(individualInfo);	
			
	while(ptr!=NULL){
		
		tempString = printEvent(ptr->data);
		individualInfo = realloc(individualInfo,sizeof(char)* (currentLength+ (strlen(tempString)+1) ));				
		strcat(individualInfo, tempString);
		free(tempString);
		currentLength = strlen(individualInfo)+1;
		ptr = ptr->next;
		
	}
	
	individualInfo = realloc(individualInfo,sizeof(char)* (currentLength+30));		
	ptr = (individual->otherFields).head;
	strcat(individualInfo, "\nOther Fields\n");		
	currentLength = strlen(individualInfo);
	
	while(ptr!=NULL){
		tempString = printField(ptr->data);
		individualInfo = realloc(individualInfo,sizeof(char)* (currentLength+ (strlen(tempString)+1) ));				
		strcat(individualInfo, tempString);
		free(tempString);
		currentLength = strlen(individualInfo)+1;
		ptr = ptr->next;
	}

	
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
	int fam1Children =0;
	int fam2Children =0;
	
	if(family1==NULL ){
		return -1;	
	}

	if(family2==NULL ){
		return 1;	
	}			
	
	if(family1->children.head!=NULL){
		
		fam1Children = family1->children.length;
		
	}else{
		
			fam1Children = 0;
			
	}
	
	if(family2->children.head!=NULL){
		
		fam2Children = family2->children.length;
		
	}else{
		
			fam1Children = 0;
			
	}	

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
	Node* ptr;
	int currentLength;
	char* tempString;	
	
	if(family==NULL){
		
		free(familyInfo);
		return NULL;
		
	}
	
	if(family->wife!=NULL){	
		
		strcpy(familyInfo,"Wife: ");
		
		if(family->wife->surname!=NULL){
			
			strcat(familyInfo,family->wife->surname);	
			
		}
		
		strcat(familyInfo,",");
			
		if(family->wife->givenName!=NULL){		
			
			strcat(familyInfo,family->wife->givenName);	
		}		
	}
	
	if(family->husband!=NULL){	
		
		if(family->wife==NULL){		
			
			strcpy(familyInfo,"\nHusband: ");
			
		}else{
			
			strcat(familyInfo,"\nHusband: ");
			
		}
		
		if(family->husband->surname!=NULL){
			
			strcat(familyInfo,family->husband->surname);
				
		}
		
		strcat(familyInfo,",");	
		
		if(family->husband->givenName!=NULL){		
			
			strcat(familyInfo,family->husband->givenName);	
			
		}
	}
		
	strcat(familyInfo,"\n");				
	ptr = (family->events).head;		
	currentLength = strlen(familyInfo);			
	familyInfo = realloc(familyInfo,sizeof(char)* (currentLength+20));						
	strcat(familyInfo,"\nEvents\n");	
	currentLength = strlen(familyInfo);	
			
	while(ptr!=NULL){
		
		tempString = printEvent(ptr->data);
		familyInfo = realloc(familyInfo,sizeof(char)* (currentLength+ (strlen(tempString)+1) ));				
		strcat(familyInfo, tempString);
		free(tempString);
		currentLength = strlen(familyInfo)+1;
		ptr = ptr->next;
		
	}

	familyInfo = realloc(familyInfo,sizeof(char)* (currentLength+30));		
	ptr = (family->otherFields).head;
	strcat(familyInfo, "\nOther Fields\n");		
	currentLength = strlen(familyInfo);
	
	while(ptr!=NULL){

		tempString = printField(ptr->data);
		familyInfo = realloc(familyInfo,sizeof(char)* (currentLength+ (strlen(tempString)+1) ));				
		strcat(familyInfo, tempString);
		free(tempString);
		currentLength = strlen(familyInfo)+1;
		ptr = ptr->next;
		
	}
	
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
	
	
	
	Field* field1 = NULL;
	field1 = (Field*)(first);
	Field* field2 = NULL;
	field2 = (Field*)(second);
	
	if(field1== NULL || field2 == NULL ){
			
		return OTHER;
		
	}

	if(field1->value== NULL || field1->tag == NULL ){
			
		return OTHER;
		
	}

	if(field2->value == NULL || field2->tag == NULL ){
			
		return OTHER;
		
	}
	
	char* field1Info = calloc(sizeof(char), 240);	
	char* field2Info = calloc(sizeof(char), 240);
	strcpy(field1Info, field1->tag);
	strcat(field1Info," ");
	strcat(field1Info, field1->value);			
	strcpy(field2Info, field2->tag);
	strcat(field2Info," ");
	strcat(field2Info, field2->value);
	int comparison = strcmp(field1Info,field2Info);
	free(field1Info);
	free(field2Info);
	return comparison;	
	
}

char* printField(void* toBePrinted){
	
	Field* field = (Field*)(toBePrinted);
	char* fieldInfo = malloc(sizeof(char)*243);		
	strcpy(fieldInfo, field->tag);
	strcat(fieldInfo," : ");
	
	if(field->value!=NULL){		
		strcat(fieldInfo,field->value);		
	}
	strcat(fieldInfo,"\n");
	return fieldInfo;
	
}

