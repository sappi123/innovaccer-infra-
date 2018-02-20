# innovaccer-infra-
Functionality :

1)This code will classify and move files from ~/Desktop to ~/Documents.

Classification will be done on basis of extension.

Extra Features :

	a) Post cleanup file stucture optimisation of ~/Desktop.

	b) In case of file name involving multiple .'s , the suffix of the last dot will be considered as the extension

2)The top 10 largest file names along with their size (in MB) in /home/* shall be displayed.
	     The underlying algorithm to achieve this runs in linear time and constant space(ignoring the space used by recursive calls)

Precautions while Executing this code :
		   
		   1) Make sure the folders Documents and Desktop are present in ~

		   2) sudo privileges might be required in some cases.
