
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
	
	FILE * fp;	
	GEDCOMerror error;
	char* fileNameCopy = calloc(sizeof(char),120);
	strcpy(fileNameCopy, fileName);

	if(validateFileName(fileNameCopy)){

		fp = fopen(fileNameCopy, "r");

		if(fp==NULL){

			error.type = INV_FILE;
			error.line = -1;
			return error;
			
		}

	}else{
		error.type = INV_FILE;
		error.line = -1;
		return error;
	}
		
	free(fileNameCopy);
	GEDCOMLine ** fileLines = malloc(sizeof(GEDCOMLine*));
	GEDCOMLine ** currentRecord = malloc(sizeof(GEDCOMLine*));
	GEDCOMreference **referenceArray = malloc(sizeof(GEDCOMreference*));
	GEDCOMobject* object = calloc(sizeof(GEDCOMobject), 1);
	
	object->header = NULL;
	object->submitter = NULL;
	object->individuals = initializeList(printIndividual,deleteIndividual,compareIndividuals);
	object->families = initializeList(printFamily,deleteFamily,compareFamilies);
	 

	int i;
	int j;
	int k;
	int lineCounter = 0;
	int recordCounter = 0;
	int numberOfRecords = 0;
	int currentRecordStart = 0;
	int referenceCounter = 0;
	int currentLevel = 0;
	char * tempLine = malloc(sizeof(char)*256);
	Header* header  = NULL;
	Submitter* submitter = NULL;
	Individual* tempIndi = NULL;
	Family* tempFamily = NULL;	
	Field* tempField = NULL;
	bool headFound = false;
	bool trlrFound = false;		
	bool submFound = false;

			
		

	while(getLine(tempLine,255,fp)!=NULL){
		
					
		fileLines = realloc(fileLines, sizeof(GEDCOMLine*) * (lineCounter + 1));
		fileLines[lineCounter] = convertStringToGEDCOMLine(tempLine, lineCounter, currentLevel);		

		currentLevel = fileLines[lineCounter]->level;
		lineCounter++;


		if((lineCounter == 1 && fileLines[0]->level !=0)){
			
			int errorLine = lineCounter-1;
			error.line = errorLine;

		
			error.type = INV_HEADER;						
						

			for(i = 0; i<lineCounter; i++){
				
				deleteGEDCOMLine(fileLines[i]);
				
			}
			
			for(i = 0; i<referenceCounter; i++){
				
				free(referenceArray[i]->ref_ID);
				referenceArray[i]->data = NULL;
				free(referenceArray[i]);
				
			}	
								
			deleteGEDCOM(object);
			free(referenceArray);
			free(currentRecord);
			free(fileLines);
			free(tempLine);		
			fclose(fp);

			return error;
			
			
		}
		
		
		
		
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

					if(strcmp(fileLines[j]->tag, "MAXLEN")==0 || fileLines[j]->level == -1){
					
						int errorLine = j+1;
						error.line = errorLine;


						if(strcmp(currentRecord[0]->tag,"HEAD")==0){
											
				
							error.type = INV_HEADER;

						}else{
					
							error.type = INV_RECORD;						
						}							

						for(i = 0; i<lineCounter; i++){
							
							deleteGEDCOMLine(fileLines[i]);
							
						}
						
						for(i = 0; i<referenceCounter; i++){
							
							free(referenceArray[i]->ref_ID);
							referenceArray[i]->data = NULL;
							free(referenceArray[i]);
							
						}	
											
						deleteGEDCOM(object);
						free(referenceArray);
						free(currentRecord);
						free(fileLines);
						free(tempLine);		
						fclose(fp);

						return error;
						
						
					}
													
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
											
						object->header = header;
						deleteGEDCOM(object);
						free(referenceArray);
						free(currentRecord);
						free(fileLines);
						free(tempLine);		
						fclose(fp);
						return error;		
						
					}

					(object->header) = header;				
					
				}else if(currentRecord[0]->tag != NULL && currentRecord[0]->ref_ID != NULL && strcmp(currentRecord[0]->tag, "SUBM")==0){
				
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
							(object->submitter) = submitter;					
							deleteGEDCOM(object);
							free(currentRecord);
							free(fileLines);
							free(tempLine);		
							free(currentRecord);
							fclose(fp);
							return error;		
						}
					}

					object->submitter = submitter;
					referenceCounter++;
					referenceArray = realloc(referenceArray, sizeof(GEDCOMreference*) * (referenceCounter +1));	
					referenceArray[referenceCounter-1] = createReference(currentRecord[0]->ref_ID, submitter);	
					submFound = true;		
						
								
				}else if(currentRecord[0]->tag != NULL && currentRecord[0]->ref_ID != NULL && strcmp(currentRecord[0]->tag, "INDI")==0){
					
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
						deleteGEDCOM(object);
						object = NULL;
						free(fileLines);
						free(tempLine);		
						free(currentRecord);
						fclose(fp);
						return error;		
					}
										
					insertSorted(&(object->individuals), tempIndi);
					referenceCounter++;
					referenceArray = realloc(referenceArray, sizeof(GEDCOMreference*) * (referenceCounter +1));	
					referenceArray[referenceCounter-1] = createReference(currentRecord[0]->ref_ID, tempIndi);
					tempIndi = NULL;								
					
								
				}else if(currentRecord[0]->tag != NULL && currentRecord[0]->ref_ID != NULL && (strcmp(currentRecord[0]->tag, "FAM")==0)){
				
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
							deleteGEDCOM(object);
							object = NULL;			
							free(fileLines);
							free(tempLine);		
							free(currentRecord);
							fclose(fp);
							
							return error;		
						}
					}
									
					insertSorted(&(object->families), tempFamily);
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
	

	if((headFound == false )|| (trlrFound ==false)||(submFound == false)){
		
		
		error.line = -1;
		error.type = INV_GEDCOM;
		
		for(i = 0; i<lineCounter; i++){		
			deleteGEDCOMLine(fileLines[i]);
		}
		
		for(i = 0; i<referenceCounter; i++){
					
			free(referenceArray[i]->ref_ID);
			referenceArray[i]->data = NULL;
			free(referenceArray[i]);	
			
		}					
		free(referenceArray);
		deleteGEDCOM(object);
		object = NULL;	
		free(fileLines);
		free(tempLine);
		free(currentRecord);
		fclose(fp);
		return error;				
	}

	Node* ptr1;
	Node* ptr2;
	bool matchFound = false;
	ptr1 = (header->otherFields).head;
		
	while(ptr1!=NULL){
		matchFound = false;
		tempField = (Field*)(ptr1->data);		
		if(strcmp(tempField->tag, "SUBM")==0){
			for(i = 0; i<referenceCounter; i++){
				if(strcmp(tempField->value, referenceArray[i]->ref_ID)==0){
					header->submitter = (Submitter*)(referenceArray[i]->data);
					ptr1 = ptr1->next;
					deleteField(deleteDataFromList(&(header->otherFields), tempField));	
					matchFound = true;
					break;

				}				
			}				
			if(matchFound == false)ptr1 = ptr1->next;		
		}else{
			ptr1 = ptr1->next;	
		}	
	}
	
	

	ptr1 = (object->individuals).head;
	while(ptr1!=NULL){
		tempIndi = (Individual*)(ptr1->data);
		ptr2 = (tempIndi->otherFields).head;

		while(ptr2!=NULL){		
			matchFound = false;			
			tempField = (Field*)(ptr2->data);		
			if(strcmp(tempField->tag, "FAMS")==0||strcmp(tempField->tag, "FAMC")==0){
				for(i = 0; i<referenceCounter; i++){
					if(strcmp(tempField->value, referenceArray[i]->ref_ID)==0){
						
						insertSorted(&(tempIndi->families), (Family*)(referenceArray[i]->data));
						ptr2 = ptr2->next;
						deleteField(deleteDataFromList(&(tempIndi->otherFields), tempField));	

						matchFound = true;
						break;

					}				
				}
				if(matchFound == false)ptr2 = ptr2->next;							
			}else{
				ptr2 = ptr2->next;
			}
		
		}
		ptr1 = ptr1->next;
	}		

	
	ptr1 = (object->families).head;
	while(ptr1!=NULL){
		
		tempFamily = (Family*)(ptr1->data);
		ptr2 = (tempFamily->otherFields).head;
		while(ptr2!=NULL){		
			matchFound = false;
			tempField = (Field*)(ptr2->data);		
			if(strcmp(tempField->tag, "CHIL")==0||strcmp(tempField->tag, "HUSB")==0||strcmp(tempField->tag, "WIFE")==0){
				for(i = 0; i<referenceCounter; i++){
					
					if(strcmp(tempField->value, referenceArray[i]->ref_ID)==0 ){
						if(strcmp(tempField->tag, "CHIL")==0){
										
							insertSorted(&(tempFamily->children), (Individual*)(referenceArray[i]->data));
							ptr2 = ptr2->next;
							deleteField(deleteDataFromList(&(tempFamily->otherFields), tempField));	
							matchFound = true;						
							break;
							
						}else if(strcmp(tempField->tag, "HUSB")==0){
							tempFamily->husband =  (Individual*)(referenceArray[i]->data);			
							ptr2 = ptr2->next;			
							deleteField(deleteDataFromList(&(tempFamily->otherFields), tempField));	
							matchFound = true;
							break;

						}else if(strcmp(tempField->tag, "WIFE")==0){
							tempFamily->wife =  (Individual*)(referenceArray[i]->data);	
							ptr2 = ptr2->next;
							deleteField(deleteDataFromList(&(tempFamily->otherFields), tempField));
							matchFound = true;	
							break;			

						}

					}				
				}
				if(matchFound==false)ptr2 = ptr2->next;						
			}else{
				ptr2 = ptr2->next;
			}
			

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
	*obj = object;
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
		
	}else if(code == 5){
		sprintf(errorMsg, "OTHER_ERROR\n");	
		
		
	}else if(code == 6){
		sprintf(errorMsg, "ERROR: WRITE_ERROR Line %d\n", lineNum);
		
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


List getDescendants(const GEDCOMobject* familyRecord, const Individual* person){

	List descendantList = initializeList(printIndividual, dummyDelete, compareIndividuals);
	
	if(familyRecord ==NULL || person == NULL){
		
		return descendantList;
			
	}
	
	
	recursivelyAddDescendants(&descendantList, person);	
	return descendantList;						
	
}
GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj){

	GEDCOMerror error;
		
	if(obj == NULL || validateFileName(fileName)==false){
		error.type = WRITE_ERROR;
		error.line = -1;
		return error;

	}

	GEDCOMreference **referenceArray = malloc(sizeof(GEDCOMreference*));

	Node* ptr;
	char* tempXref = NULL;
	int indiCounter= 0;
	int famCounter = 0;
	int referenceCounter = 0;

	for(ptr = (obj->individuals).head; ptr !=NULL; ptr = ptr->next){

		tempXref = malloc(sizeof(char)*10);
		sprintf(tempXref,"@I%d@", indiCounter);
		indiCounter++;
		referenceCounter++;
		referenceArray = realloc(referenceArray, sizeof(GEDCOMreference*) * (referenceCounter +1));	
		referenceArray[referenceCounter-1] = createReference(tempXref, (Individual*)(ptr->data));
		free(tempXref);

	}

	for(ptr = (obj->families).head; ptr !=NULL; ptr = ptr->next){

		tempXref = malloc(sizeof(char)*10);
		sprintf(tempXref,"@F%d@", famCounter);
		famCounter++;
		referenceCounter++;
		referenceArray = realloc(referenceArray, sizeof(GEDCOMreference*) * (referenceCounter +1));	
		referenceArray[referenceCounter-1] = createReference(tempXref, (Family*)(ptr->data));	
		free(tempXref);

	}

	FILE * fp = fopen(fileName, "w");
	writeHeader(fp, obj->header);
	writeSubmitter(fp, obj->submitter);

	for(ptr = (obj->individuals).head; ptr !=NULL; ptr = ptr->next ){
			
		error = writeIndividual(fp,(Individual*)(ptr->data), referenceArray, referenceCounter);
		if(error.type!=OK){

			error.type = WRITE_ERROR;			
			error.line = -1;
			return error;
		}
	}

	for(ptr = (obj->families).head; ptr !=NULL; ptr = ptr->next){
			
		error = writeFamily(fp,(Family*)(ptr->data), referenceArray, referenceCounter);
		if(error.type!=OK){
			error.type = WRITE_ERROR;
			error.line = -1;
			return error;
		}

	}
	for(int i = 0; i<referenceCounter; i++){

		free(referenceArray[i]->ref_ID);
		referenceArray[i]->data = NULL;
		free(referenceArray[i]);
		
	}
	fprintf(fp, "0 TRLR\n");
	free(referenceArray);
	error.type = OK;
	error.line = -1;
	fclose(fp);
	return error;
}
ErrorCode validateGEDCOM(const GEDCOMobject* obj){


	if(obj == NULL){

		return  INV_GEDCOM;
	}

	if(obj->header == NULL || obj->submitter == NULL){

		return  INV_GEDCOM;

	}

	Header* header = obj->header;

	if(header->submitter ==NULL){


		return  INV_HEADER;

	}

	if(strlen(header->source)>200){

		printf("Header->source length >200\n");
		return INV_RECORD;

	}

	Individual* tempIndi;

	for(Node* ptr = (obj->individuals).head; ptr!=NULL; ptr = ptr->next){

		tempIndi = (Individual*)(ptr->data);

		if(tempIndi == NULL){

		printf("Individual NULL\n");
		
			return INV_RECORD;

		}

		if(strlen(tempIndi->givenName)>120 || strlen(tempIndi->surname)>120){
	
			printf("Individual name exceeds max length\n");

			return INV_RECORD;			

		}

	}

	Family* tempFamily;

	for(Node* ptr = (obj->families).head; ptr!=NULL; ptr = ptr->next){

		tempFamily = (Family*)(ptr->data);

		if(tempFamily == NULL ){

			printf("Family NULL \n");
		
			return INV_RECORD;			

		}

		
		for(Node* ptr2 = (tempFamily->children).head; ptr2!=NULL; ptr2 = ptr2->next){

			tempIndi = (Individual*)(ptr2->data);
			if(tempIndi == NULL){

			printf("Child NULL \n");

			return INV_RECORD;			


			}


		}

	}

	return OK;			



}

List getDescendantListN(const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen){

	List descendantListN = initializeList(printIndividualList, deleteIndividualList ,compareIndividualsLists );
	char* string = NULL;
	free(string);
	List* firstGeneration = calloc(sizeof(List),1);
	firstGeneration->head = NULL;
	firstGeneration->tail = NULL;
	firstGeneration->deleteData = dummyDelete;
	firstGeneration->compare = compareIndividuals;
	firstGeneration->printData = printIndividual;
	firstGeneration->length = 0;
	
	insertBack(&descendantListN,firstGeneration);
	recursivelyAddDescendantsN(&descendantListN, firstGeneration,  person, 1 , maxGen);

	return descendantListN;
}

List getAncestorListN(const GEDCOMobject* familyRecord, const Individual* person, int maxGen){

	List ancestorListN = initializeList(printIndividualList, deleteIndividualList ,compareIndividualsLists );
	char* string = NULL;
	free(string);
	List* firstGeneration = calloc(sizeof(List),1);
	firstGeneration->head = NULL;
	firstGeneration->tail = NULL;
	firstGeneration->deleteData = dummyDelete;
	firstGeneration->compare = compareIndividuals;
	firstGeneration->printData = printIndividual;
	firstGeneration->length = 0;
	
	insertBack(&ancestorListN,firstGeneration);
	recursivelyAddAncestorsN(&ancestorListN, firstGeneration,  person, 1 , maxGen);

	return ancestorListN;
}

char* indToJSON(const Individual* ind){

	char* JSONstring;
	int currentLength = 0;
	JSONstring = calloc(sizeof(char), 2);	

	if(ind == NULL){	
		strcpy( JSONstring,"");		
		return JSONstring;
	}

	JSONstring = realloc(JSONstring, sizeof(char)*140);
	strcpy(JSONstring,"{\"givenName\":\"");
	if(ind->givenName!=NULL){
		strcat( JSONstring, ind->givenName);
	}else{
		strcat( JSONstring, "\"");		
	}

	currentLength = strlen(JSONstring);
	JSONstring = realloc(JSONstring, sizeof(char)*(currentLength + 140));
	strcat(JSONstring, "\",\"surname\":\"");

	if(ind->surname!=NULL){
		strcat(JSONstring, ind->surname);
	}else{
		strcat(JSONstring,"\"");		
	}	
	currentLength = strlen(JSONstring);
	JSONstring = realloc(JSONstring, sizeof(char)*(currentLength + 10));
	strcat( JSONstring, "\"}");		
	return JSONstring;

}

Individual* JSONtoInd(const char* str){

	if(str == NULL){
		return NULL;

	}

	int totalLength = strlen(str);
	char* tempString = calloc(sizeof(char), totalLength+1);
	strcpy(tempString, str);

	Individual* newInd = malloc(sizeof(Individual));
	newInd->events = initializeList(printEvent, deleteEvent, compareEvents);
	newInd->otherFields = initializeList(printField, deleteField, compareFields);	
	newInd->families = initializeList(printFamily, deleteFamily, compareFamilies);	
	char* token = strtok(tempString, ":");

	if(strcmp(token, "{\"givenName\"")!=0){
		printf("Incorrect format for givenName, token: %s\n", token);
		free(tempString);
		free(newInd);
		return NULL;
	}
	token = strtok(NULL, ",");
	newInd->givenName = calloc(sizeof(char),120);

	if(strcmp(token, "\"\"")!=0){

		char* gName = calloc(sizeof(char),120);
		int nameCounter = 0;
		for(int i =  0; i<strlen(token);i++){
			if(token[i]!='\"'){
				gName[nameCounter++] = token[i];
			}

		}
		strcpy(newInd->givenName, gName);
		free(gName);
	}else{
		strcpy(newInd->givenName, "");
	}
	printf("Given Name: %s\n", newInd->givenName);
	token = strtok(NULL, ":");

	if(strcmp(token,"\"surname\"" )!=0){

		printf("Incorrect format for surname, token: %s\n", token);
		free(tempString);
		free(newInd->givenName);
		free(newInd);
		return NULL;	
	}

	token = strtok(NULL, "\"");
	newInd->surname = calloc(sizeof(char),120);
	strcpy(newInd->surname, token);	
	printf("Surname: %s\n", newInd->surname);
	free(tempString);
	return newInd;

}
GEDCOMobject* JSONtoGEDCOM(const char* str){

	if(str == NULL){
		return NULL;

	}

	int totalLength = strlen(str);
	char* tempString = calloc(sizeof(char), totalLength+1);
	strcpy(tempString, str);

	GEDCOMobject* newObj = malloc(sizeof(GEDCOMobject));
	newObj->header =NULL;
	newObj->submitter = NULL;
	newObj->families = initializeList(printFamily, deleteFamily, compareFamilies);	
	newObj->individuals = initializeList(printIndividual, deleteIndividual, compareIndividuals);

	Header* newHeader = calloc(sizeof(Header),1);
	newObj->header = newHeader;


	char* token = strtok(tempString, ":");

	if(strcmp(token, "{\"source\"")!=0){
		printf("Incorrect format for source, token: %s\n", token);
		free(tempString);
		deleteGEDCOM(newObj);
		return NULL;
	}
	token = strtok(NULL, ",");

	if(strcmp(token, "\"\"")!=0){

		char* sourceName = calloc(sizeof(char),120);
		int nameCounter = 0;
		for(int i =  0; i<strlen(token);i++){
			if(token[i]!='\"' &&token[i]!='{'&&token[i]!='}'){
				sourceName[nameCounter++] = token[i];
			}

		}
		for(int i = 0; i<strlen(sourceName); i++){
			newHeader->source[i] = sourceName[i];

		}
		free(sourceName);
	}

	printf("Header Source: %s\n", newObj->header->source);

	token = strtok(NULL, ":");
	if(strcmp(token, "\"gedcVersion\"")!=0){
		printf("Incorrect format for Version, token: %s\n", token);
		free(tempString);
		deleteGEDCOM(newObj);
		return NULL;
	}
	token = strtok(NULL, ",");

	if(strcmp(token, "\"\"")!=0){

		char* versionString = calloc(sizeof(char),10);
		int versCounter = 0;

		for(int i =  0; i<strlen(token);i++){
			if(token[i]!='\"' &&token[i]!='{'&&token[i]!='}'){
				versionString[versCounter++] = token[i];
			}

		}

		for(int i = 0; i<strlen(versionString); i++){
			newHeader->gedcVersion = atof(versionString);

		}
		free(versionString);
	}

	printf("Header version: %.1f\n", newHeader->gedcVersion );
	 token = strtok(NULL, ":");

	if(strcmp(token, "\"encoding\"")!=0){
		printf("Incorrect format for encoding, token: %s\n", token);
		free(tempString);
		deleteGEDCOM(newObj);
		return NULL;
	}
	token = strtok(NULL, ",");

	if(strcmp(token, "\"\"")!=0){

		char* encodingString = calloc(sizeof(char),120);
		int enCounter = 0;
		for(int i =  0; i<strlen(token);i++){
			if(token[i]!='\"' &&token[i]!='{'&&token[i]!='}'){
				encodingString[enCounter++] = token[i];
			}

		}

			newHeader->encoding = decodeCharSet(encodingString);

		free(encodingString);
	}
	printf("Header Encoding: %s\n",encodeCharSet(newHeader->encoding) );

	token = strtok(NULL, ":");

	if(strcmp(token, "\"subName\"")!=0){
		printf("Incorrect format for submitter name, token: %s\n", token);
		free(tempString);
		deleteGEDCOM(newObj);
		return NULL;
	}
	token = strtok(NULL, ",");
	char* subName = NULL;
	if(strcmp(token, "\"\"")!=0){

		subName = calloc(sizeof(char),120);
		int nameCounter = 0;
		for(int i =  0; i<strlen(token);i++){
			if(token[i]!='\"' &&token[i]!='{'&&token[i]!='}'){
				subName[nameCounter++] = token[i];
			}

		}

	}


	 token = strtok(NULL, ":");

	if(strcmp(token, "\"subAddress\"")!=0){
		printf("Incorrect format for submitter address, token: %s\n", token);
		free(tempString);
		deleteGEDCOM(newObj);
		return NULL;
	}
	token = strtok(NULL, ",");
	Submitter* newSubmitter = NULL;
	newObj->submitter = newSubmitter;
	if(strcmp(token, "\"\"")!=0){

		char* subAddr = calloc(sizeof(char),300);
		int addrCounter = 0;
		for(int i =  0; i<strlen(token);i++){
			if(token[i]!='\"' &&token[i]!='{'&&token[i]!='}'){
				subAddr[addrCounter++] = token[i];
			}

		}	

		newSubmitter = calloc(sizeof(Submitter)+ sizeof(char)*strlen(subName)+1, 1);
		newObj->submitter = newSubmitter;


		sprintf(newSubmitter->submitterName,"%s", subName);

	
		free(subName);
		sprintf(newSubmitter->address,"%s", subAddr);

		free(subAddr);
		printf("Submitter Name: %s\n",newSubmitter->submitterName );
		printf("Submitter Address: %s\n",newSubmitter->address );
	}

	free(tempString);		
	return newObj;

}

void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded){

	if(obj == NULL || toBeAdded == NULL){
		return;
	}
	insertSorted(&(obj->individuals), (void*)toBeAdded);	

}
char* iListToJSON(List iList){

	char* JSONString = NULL;
	char* tempString;
	int currentLength = 3;
	if(iList.length == 0){

		JSONString = calloc(sizeof(char),3);
		strcpy(JSONString, "[]");

		return JSONString;

	}


	JSONString = calloc(sizeof(char),3);

	strcpy(JSONString, "[");
	Individual* tempIndi = NULL;
	for(Node* ptr = iList.head; ptr!=NULL; ptr= ptr->next){

		tempIndi = (Individual*)ptr->data;
		tempString = indToJSON(tempIndi);		
		currentLength = currentLength + strlen(tempString);
		JSONString = realloc(JSONString, sizeof(char)*currentLength+2);
		strcat(JSONString, tempString);
		free(tempString);
		strcat(JSONString, ",");
		currentLength = strlen(JSONString);

	}
	JSONString = realloc(JSONString, sizeof(char)*currentLength+2);

	JSONString[strlen(JSONString)-1] = ']';
	
	return JSONString;

}
char* gListToJSON(List gList){


	char* JSONString = NULL;
	char* tempString;
	int currentLength = 3;
	if(gList.length == 0){

		JSONString = calloc(sizeof(char),3);
		strcpy(JSONString, "[]");

		return JSONString;

	}


	JSONString = calloc(sizeof(char),3);

	strcpy(JSONString, "[");
	List* tempList = NULL;
	for(Node* ptr = gList.head; ptr!=NULL; ptr= ptr->next){

		tempList = (List*)ptr->data;
		tempString = iListToJSON(*(tempList));		
		currentLength = currentLength + strlen(tempString);
		JSONString = realloc(JSONString, sizeof(char)*currentLength+2);
		strcat(JSONString, tempString);
		free(tempString);
		strcat(JSONString, ",");
		currentLength = strlen(JSONString);

	}
	JSONString = realloc(JSONString, sizeof(char)*currentLength+2);

	JSONString[strlen(JSONString)-1] = ']';
	
	return JSONString;



}

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
		return OTHER_ERROR;
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
		return OTHER_ERROR;
	}
	
	if(individual1->surname==NULL ||individual2->surname==NULL ){
		return OTHER_ERROR;
	}
	
	if(individual1->givenName==NULL ||individual2->givenName==NULL ){
		return OTHER_ERROR;
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
		individualInfo = realloc(individualInfo,sizeof(char)* (currentLength+ (strlen(tempString)+2) ));				
		strcat(individualInfo, tempString);
		strcat(individualInfo, "\n");
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
			
		return OTHER_ERROR;
		
	}

	if(field1->value== NULL || field1->tag == NULL ){
			
		return OTHER_ERROR;
		
	}

	if(field2->value == NULL || field2->tag == NULL ){
			
		return OTHER_ERROR;
		
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

//*************************************************************************************************
//											Utility Functions
//*************************************************************************************************
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

GEDCOMLine* convertStringToGEDCOMLine(char* toConvert, int lineNum, int previousLevel){
	
	int i = 0;
	char* token;
	int level = previousLevel;

	GEDCOMLine* line = malloc(sizeof(GEDCOMLine));
	line->lineNum = lineNum;
	line->tag = NULL;
	line->ref_ID = NULL;
	line->value = NULL;

	
	if(toConvert[strlen(toConvert)-1]!='\r' && toConvert[strlen(toConvert)-1]!='\n'){
	//printf("Line %d exceeds max length\n", lineNum);
		line->tag = malloc(sizeof(char)*31);
		line->level = previousLevel;
		strcpy(line->tag, "MAXLEN");
		return line;
	}	
	
	
	
	token = strtok(toConvert," \r\n");	
	
	if(validateLevel(token, level)==true){
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
		//if(line->value[strlen(line->value)-1]!='\r'&&line->value[strlen(line->value)-1]!='\n')line->level = -2;


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
	
	int level = atoi(toValidate);

	if((level-previousLevel)>1 ){
		//printf("Previous level: %d Current Level: %d \n", previousLevel, level);
		return false;
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
					
				
					if(record[i]->value == NULL || record[i]->level == -1){
						
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

char* encodeCharSet(CharSet toEncode){

	char* tempString = NULL;

	if(toEncode == ANSEL){
		
		tempString = "ANSEL";
		return tempString;
			
	}else if(toEncode == UTF8){
	
		tempString = "UTF-8";
		return tempString;
		
	}else if(toEncode == UNICODE){

		tempString = "UNICODE";
		return tempString;
		
	}else if(toEncode == ASCII ){

		tempString = "ASCII";
		return tempString;
		
	}else{
		
		return NULL;
		
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

	if (ptr == destination){

		return NULL;
		
	}
	if(character ==EOF){
		*ptr++ = '\n';

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
								
		}else if(strcmp(record[i]->tag, "GIVN")==0||strcmp(record[i]->tag, "SURN")==0){

			insertSorted(&(individual->otherFields), createField(record[i]));			

		}else if(strcmp(record[i]->tag, "NAME")==0){

			if(record[i]->value == NULL ){
				
				strcpy(individual->givenName, "");				
				strcpy(individual->surname, "");				
			
			}else{						
					
				token = strtok(record[i]->value,"/");	
				
				if(token!=NULL){
					
					if(token[strlen(token)-1]==' ')token[strlen(token)-1] = '\0';
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
			int lineCounter = 0;
			memcpy(&currentEvent[eventCounter-1], &record[i], sizeof(GEDCOMLine*));	
					
			while(record[k]->level>=2){

				eventCounter++;
				currentEvent = realloc(currentEvent, sizeof(GEDCOMLine*) * (eventCounter+1));
				memcpy(&currentEvent[eventCounter-1], &record[k], sizeof(GEDCOMLine*));
				lineCounter++;			
				k++;
			}
			
			tempEvent = createEvent(currentEvent, eventCounter);
			eventCounter = 0;
	
			if(tempEvent->date == NULL || strcmp(tempEvent->date, "ERROR")==0){				
										
				clearList(&(individual->otherFields));
				clearList(&(individual->events));					
				sprintf(individual->givenName, "ERROR");
				sprintf(individual->surname, "%s", tempEvent->place);												
				free(tempEvent->date);
				free(tempEvent->place);								
				free(tempEvent);
				free(currentEvent);
				return individual;
								
			}
												
			insertSorted(&(individual->events),tempEvent);				
			i = i + lineCounter-1;
			tempEvent = NULL;								
					
		}else if((!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0))&& i>0 && record[i]->level <2){
			
			insertSorted(&(individual->otherFields), createField(record[i]));
			
		}	
	
	}
	
	free(currentEvent);
	return individual;
		
}
Event* createEvent(GEDCOMLine ** record, int numLines){
	
	Event* event = calloc(sizeof(Event),1);
	
	event->date = calloc(sizeof(char),120);
	event->place = calloc(sizeof(char),120);	
	strcpy(event->date,"");
	strcpy(event->place,"");
	
	if(strlen(record[0]->tag)>4){
	
		sprintf(event->date, "ERROR");
		sprintf(event->place, "%d", record[0]->lineNum);						
		return event;	
		
	}
	
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
				sprintf(event->date,"%s",record[i]->value);

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
			int lineCounter = 0;
			eventCounter++;
			memcpy(&currentEvent[eventCounter-1], &record[i], sizeof(GEDCOMLine*));		
							
			while(k < numLines && record[k]->level >=2){
	
				eventCounter++;
				currentEvent = realloc(currentEvent, sizeof(GEDCOMLine*) * (eventCounter+1));
				memcpy(&currentEvent[eventCounter-1], &record[k], sizeof(GEDCOMLine*));			
				lineCounter++;				
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
			i = i + lineCounter-1;
			tempEvent = NULL;								
					
		}else if((!(strcmp(record[i]->tag,"CONC")==0)&& i!=0 &&!(strcmp(record[i]->tag,"CONT")==0)) &&i > 0 && record[i]->level<2){
			
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

	
	if(fp!=NULL)fclose(fp);
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


void recursivelyAddDescendants(List *descendantList, const Individual* currentPerson){
	
	Node* ptr1 = NULL;
	Node* ptr2 = NULL;
	Family* tempFam;
	
	ptr1 =(currentPerson->families).head;
	
	while(ptr1!=NULL){
		
		tempFam = (Family*)ptr1->data;
		if(tempFam->wife == currentPerson || tempFam->husband == currentPerson){
			ptr2 = (tempFam->children).head;
			
			while(ptr2!=NULL){
				
				insertSorted(descendantList, ptr2->data);
				recursivelyAddDescendants(descendantList, ptr2->data);
				ptr2 = ptr2->next;				
				
			}			
			
		}
		ptr1 = ptr1->next;				
		
	}
	
}



GEDCOMerror writeHeader(FILE * fp, Header* header){

	GEDCOMerror error;
	if(header == NULL || fp == NULL){
		error.type = WRITE_ERROR;
		error.line = -1;
		return error;

	}
	char* tempString = NULL;
							
	fprintf(fp, "0 HEAD\n");
	tempString = header->source;		
	
	if(tempString!=NULL){			

		fprintf(fp, "1 SOUR ");
		fprintf(fp,"%s\n",tempString);
		
	}
	tempString = encodeCharSet(header->encoding);
	if(tempString!=NULL){			

		fprintf(fp,"1 CHAR ");
		fprintf(fp, "%s\n", tempString);
		
	}
	fprintf(fp,"1 GEDC\n");
	fprintf(fp,"1 VERS ");	
	fprintf(fp,"%.1f\n", header->gedcVersion);
	fprintf(fp,"1 FORM LINEAGE-LINKED\n");		
	fprintf(fp, "1 SUBM @SUB1@\n");
	error.type = OK;
	error.line = -1;
	return error;
	
}

GEDCOMerror writeSubmitter(FILE * fp, Submitter* submitter){

	GEDCOMerror error;
	if(submitter == NULL || fp == NULL){
		error.type = WRITE_ERROR;
		error.line = -1;
		return error;

	}
	char* tempString = NULL;
							
	fprintf(fp, "0 @SUB1@ SUBM\n");
	tempString = submitter->submitterName;		
	
	if(tempString!=NULL){			

		fprintf(fp, "1 NAME ");
		fprintf(fp,"%s\n",tempString);
		
	}

	tempString = submitter->address;

	if(strcmp(tempString,"")!=0){			

		fprintf(fp, "1 ADDR ");
		char* token = strtok(tempString, "\n");
		fprintf(fp, "%s\n", token);

		while((token = strtok(NULL, "\n"))!= NULL){
			fprintf(fp, "2 CONT %s\n", token);
		}
		
	}

	error.type = OK;
	error.line = -1;
	return error;
	
}


GEDCOMerror writeIndividual(FILE * fp, Individual* individual,GEDCOMreference ** referenceArray, int referenceCount){

	GEDCOMerror error;
	Node* ptr;
	char* xref = malloc(sizeof(char)*10);

	if(individual == NULL || fp == NULL){
		error.type = WRITE_ERROR;
		error.line = -1;
		return error;

	}


	for(int i =0; i<referenceCount; i++){
		if(referenceArray[i]->data == individual){
			strcpy(xref, referenceArray[i]->ref_ID);
		}


	}


	char* tempString = NULL;
							
	fprintf(fp, "0 %s INDI\n", xref);
	tempString = individual->givenName;		
	
	if(tempString!=NULL){			

		fprintf(fp, "1 NAME ");
		fprintf(fp,"%s",tempString);
		
	}

	tempString = individual->surname;		
	
	if(tempString!=NULL && strcmp(tempString,"")!=0){			
		fprintf(fp," /%s/",tempString);
		
	}
	fprintf(fp, "\n");

	tempString = individual->givenName;		

	if(tempString!=NULL && strcmp(tempString,"")!=0){			
		fprintf(fp,"2 GIVN %s\n",tempString);
		
	}
	tempString = individual->surname;		

	if(tempString!=NULL && strcmp(tempString,"")!=0){			
		fprintf(fp,"2 SURN %s\n",tempString);
		
	}

	for(ptr = (individual->otherFields).head; ptr!= NULL; ptr = ptr->next){


		Field* tempField = (Field*)(ptr->data);
		if(!(strcmp(tempField->tag, "GIVN")==0 ||strcmp(tempField->tag, "SURN")==0)){
			
			fprintf(fp, "1 %s %s\n",tempField->tag, tempField->value );
		}

	}

	Event* tempEvent;

	for(ptr = (individual->events).head; ptr!= NULL; ptr = ptr->next){

		tempEvent = (Event*)(ptr->data);
		error = writeEvent(fp, tempEvent);

		if(error.type!=OK){
			free(xref);
			error.line = -1;
			error.type = WRITE_ERROR;
			return error;			
		}

	}	
	for(ptr = (individual->families).head; ptr!=NULL; ptr= ptr->next){

		for(int i =0; i<referenceCount; i++){
			if(referenceArray[i]->data == (Family*)(ptr->data)){				

				if(isChild((Family*)(ptr->data), individual)){
					fprintf(fp, "1 FAMC %s\n", referenceArray[i]->ref_ID);
					
				}else{
					fprintf(fp, "1 FAMS %s\n", referenceArray[i]->ref_ID);
					

				}
			}

		}

	}

	free(xref);
	error.type = OK;
	error.line = -1;
	return error;

}

GEDCOMerror writeFamily(FILE * fp, Family* family,GEDCOMreference ** referenceArray, int referenceCount){

	GEDCOMerror error;
	Node* ptr;
	char* xref = malloc(sizeof(char)*10);

	if(family == NULL || fp == NULL){
		error.type = WRITE_ERROR;
		error.line = -1;
		return error;

	}


	for(int i =0; i<referenceCount; i++){
		if(referenceArray[i]->data == family){
			strcpy(xref, referenceArray[i]->ref_ID);
		}

	}

						
	fprintf(fp, "0 %s FAM\n", xref);

	free(xref);
	xref = NULL;
	Individual* tempIndi = family->wife;

	if(tempIndi!=NULL){

		for(int i =0; i<referenceCount; i++){
			if(referenceArray[i]->data == tempIndi){
				xref = malloc(sizeof(char)* 10);			
				strcpy(xref, referenceArray[i]->ref_ID);
			}

		}

	}	


	if(xref!=NULL){			

		fprintf(fp, "1 WIFE %s\n", xref);
		free(xref);
	}

	xref = NULL;
	tempIndi = family->husband;
	if(tempIndi!=NULL){

		for(int i =0; i<referenceCount; i++){
			if(referenceArray[i]->data == tempIndi){

				xref = malloc(sizeof(char)* 10);						
				strcpy(xref, referenceArray[i]->ref_ID);
			}

		}

	}	

	if(xref!=NULL){			

		fprintf(fp, "1 HUSB %s\n", xref);
		free(xref);
		xref = malloc(sizeof(char)* 10);
	}

	Event* tempEvent;

	for(ptr = (family->events).head; ptr!= NULL; ptr = ptr->next){

		tempEvent = (Event*)(ptr->data);
		error = writeEvent(fp, tempEvent);

		if(error.type!=OK){
			free(xref);
			error.line = -1;
			error.type = WRITE_ERROR;
			return error;			
		}

	}	
	for(ptr = (family->children).head; ptr!= NULL; ptr = ptr->next){

		tempIndi = (Individual*)(ptr->data);

		for(int i =0; i<referenceCount; i++){
			if(referenceArray[i]->data == tempIndi){
				strcpy(xref, referenceArray[i]->ref_ID);
			}

		}
		if(xref!=NULL){			

		fprintf(fp, "1 CHIL %s\n", xref);
		free(xref);
		xref = malloc(sizeof(char)* 10);
	}

	}


	free(xref);
	error.type = OK;
	error.line = -1;
	return error;

}

GEDCOMerror writeEvent(FILE * fp, Event* event){

	GEDCOMerror error;

	if(event == NULL || fp == NULL){
		error.type = WRITE_ERROR;
		error.line = -1;
		return error;

	}

	char* tempString = NULL;
	tempString = event->type;

					
	if((event->date!=NULL || strcmp(event->date,"")==0)&&(event->place!=NULL || strcmp(event->place,"")==0)){
		fprintf(fp, "1 %s\n", tempString);
	}else{
		fprintf(fp, "1 %s Y\n", tempString);		
	}

	tempString = event->date;		
	
	if(tempString!=NULL && strcmp(tempString,"")!=0){			

		fprintf(fp, "2 DATE ");
		fprintf(fp,"%s\n",tempString);
		
	}
	tempString = event->place;		
	
	if(tempString!=NULL&& strcmp(tempString,"")!=0){			

		fprintf(fp, "2 PLAC ");
		fprintf(fp,"%s\n",tempString);
		
	}
	
	error.type = OK;
	error.line = -1;
	return error;

}
bool isChild(Family* family, Individual* individual){


	for(Node* ptr = (family->children).head; ptr!=NULL; ptr = ptr->next){
		if(compareIndividuals(individual, (Individual*)(ptr->data))==0){
			return true;
		}

	}
	return false;
}
void deleteIndividualList(void * toBeDeleted){

	List* list = (List*)toBeDeleted;
    if (list == NULL){
		return;
	}

	if (list->head == NULL && list->tail == NULL){
		return;
	}

	Node* tmp;
	
	while (list->head != NULL){
		list->deleteData(list->head->data);
		tmp = list->head;
		list->head = list->head->next;
		free(tmp);
	}
	
	list->head = NULL;
	list->tail = NULL;
	free(toBeDeleted);
}
int compareIndividualsLists(const void* first,const void* second){
	return -1;
}

char* printIndividualList(void* toBePrinted){
	

	List* list = (List*)toBePrinted;
	
	if(list == NULL){
		return NULL;
	}

	char* listString = calloc(sizeof(char),2);	

	Node* ptr1 = list->head;
	char* tempString;
	int currentLength = 0;
	strcpy("", listString);
	Individual* individual;
	while(ptr1!=NULL){
		
		individual = (Individual*)(ptr1->data);	
		listString = realloc(listString,sizeof(char)* (currentLength+ 30 ));			
		tempString = printIndividual(individual);
		currentLength  = strlen(listString);
		listString = realloc(listString,sizeof(char)* (currentLength+strlen(tempString) +1));			
		strcat(listString, tempString);
		free(tempString);		
		currentLength = strlen(listString);		
		ptr1 = ptr1->next;
	}
	return listString;

}
void recursivelyAddDescendantsN(List *descendantList,List * currentGeneration ,const Individual* currentPerson, int counter,int  maxGen){

	Family* tempFam;
	Individual* tempIndi;
	Node* ptr1;
	Node* ptr2;

	
	for(ptr1 = (currentPerson->families).head; ptr1!=NULL; ptr1 = ptr1->next){
		
		tempFam = (Family*)ptr1->data;

		if(tempFam->wife == currentPerson|| tempFam->husband == currentPerson){

			for(ptr2 = (tempFam->children).head; ptr2!=NULL; ptr2 = ptr2->next){
				
				tempIndi = (Individual*)(ptr2->data);

				if(!(listContains(currentGeneration, tempIndi))){

					insertSorted(currentGeneration, tempIndi);

				}
			}

		}

	}

	if((counter< maxGen )||(maxGen == 0 && isEmpty(currentGeneration)==false)){		

		counter++;
		List* newGeneration = calloc(sizeof(List),1);
		newGeneration->head = NULL;
		newGeneration->tail = NULL;
		newGeneration->deleteData = dummyDelete;
		newGeneration->compare = compareIndividuals;
		newGeneration->printData = printIndividual;
		newGeneration->length = 0;


		for(ptr1 = currentGeneration->head; ptr1!=NULL; ptr1 = ptr1->next){

			recursivelyAddDescendantsN(descendantList,newGeneration, ptr1->data, counter, maxGen);			

		}

		if(isEmpty(newGeneration)){
			clearList(newGeneration);
			free(newGeneration);
		}else{
			insertSorted(descendantList, newGeneration);
		}
		
	}

}
void recursivelyAddAncestorsN(List *ancestorList,List * currentGeneration ,const Individual* currentPerson, int counter,int  maxGen){

	Family* tempFam;
	Individual* tempIndi;
	Node* ptr1;

	
	for(ptr1 = (currentPerson->families).head; ptr1!=NULL; ptr1 = ptr1->next){
		
		tempFam = (Family*)ptr1->data;

		if(!(tempFam->wife == currentPerson|| tempFam->husband == currentPerson)){
				
			tempIndi = tempFam->wife;

			if(!(listContains(currentGeneration, tempIndi))){

				insertSorted(currentGeneration, tempIndi);

			}
			tempIndi = tempFam->husband;
			
			if(!(listContains(currentGeneration, tempIndi))){

				insertSorted(currentGeneration, tempIndi);

			}


		}

	}

	if((counter< maxGen )||(maxGen == 0 && isEmpty(currentGeneration)==false)){		

		counter++;
		List* newGeneration = calloc(sizeof(List),1);
		newGeneration->head = NULL;
		newGeneration->tail = NULL;
		newGeneration->deleteData = dummyDelete;
		newGeneration->compare = compareIndividuals;
		newGeneration->printData = printIndividual;
		newGeneration->length = 0;


		for(ptr1 = currentGeneration->head; ptr1!=NULL; ptr1 = ptr1->next){

			recursivelyAddAncestorsN(ancestorList,newGeneration, ptr1->data, counter, maxGen);			

		}

		if(isEmpty(newGeneration)){
			clearList(newGeneration);
			free(newGeneration);
		}else{
			insertSorted(ancestorList, newGeneration);
		}
		
	}

}

bool listContains(List* list, void* target ){

	for(Node* ptr = list->head; ptr!=NULL; ptr = ptr->next){
		if(ptr->data == target){
			return true;
		}

	}
	return false;

}
bool isEmpty(List* list){
	if(list == NULL){
		return true;
	}
	if(list->head == NULL || list->tail == NULL){
		return true;
	}
	return false;
}
