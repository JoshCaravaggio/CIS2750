

/****************************************************************************************/
/***************************************************************************************/
			      CIS2750 Assignment1 Documentation
							 GEDCOM Parser
						  Joshua Caravaggio
							   0963429
/**************************************************************************************/
/*************************************************************************************/


////////////////// Constraints & Limitations ///////////////////


	The parser will handle various forms of invalid input, including invalid or
unreachable files, invalid file formats and various kinds of data that contrary to the
GEDCOM spec. On calling createGEDCOM, the parser will attempt to work its way through 
the file, and if an error is encountered, the memory is free'd and an error is returned.


//////////////////// Program Description /////////////////////


	CREATEgedcom
	


