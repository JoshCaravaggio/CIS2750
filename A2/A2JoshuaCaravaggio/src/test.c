#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"
int main(int argc,char **argv){



	/**
	//Testing Field helpers	
	Field* testField1 = malloc(sizeof(Field));
	Field* testField2 = malloc(sizeof(Field));
	//Field* testField3 = malloc(sizeof(Field));	
		
	char* testTag1 = malloc(sizeof(char)*31);
	char* testValue1 = malloc(sizeof(char)*120);
	char* testTag2 = malloc(sizeof(char)*31);
	char* testValue2 = malloc(sizeof(char)*120);	
	
	strcpy(testTag1,"TRADE");
	strcpy(testValue1, "Electrician");
	
	strcpy(testTag2,"TRADE");
	strcpy(testValue2, "Engineer");
	testField1->tag = testTag1;
	testField1->value = testValue1;
	testField2->tag = testTag2;
	testField2->value = testValue2;	
	
	//Testing compare for two fields
	printf("The comparison returns : %d\n",compareFields(testField1,testField2));
	//Testing print for field 2
	char* field2Info = printField(testField2);
	printf(field2Info);
	printf("\n");
	free(field2Info);
	deleteField(testField1);
	deleteField(testField2);	
	//deleteField(testField3);
	
	Individual* testIndividual1 = malloc(sizeof(Individual));
	Individual* testIndividual2 = malloc(sizeof(Individual));
	Individual* testIndividual3 = malloc(sizeof(Individual));
	Individual* testIndividual4 = malloc(sizeof(Individual));
	
	char* givenName1 = malloc(sizeof(char)*120);
	char* surname1 = malloc(sizeof(char)*120);
	char* givenName2 = malloc(sizeof(char)*120);
	char* surname2 = malloc(sizeof(char)*120);	
	char* givenName3 = malloc(sizeof(char)*120);
	char* surname3 = malloc(sizeof(char)*120);
	char* givenName4 = malloc(sizeof(char)*120);
	char* surname4 = malloc(sizeof(char)*120);	
		
	strcpy(surname1,"Caravaggio");
	strcpy(givenName1,"Joshua");
	strcpy(surname2,"Heaney");
	strcpy(givenName2,"Megan");	
	strcpy(surname3,"Spencer");
	strcpy(givenName3,"Nick");
	strcpy(surname4,"P");
	strcpy(givenName4,"Danielle");	
	
		
	testIndividual1->surname = surname1;
	testIndividual1->givenName = givenName1;
	testIndividual2->surname = surname2;
	testIndividual2->givenName = givenName2;
	testIndividual3->surname = surname3;
	testIndividual3->givenName = givenName3;
	testIndividual4->surname = surname4;
	testIndividual4->givenName = givenName4;			
	
	//Testing compare for two individuals
	printf("The comparison returns : %d\n",compareIndividuals(testIndividual1,testIndividual2));
	//Testing print for field 2
	char* individual2Info = printIndividual(testIndividual1);
	printf(individual2Info);
	free(individual2Info);
	
	
	//deleteIndividual(testIndividual1);
	//deleteIndividual(testIndividual2);
	//deleteIndividual(testIndividual3);
			
	Family* testFamily1 = malloc(sizeof(Family));
	Family* testFamily2 = malloc(sizeof(Family));	
	testFamily1->children.length = 6;
	testFamily2->children.length = 3;
	//Individual* testIndividual3 = NULL;
	testFamily1->wife= testIndividual2;
	testFamily1->husband = testIndividual1;	
	
	testFamily2->wife= testIndividual4;
	testFamily2->husband = testIndividual3;		
	//Testing compare for two individuals
	printf("The comparison returns : %d\n",compareFamilies(testFamily1,testFamily2));
	//Testing print for field 2
	char* family1Info = printFamily(testFamily2);
	printf(family1Info);
	free(family1Info);
	
	
	deleteFamily(testFamily1);
	deleteFamily(testFamily2);	
	deleteIndividual(testIndividual1);
	deleteIndividual(testIndividual2);
	deleteIndividual(testIndividual3);
	deleteIndividual(testIndividual4);
	
	Event* testEvent1 = malloc(sizeof(Event));
	Event* testEvent2 = malloc(sizeof(Event));
	
	char* testDate1 = malloc(sizeof(char)*31);
	char* testPlace1 = malloc(sizeof(char)*120);
	char* testDate2 = malloc(sizeof(char)*31);
	char* testPlace2 = malloc(sizeof(char)*120);	
	
	
	strcpy(testDate1,"10 NOV 1987");
	strcpy(testPlace1, "Milton, Ontario");
	c
	strcpy(testDate2,"09 JAN 1248");
	strcpy(testPlace2, "England");
	
	testEvent1->date = testDate1;
	testEvent1->place = testPlace1;
	//testEvent1->type = "Type1";
	//testEvent2->type = "Type2";	
	testEvent2->date = testDate2;
	testEvent2->place = testPlace2;		
	
	printf("The comparison returns : %d\n",compareEvents(testEvent1,testEvent2));	
	
	deleteEvent(testEvent1);
	deleteEvent(testEvent2);
		
**/
	GEDCOMobject *testObject = NULL; 
	
	GEDCOMerror error = createGEDCOM("testFiles/shakespeare.ged", &testObject);
	
	char* string = NULL;
	
	string = printGEDCOM(testObject);

	if(string!=NULL)printf("%s\n",string);
	free(string);	
	string = printError(error);
	if(string!=NULL)printf("%s\n",string);
	free(string);

	GEDCOMerror error2 = writeGEDCOM("testFiles/shakespeareWritten.ged", testObject);
	string = printError(error2);
	if(string!=NULL)printf("%s\n",string);
	free(string);

	GEDCOMerror validateError;
	validateError.type = validateGEDCOM(testObject);
	validateError.line = -1;
	string = printError(validateError);
	if(string!=NULL)printf("Validate: %s\n",string);
	free(string);
	Node* ptr = (testObject->individuals).tail;
	ptr = ptr->previous->previous->previous;

	Individual* testIndividual2 = (Individual*)ptr->data;
	

	List descendantList = getAncestorListN(testObject, testIndividual2,2);

	for(Node * famPtr =  descendantList.head; famPtr!=NULL; famPtr = famPtr->next){

		List* testList = (List*)(famPtr->data);

		for(Node* ptr = testList->head; ptr !=NULL; ptr = ptr->next){
			Individual* indi = (Individual*)ptr->data;
			string = indToJSON(indi);
			printf("%s\n",string );
			Individual* tempIndi = JSONtoInd(string);
			free(string);
			if(tempIndi !=NULL){
	
				string = printIndividual(tempIndi);
				printf("%s\n",string );
							
				deleteIndividual(tempIndi);	
			}	
			free(string);
		}
	}

	clearList(&descendantList);
	deleteGEDCOM(testObject);
		
	
	return 0;
}