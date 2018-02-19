




	Hi,

	On downloading my A1 tar file from the dropbox and placing the source and includes
into studentCode and studentInclude, my code appeared to compile up until a multiple definitions
error. This was due to a function called testCompare I had in my GEDCOMparser.c, as there was also
a function called testCompare in the harnesssource file ListTestCases.c. Upon commenting out my
testCompare function, as it was only used as the test predicate compare function for my getDescendents,
the code compiled on the noMachine socx linux serv. Upon running, it returned a score of 78.
I feel that if this compiles similaraly on your end as it should, a regrade is appropriate  because
I could not have known the name of a function in the test harness.

	I have included a verison of my program that only differs from my original in that I commented out
the testCompare function in my parser, but you could comment it out from the version I submitted.


	Thanks, 
		Joshua Caravaggio
