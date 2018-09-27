/*---------------------------------------------------------------------------------------
 Program name   : homo.c
 
 Version        : 1.3
 
 Author         : Lars S Jermiin
 
 Institutions   : School of Biological Sciences
                  University of Sydney
                  Sydney, NSW 2006, Australia
 
                : CSIRO Land & Water
                  GPO Box 1700
                  Canberra, ACT 2601, Australia
 
 Date begun     : 18 April, 2001
 
 Date modified  : 14 May, 2018
 
 Copyright      : Copyright 2018, CSIRO & The University of Sydney.
 
 Responsibility : The copyright holders take no legal responsibility for the correctness
                  of results obtained using this program.
 
 Summary        : Homo test the assumption that aligned nucleotide or amino acid sequences
                  have evolved under stationary, reversible, and homogeneous conditions.
                  The sequences must be stored in the FASTA format.
 
 Comments       : Still under development.  Things that need to be added:
                   1 - Stuart (1955) test of marginal symmetry
                   2 - Ababneh et al. (2006) test of internal symmetry
                   3 - Bi- and tri-nucleotide patterns -- these may be more relevant than
                       expected because they accommodate site-specific covariance at the
                       nucleotide level.
 
 Nucleotides    : Listed in order A, C, G and T (U).
 
 Amino acids    : Listed in order A, C, D, E, F, G, H, I, K, L, M, N, P, Q, R, S, T, V, W
                  and Y.
 
 Ambiguities    : Ambiguities in DNA (-, ?, N, B, D, H, K, M, R, S, V, W, Y) and protein
                  (-, ?, X, Z) are ignored during the comparison of sequence pairs.
 
 References     : Ababneh F, Jermiin LS, Ma C, Robinson J (2006). Matched-pairs tests of
                  homogeneity with applications to homologous nucleotide sequences.
						Bioinformatics 22, 1225-1231.
 
                : Bowker AH (1948). A test of symmetry in contingency tables. J. Am. Stat.
                  Assoc. 43, 572-574.
 
                : Martin-Fernandéz J-A, Hron K, Templ M, Filzmoser P, Palarea-Albaladejo J
                  (2015). Bayesian-multiplicative treatment of count zeros in compositional
                  data sets. Stat. Model. 15, 134-158.
 
                : Aitchison J (1986). The statistical analysis of compositional data.
                  Chapman and Hall.
 
                : Jermiin LS, Lovell DR, von Haeseler A (2018). Homo 1.3: a program for
                  surveying and visualizing compositional heterogeneity across aligned
                  sequence data. Syst. Biol. (in prep.)
 
 ---------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>



#define FALSE                 0
#define TRUE                  1
#define MINIMUM               0
#define NUCLEOTIDE            0
#define AMINO_ACID            1
#define SHORT_LINE           10
#define SCROLL_LENGTH        40
#define LINE_LENGTH         500
#define SEQUENCE_LENGTH 2000000
#define SQR(a) ((a) * (a))

/*----------------- Declaration of Function Prototypes -------------------*/

void               Buffer_flush(void);
void               Screen_flush(void);
void               Welcome(void);
void               Information(void);
void               Entry_of_file_names(void);
void               Looking_for_infile(void);
void               Looking_for_outfiles(void);
void               Make_temporary_work_file(void);
void               Heterogeneity(int num_of_seq, int character_type, int coding_type);
unsigned int       Bowker_DF(unsigned int temp[20][20]);
int                Specify_sequence_characters(void);
int                Specify_nucleotide_coding(void);
int                Another_analysis(void);
int                ReadAlignedFasta(void);
double             Bowker_Test(unsigned int temp[20][20]);
long double        xChi_Square_Distribution_Tail(long double x, int dof);
long double        xGaussian_Distribution_Tail( long double x );
static long double Sum_Poisson_Terms(long double x, int n);
static long double Sum_Over_Odd_Terms(long double x, int dof);


/*------------------ Declaration of External Variables -------------------*/

char    inName[75], outName[75];
char    mat1Name[75], mat2Name[75],mat3Name[75],mat4Name[75],mat5Name[75],mat6Name[75];
char    tempName[14] = ".temporary.lsj";
FILE    *fopen(), *inFile, *outFile, *tempFile;
FILE    *mat1File, *mat2File, *mat2File, *mat3File, *mat4File, *mat5File;

/*--------------------------- Main Program ---------------------------------*/

int main(void)
{
	int    num_of_seq = -1, coding_type = -1, character_type = -1;
	int    CONTINUE = TRUE;
	
	Welcome();
	while(CONTINUE == TRUE) {
		Entry_of_file_names();
		Looking_for_infile();
		Looking_for_outfiles();
		Make_temporary_work_file();
		character_type = Specify_sequence_characters();
		if(character_type == 0)
			coding_type = Specify_nucleotide_coding();
		num_of_seq = ReadAlignedFasta();
		Heterogeneity(num_of_seq, character_type, coding_type);
		fclose(outFile);
		fclose(mat1File);
		fclose(mat2File);
		fclose(mat3File);
		fclose(mat4File);
		fclose(mat5File);
		fclose(tempFile);
		remove(tempName);
		CONTINUE = Another_analysis();
	}
	return 0;
}

/*---------------------- Declaration of Functions ---------------------------*/

void Buffer_flush()
{
	int  c;
	
	while ((c = getchar()) != '\n')
		;
}


void Screen_flush()
{
	int  c;
	
	for(c = 0; c < SCROLL_LENGTH; c++)
		printf("\n");
}


/*
 Welcome and information for users
 */

void Welcome(void)
{
	int  c, d, counter = 0, SUCCESS = FALSE;
	
	while (SUCCESS == FALSE && counter < 3) {
		++counter;
		printf("\nInformation [Y/N] ? ");
		c = getchar();
		d = getchar();
		if (d != '\n') {
			printf("\n\nERROR - More than one character was entered!\n");
			Buffer_flush();
		}
		else {
			c = toupper(c);
			if (c != 'Y' && c != 'N')
				printf("\n\nERROR - This character is not an option!\n");
			else {
				if (c == 'Y') {
					SUCCESS = TRUE;
					Information();
					Screen_flush();
				}
				else {
					SUCCESS = TRUE;
					Screen_flush();
				}
			}
		}
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		exit(1);
	}
}


/*
 Deals with the question of another analysis
 */

int Another_analysis(void)
{
	int  c, d, counter = 0, SUCCESS = FALSE, finish = 0;
	
	while (SUCCESS == FALSE && counter < 3) {
		++counter;
		printf("\nDo you wish to continue [Y/N] ? ");
		c = getchar();
		d = getchar();
		if (d != '\n') {
			printf("\n\nERROR - More than one character was entered!\n");
			Buffer_flush();
		}
		else {
			c = toupper(c);
			if (c != 'Y' && c != 'N')
				printf("\n\nERROR - This character is not an option!\n");
			else {
				if (c == 'Y') {
					SUCCESS = TRUE;
					finish = 1;
					Screen_flush();
				}
				else {
					SUCCESS = TRUE;
					finish = 0;
					Screen_flush();
				}
			}
		}
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		exit(1);
	}
	return(finish);
}


/*
 Information and instructions provided
 */

void Information()
{
	printf("\nProgram   : Homo - Version 1.3");
	printf("\n");
	printf("\nPurpose   : Homo is designed to allow users to analyse compositional");
	printf("\n            heterogeneity across aligned molecular sequence data.\n");
	printf("\nCopyright : 2017, CSIRO and the University of Sydney.\n");
	printf("\nPermission is granted to use and distribute this program, provided the");
	printf("\nthe licence to use the program is not violated.  The copyright holders");
	printf("\ntake no legal responsibility for correctness of results obtained using");
	printf("\nthis program.");
	printf("\n");
	printf("\nFor further information, consult the manual and/or contact the author.");
	printf("\n");
	printf("\nAuthor    : Lars S. Jermiin");
	printf("\nAddress   : Research School of Biology");
    printf("\n            Australian National University");
	printf("\n            Canberra, ACT 2600");
	printf("\n            Australia.");
	printf("\nPhone     : +61 (02) 6125 4720");
	printf("\nE-mail    : lars.jermiin@anu.edu.au");
	printf("\n\nType RETURN to continue...");
	getchar();
}


/*
 Entry of filenames
 */

void Entry_of_file_names(void)
{
	int  counter = 0, SUCCESS = FALSE;
	
	while (counter < 3 && SUCCESS == FALSE) {
		counter++;
		printf("\nInput file (sequences in FASTA format)            : ");
		scanf("%74s",inName);
		Buffer_flush();
		printf("\nList the first part of the output files' names    : ");
		scanf("%74s",outName);
		Buffer_flush();
		if (strcmp(inName,outName)) {
			mat1Name[0] = '\0';
			mat2Name[0] = '\0';
			mat3Name[0] = '\0';
			mat4Name[0] = '\0';
			mat5Name[0] = '\0';
			mat6Name[0] = '\0';
			strcat(mat1Name, outName);
			strcat(mat1Name, "_Pvalues.csv\0");
			strcat(mat2Name, outName);
			strcat(mat2Name, "_EMS.txt\0");
			strcat(mat3Name, outName);
			strcat(mat3Name, "_EFS.txt\0");
			strcat(mat4Name, outName);
			strcat(mat4Name, "_AMS.txt\0");
			strcat(mat5Name, outName);
			strcat(mat5Name, "_AFS.txt\0");
			strcat(outName, "_summary.csv\0");
			SUCCESS = TRUE;	
		}
		else {
			printf("\n\nERROR - The two file names are identical.\n");
			Screen_flush();
		}
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		exit(1);
	}
}


/*
 Specify the sequence characters
 */

int Specify_sequence_characters(void)
{
	int  c, d, counter, SUCCESS;
	int  e = -1;
	
	counter = 0;
	SUCCESS = FALSE;
	while (SUCCESS == FALSE && counter < 3) {
		++counter;
		printf("\nDo you analyse nucleotides or amino acids (N/A)?  : ");
		c = getchar();
		d = getchar();
		if (d != '\n') {
			printf("\n\nERROR - This character is not an option!\n");
			Buffer_flush();
		}
		else {
			c = toupper(c);
			if (c != 'A' && c != 'N')
				printf("\n\nERROR - This character is not an option!\n");
			else {
				if (c == 'N') {
					SUCCESS = TRUE;
					e = 0;
				}
				else {
					SUCCESS = TRUE;
					e = 1;
					Screen_flush();
				}
			}
		}
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		exit(1);
	}
	return(e);
}


/*
 Specify the type of coding of nucleotide sequences
 */

int Specify_nucleotide_coding(void)
{
	int  c, d, counter, SUCCESS;
	
	counter = 0;
	SUCCESS = FALSE;
	while (SUCCESS == FALSE && counter < 3) {
		++counter;
		printf("\nThe following options are then available:");
		printf("\n\n   Normal coding (A, C, G, T) ..................... A");
		printf("\n   CTR coding (C, T, AG) .......................... B");
		printf("\n   AGY coding (A, G, CT) .......................... C");
		printf("\n   ATS coding (A, T, CG) .......................... D");
		printf("\n   CGW coding (C, G, AT) .......................... E");
		printf("\n   ACK coding (A, C, GT) .......................... F");
		printf("\n   GTM coding (G, T, AC) .......................... G");
		printf("\n   KM coding (GT, AC) ............................. H");
		printf("\n   SW coding (GC, AT) ............................. I");
		printf("\n   RY coding (AG, CT) ............................. J");
		printf("\n   AB coding (A, CGT) ............................. K");
		printf("\n   CD coding (C, AGT) ............................. L");
		printf("\n   GH coding (G, ACT) ............................. M");
		printf("\n   TV coding (T, ACG) ............................. N");
		printf("\n\nYour choice ....................................... ");
		c = getchar();
		d = getchar();
		if (d != '\n') {
			printf("\n\nERROR - This character is not an option!\n");
			Buffer_flush();
		}
		else {
			c = toupper(c);
			if (c != 'A' && c != 'B' && c != 'C' && c != 'D' && c != 'E' &&
				 c != 'F' && c != 'G' && c != 'H' && c != 'I' && c != 'J' &&
				 c != 'K' && c != 'L' && c != 'M' && c != 'N')
				printf("\n\nERROR - This character is not an option!\n");
			else {
				SUCCESS = TRUE;
				Screen_flush();
			}
		}
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		exit(1);
	}
	return(c - 'A');
}

/*
 Looking for the infile
 */

void Looking_for_infile(void)
{
	int  counter = 0, SUCCESS = FALSE;
	
	while (counter < 3 && SUCCESS == FALSE) {
		if ((inFile = fopen(inName, "r")) == NULL) {
			++counter;
			printf("\n\nERROR - Could not open file named %s.\n",inName);
			printf("\nEnter name of file with sequence data  : ");
			scanf("%74s",inName);
			Buffer_flush();
			Screen_flush();
		}
		else
			SUCCESS = TRUE;
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		exit(1);
	}
}


/*
 Looking for the outfiles
 */

void Looking_for_outfiles(void)
{
	int  counter = 0, SUCCESS = FALSE;
	
	while (counter < 3 && SUCCESS == FALSE) {
		if ((outFile = fopen(outName, "w")) == NULL) {
			++counter;
			printf("\n\nERROR - Could not open file named %s.\n",outName);
			printf("\nName of output file with test results : ");
			scanf("%74s",outName);
			Buffer_flush();
			Screen_flush();
		}
		else
			SUCCESS = TRUE;
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		fclose(inFile);
		exit(1);
	}
	SUCCESS = FALSE;
	counter = 0;
	while (counter < 3 && SUCCESS == FALSE) {
		if ((mat1File = fopen(mat1Name, "w")) == NULL) {
			++counter;
			printf("\n\nERROR - Could not open file named %s.\n",mat1Name);
			printf("\nName of output file with distances    : ");
			scanf("%74s",mat1Name);
			Buffer_flush();
			Screen_flush();
		}
		else
			SUCCESS = TRUE;
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		fclose(inFile);
		fclose(outFile);
		exit(1);
	}

	SUCCESS = FALSE;
	counter = 0;
	while (counter < 3 && SUCCESS == FALSE) {
		if ((mat2File = fopen(mat2Name, "w")) == NULL) {
			++counter;
			printf("\n\nERROR - Could not open file named %s.\n",mat2Name);
			printf("\nName of output file with distances    : ");
			scanf("%74s",mat2Name);
			Buffer_flush();
			Screen_flush();
		}
		else
			SUCCESS = TRUE;
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		fclose(inFile);
		fclose(outFile);
		fclose(mat1File);
		exit(1);
	}
	SUCCESS = FALSE;
	counter = 0;
	while (counter < 3 && SUCCESS == FALSE) {
		if ((mat3File = fopen(mat3Name, "w")) == NULL) {
			++counter;
			printf("\n\nERROR - Could not open file named %s.\n",mat3Name);
			printf("\nName of output file with distances    : ");
			scanf("%74s",mat3Name);
			Buffer_flush();
			Screen_flush();
		}
		else
			SUCCESS = TRUE;
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		fclose(inFile);
		fclose(outFile);
		fclose(mat1File);
		fclose(mat2File);
		exit(1);
	}
	SUCCESS = FALSE;
	counter = 0;
	while (counter < 3 && SUCCESS == FALSE) {
		if ((mat4File = fopen(mat4Name, "w")) == NULL) {
			++counter;
			printf("\n\nERROR - Could not open file named %s.\n",mat4Name);
			printf("\nName of output file with distances    : ");
			scanf("%74s",mat4Name);
			Buffer_flush();
			Screen_flush();
		}
		else
			SUCCESS = TRUE;
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		fclose(inFile);
		fclose(outFile);
		fclose(mat1File);
		fclose(mat2File);
		fclose(mat3File);
		exit(1);
	}
	SUCCESS = FALSE;
	counter = 0;
	while (counter < 3 && SUCCESS == FALSE) {
		if ((mat5File = fopen(mat5Name, "w")) == NULL) {
			++counter;
			printf("\n\nERROR - Could not open file named %s.\n",mat5Name);
			printf("\nName of output file with distances    : ");
			scanf("%74s",mat5Name);
			Buffer_flush();
			Screen_flush();
		}
		else
			SUCCESS = TRUE;
	}
	if (SUCCESS == FALSE) {
		printf("\n\nSorry - Program aborted\n\n");
		fclose(inFile);
		fclose(outFile);
		fclose(mat1File);
		fclose(mat2File);
		fclose(mat3File);
		fclose(mat4File);
		exit(1);
	}
}


/*
 Opening the temporary file
 */

void Make_temporary_work_file(void)
{
	if ((tempFile = fopen(tempName, "w")) == NULL) {
		printf("\n\nSorry - program aborted because it could not");
		printf("\ncreate temporary file named %s.\n\n",tempName);
		fclose(inFile);
		fclose(outFile);
		fclose(mat1File);
		fclose(mat2File);
		fclose(mat3File);
		fclose(mat4File);
		fclose(mat5File);
		exit(1);
	}
}


int ReadAlignedFasta(void)
{
#define  IN_SEQUENCE   1                  /*Inside nucleotide sequence*/
#define  IN_NAME       0                        /*Inside sequence name*/

	int  c;                                  /*Character read from file*/
	int  h = 0;         /*Counter of the number of characters in a name*/
	int  i = 0;            /*Counter of the number of sequences in file*/
	int  j = 0;                /*Counter of the number of sites in file*/
	int  seqNum = 0;                      /*Number of sequences in file*/
	int  seqLen = 0;                 /*Number of sites in each sequence*/
	int  SUCCESS = TRUE;                             /*Boolean variable*/
	int  WHERE = IN_NAME;                            /*Boolean variable*/
/*
 Read infile and count the number of sequences
 */
	while (!feof(inFile)) {
		c = fgetc(inFile);
		if (c == '>') {
			seqNum++;
		}
	}
	if (seqNum == 0) {
		printf("\n\nSorry - program aborted because it could not");
		printf("\nfind the expected '>' character in %s.\n\n",inName);
		SUCCESS = FALSE;
		fclose(inFile);
		fclose(outFile);
		fclose(mat1File);
		fclose(mat2File);
		fclose(mat3File);
		fclose(mat4File);
		fclose(mat5File);
		fclose(tempFile);
		exit(1);
	}

/*
 Read infile and count the number of sites
 */

	if(SUCCESS == TRUE) {
		rewind(inFile);
		WHERE = IN_SEQUENCE;
		i = 0;
		do {
			c = fgetc(inFile);
			if (WHERE == IN_SEQUENCE && c == '>') {
				WHERE = IN_NAME;
				h = 0;
				if (i == 1) {
					seqLen = j;
					i++;
				} else {
					if (i > 1 && j != seqLen) {
						printf("\n\nSorry - program aborted because sequences 1 and");
						printf("\n%d have different numbers of sites.\n\n",i);
						SUCCESS = FALSE;
						fclose(inFile);
						fclose(outFile);
						fclose(mat1File);
						fclose(mat2File);
						fclose(mat3File);
						fclose(mat4File);
						fclose(mat5File);
						fclose(tempFile);
						exit(1);
					}
					i++;
				}
			}
			if (WHERE == IN_NAME && c != '\n') {
				h++;
			}
			if (WHERE == IN_NAME && c == '\n') {
				if (h == 0) {
					printf("\n\nSorry - program aborted because sequence %d",i);
					printf("\nhas no name.\n\n");
					SUCCESS = FALSE;
					fclose(inFile);
					fclose(outFile);
					fclose(mat1File);
					fclose(mat2File);
					fclose(mat3File);
					fclose(mat4File);
					fclose(mat5File);
					fclose(tempFile);
					exit(1);
				} else {
					WHERE = IN_SEQUENCE;
					j = 0;
				}
			}
			if (WHERE == IN_SEQUENCE && c != '>') {
				if (toupper(c) == 'A') j++;
				if (toupper(c) == 'B') j++;
				if (toupper(c) == 'C') j++;
				if (toupper(c) == 'D') j++;
				if (toupper(c) == 'E') j++;
				if (toupper(c) == 'F') j++;
				if (toupper(c) == 'G') j++;
				if (toupper(c) == 'H') j++;
				if (toupper(c) == 'I') j++;
				if (toupper(c) == 'K') j++;
				if (toupper(c) == 'L') j++;
				if (toupper(c) == 'M') j++;
				if (toupper(c) == 'N') j++;
				if (toupper(c) == 'P') j++;
				if (toupper(c) == 'Q') j++;
				if (toupper(c) == 'R') j++;
				if (toupper(c) == 'S') j++;
				if (toupper(c) == 'T') j++;
				if (toupper(c) == 'U') j++;
				if (toupper(c) == 'V') j++;
				if (toupper(c) == 'W') j++;
				if (toupper(c) == 'X') j++;
				if (toupper(c) == 'Y') j++;
				if (toupper(c) == 'Z') j++;
				if (toupper(c) == '-') j++;
				if (toupper(c) == '?') j++;
			}
		} while(SUCCESS == TRUE && !feof(inFile));
	}
/*
 The sequences are now being converted to a format used in this program
 */

	if(SUCCESS == TRUE) {
		rewind(inFile);
		rewind(tempFile);
		WHERE = IN_SEQUENCE;
		i = 0;
		do {
			c = fgetc(inFile);
			if (WHERE == IN_SEQUENCE && c == '>') {
				if (i == 0) {
					fprintf(tempFile,"%d\n",seqLen);
					WHERE = IN_NAME;
					i++;
				} else {
					fprintf(tempFile,"\n%d\n",seqLen);
					WHERE = IN_NAME;
					i++;
				}
			}
			if (WHERE == IN_NAME && c != '\n') {
				if (c != '>') {
					fprintf(tempFile,"%c",c);
				}
			}
			if (WHERE == IN_NAME && c == '\n') {
				fprintf(tempFile,"\n");
				WHERE = IN_SEQUENCE;
			}
			if (WHERE == IN_SEQUENCE && c != '>') {
				if (toupper(c) == 'A') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'B') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'C') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'D') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'E') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'F') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'G') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'H') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'I') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'K') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'L') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'M') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'N') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'P') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'Q') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'R') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'S') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'T') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'U') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'V') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'W') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'X') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'Y') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == 'Z') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == '-') fprintf(tempFile,"%c",toupper(c));
				if (toupper(c) == '?') fprintf(tempFile,"%c",toupper(c));
			}
		} while (SUCCESS == TRUE && !feof(inFile));
	}
	fclose(inFile);
	fclose(tempFile);
	printf("\n\n1 - Input file read successfully...");
	return((int) seqNum);
}


/*
 Heterogeneity reads the file temporary.lsj and do all possible
 comparisons of sequence pairs using a set of functions.
 */

void Heterogeneity(int num_of_seq, int character_type, int coding_type)
{
	unsigned int  i, j, k = 0, l, m, n, varU, varV, sizeYZ = 0, sizeUV = 0;
	unsigned int  h, df = 0, length_1, length_2, sum, count, counter = 0;
	unsigned int  zeroU, zeroV, zeroY, zeroZ;
	unsigned long sig_A = 0, sig_B = 0, sig_C = 0, sig_D = 0;
	unsigned long sig_E = 0, sig_F = 0, sig_G = 0, sig_H = 0;
    unsigned long total;
    double        bowker = 0.0;
	double        MIN_Q = 1.0;
	double        d_Eucl_Marg = 0.0, d_Eucl_Full = 0.0;
	double        d_Aitc_Marg = 0.0, d_Aitc_Full = 0.0;
	double        zeroAdj;
	long double   Q = 1.0L;
	char          a, b, c;
	static char   sequence_1[SEQUENCE_LENGTH+1];      /*Array containing sequence 1*/
	static char   sequence_2[SEQUENCE_LENGTH+1];      /*Array containing sequence 2*/
	static char   seq_name_1[LINE_LENGTH+1];  /*Array containing name of sequence 1*/
	static char   seq_name_2[LINE_LENGTH+1];  /*Array containing name of sequence 2*/
	static char   seq_len_1[SHORT_LINE+1];  /*Array containing length of sequence 1*/
	static char   seq_len_2[SHORT_LINE+1];  /*Array containing length of sequence 2*/
	unsigned int  dm[20][20];                     /*Divergence matrix with integers*/
	double        vectorU[20], vectorV[20];                 /*Compositional vectors*/
	double        vectorY[190], vectorZ[190];               /*Compositional vectors*/
	
	if ((tempFile = fopen(tempName, "r")) == NULL) {
		printf("\n\nERROR - Could not open file named %s.\n",tempName);
		printf("\n\nSorry - Program aborted\n\n");
		exit(1);
	}
	printf("\n\n3 - Statistical analysis in progress...");
	
    if(num_of_seq % 2 == 0){
        total = (unsigned long)(num_of_seq/2)*(num_of_seq-1);
    }else{
        total = (unsigned long)((num_of_seq-1)/2)*(num_of_seq);
    }

/*
 The first part checks that the data are still in the correct format
 */
	
	rewind(tempFile);
	for(i = 0; i < num_of_seq; i++){
		if(i == 0) {
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < SHORT_LINE)
				seq_len_1[h++] = c;
			seq_len_1[h] = '\0';
			if(1 != sscanf(seq_len_1,"%d", &length_1)) {
				printf("\nExpected the length for sequence %d but found none.", i + 1);
				printf("\nPlease check file named %s.\n", tempName);
				printf("\n\nSorry - Program aborted\n\n");
				fclose(outFile);
				fclose(mat1File);
				fclose(mat2File);
				fclose(mat3File);
				fclose(mat4File);
				fclose(mat5File);
				fclose(tempFile);
				exit(1);
			}
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < LINE_LENGTH)
				seq_name_1[h++] = c;
			seq_name_1[h] = '\0';
			if(h <= 1) {
				printf("\nExpected a name for sequence %d but found none.", i + 1);
				printf("\nPlease check file named %s.\n", tempName);
				printf("\n\nSorry - Program aborted\n\n");
				fclose(outFile);
				fclose(mat1File);
				fclose(mat2File);
				fclose(mat3File);
				fclose(mat4File);
				fclose(mat5File);
				fclose(tempFile);
				exit(1);
			}
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < SEQUENCE_LENGTH)
				sequence_1[h++] = c;
			sequence_1[h] = '\0';
			if(h <= 1) {
				printf("\nExpected sequence %d but found none.", i + 1);
				printf("\nPlease check file named %s.\n", tempName);
				printf("\n\nSorry - Program aborted\n\n");
				fclose(outFile);
				fclose(mat1File);
				fclose(mat2File);
				fclose(mat3File);
				fclose(mat4File);
				fclose(mat5File);
				fclose(tempFile);
				exit(1);
			}
		}
		if(i > 0) {
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < SHORT_LINE)
				seq_len_2[h++] = c;
			seq_len_2[h] = '\0';
			if(1 != sscanf(seq_len_2,"%d", &length_2)) {
				printf("\nExpected the length for sequence %d but found none.", i + 1);
				printf("\nPlease check file named %s.\n", tempName);
				printf("\n\nSorry - Program aborted\n\n");
				fclose(outFile);
				fclose(mat1File);
				fclose(mat2File);
				fclose(mat3File);
				fclose(mat4File);
				fclose(mat5File);
				fclose(tempFile);
				exit(1);
			}
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < LINE_LENGTH)
				seq_name_2[h++] = c;
			seq_name_2[h] = '\0';
			if(h <= 1) {
				printf("\nExpected a name for sequence %d but found none.", i + 1);
				printf("\nPlease check file named %s.\n", tempName);
				printf("\n\nSorry - Program aborted\n\n");
				fclose(outFile);
				fclose(mat1File);
				fclose(mat2File);
				fclose(mat3File);
				fclose(mat4File);
				fclose(mat5File);
				fclose(tempFile);
				exit(1);
			}
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < SEQUENCE_LENGTH)
				sequence_2[h++] = c;
			sequence_2[h] = '\0';
			if(h <= 1) {
				printf("\nExpected sequence %d but found none.", i + 1);
				printf("\nPlease check file named %s.\n", tempName);
				printf("\n\nSorry - Program aborted\n\n");
				fclose(outFile);
				fclose(mat1File);
				fclose(mat2File);
				fclose(mat3File);
				fclose(mat4File);
				fclose(mat5File);
				fclose(tempFile);
				exit(1);
			}
		}
	}
	
/*
 Print information to outfile and statfile
 */
	
	fprintf(outFile,"Program,Homo\n");
	fprintf(outFile,"Version,1.3\n");
	fprintf(outFile,"Input file,%s\n",inName);
	fprintf(outFile,"Data,");
	if (character_type == 1) {
		fprintf(outFile,"Amino acids\n");
	} else {
		fprintf(outFile,"Nucleotides\n");
		fprintf(outFile,"Characters used:,");
		if(coding_type == 0) fprintf(outFile,"ACGT (A|C|G|T)\n");
		if(coding_type == 1) fprintf(outFile,"CTR (C|T|AG)\n");
		if(coding_type == 2) fprintf(outFile,"AGY (A|G|CT)\n");
		if(coding_type == 3) fprintf(outFile,"ATS (A|T|CG)\n");
		if(coding_type == 4) fprintf(outFile,"CGW (C|G|AT)\n");
		if(coding_type == 5) fprintf(outFile,"ACK (A|C|GT)\n");
		if(coding_type == 6) fprintf(outFile,"GTM (G|T|AC)\n");
		if(coding_type == 7) fprintf(outFile,"KM (GT|AC)\n");
		if(coding_type == 8) fprintf(outFile,"SW (GC|AT)\n");
		if(coding_type == 9) fprintf(outFile,"RY (AG|CT)\n");
		if(coding_type == 10) fprintf(outFile,"AB (A|CGT)\n");
		if(coding_type == 11) fprintf(outFile,"CD (C|AGT)\n");
		if(coding_type == 12) fprintf(outFile,"GH (G|ACT)\n");
		if(coding_type == 13) fprintf(outFile,"TV (T|ACG)\n");
	}
	fprintf(outFile,"Sequences,%d\n",num_of_seq);
	fprintf(outFile,"\n\nSeq. 1.,Seq. 2.,Chi-square,df,p,d_EMS,d_EFS,d_AMS,d_AFS,Sites\n\n");
	fprintf(mat1File,"%d\n",num_of_seq);
	fprintf(mat2File,"%d\n",num_of_seq);
	fprintf(mat3File,"%d\n",num_of_seq);
	fprintf(mat4File,"%d\n",num_of_seq);
	fprintf(mat5File,"%d\n",num_of_seq);

/*
 This is where the actual comparisons and estimations take place
 */
	
	for(i = 0; i < num_of_seq; i++) {
		rewind(tempFile);
		k = 0;
		
/*
 The next if-statements brings us to the correct line in tempfile1
 and allow us to read the correct three lines with sequence length,
 sequence name and finally the sequence itself (sequence 1)
 */
		
		if(k < (3 * i)) {
			do {
				while((c = fgetc(tempFile)) != '\n')
					;
				k++;
			} while (k < (3 * i));
		}
		if(k == (3 * i)) {
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < SHORT_LINE)
				seq_len_1[h++] = c;
			seq_len_1[h] = '\0';
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < LINE_LENGTH)
				seq_name_1[h++] = c;
			seq_name_1[h] = '\0';
			h = 0;
			while((c = fgetc(tempFile)) != '\n' && h < SEQUENCE_LENGTH)
				sequence_1[h++] = c;
			sequence_1[h] = '\0';
			k = k + 3;
		}
		
/*
 Print sequence names to the five matrix files
 */
		fprintf(mat1File,"%s",seq_name_1);
		h = 0;
		do {
			c = seq_name_1[h];
			if (c != '\0') {
				fprintf(mat2File,"%c",seq_name_1[h]);
				fprintf(mat3File,"%c",seq_name_1[h]);
				fprintf(mat4File,"%c",seq_name_1[h]);
				fprintf(mat5File,"%c",seq_name_1[h]);
			}
			h++;
		} while (c != '\0' && h < 9);
		fprintf(mat1File,",");
		if (h < 10) {
			do {
				fprintf(mat2File," ");
				fprintf(mat3File," ");
				fprintf(mat4File," ");
				fprintf(mat5File," ");
				h++;
			} while (h <= 10);
		}

		for(j = 0; j < num_of_seq; j++) {
			rewind(tempFile);
			l = 0;
			
/*
 The next if-statements brings us to the correct line in tempfile1
 and allow us to read the correct three lines with sequence length,
 sequence name and finally the sequence itself (sequence 2)
 */
			
			if(l < (3 * j)) {
				do {
					while((c = fgetc(tempFile)) != '\n')
						;
					l++;
				} while (l < (3 * j));
			}
			if(l == (3 * j)) {
				h = 0;
				while((c = fgetc(tempFile)) != '\n' && h < SHORT_LINE)
					seq_len_2[h++] = c;
				seq_len_2[h] = '\0';
				h = 0;
				while((c = fgetc(tempFile)) != '\n' && h < LINE_LENGTH)
					seq_name_2[h++] = c;
				seq_name_2[h] = '\0';
				h = 0;
				while((c = fgetc(tempFile)) != '\n' && h < SEQUENCE_LENGTH)
					sequence_2[h++] = c;
				sequence_2[h] = '\0';
				l = l + 3;
			}

//			printf("\ni = %d, %s, j = %d,  %s,",i,seq_name_1,j,seq_name_2);
/*
 Prepare divergence matrix with zero in all cells
 */
			for(m = 0; m < 20; m++) {
				for(n = 0; n < 20; n++) {
					dm[m][n] = 0;
				}
			}
			
/*
 Here the summation of characters into a 20 x 20 matrix starts. Note
 that ambiguous characters are accounted for in a proper manner.
 */

			if(character_type == NUCLEOTIDE) {
				for(h = 0; h < length_1; h++) {       /*requires that length_1 = length_2*/
					a = sequence_1[h];
					b = sequence_2[h];
					if((a == 'A') && (b == 'A')) ++dm[0][0];
					if((a == 'A') && (b == 'C')) ++dm[0][1];
					if((a == 'A') && (b == 'G')) ++dm[0][2];
					if((a == 'A') && (b == 'T')) ++dm[0][3];
					if((a == 'A') && (b == 'U')) ++dm[0][3];
					if((a == 'C') && (b == 'A')) ++dm[1][0];
					if((a == 'C') && (b == 'C')) ++dm[1][1];
					if((a == 'C') && (b == 'G')) ++dm[1][2];
					if((a == 'C') && (b == 'T')) ++dm[1][3];
					if((a == 'C') && (b == 'U')) ++dm[1][3];
					if((a == 'G') && (b == 'A')) ++dm[2][0];
					if((a == 'G') && (b == 'C')) ++dm[2][1];
					if((a == 'G') && (b == 'G')) ++dm[2][2];
					if((a == 'G') && (b == 'T')) ++dm[2][3];
					if((a == 'G') && (b == 'U')) ++dm[2][3];
					if((a == 'T') && (b == 'A')) ++dm[3][0];
					if((a == 'T') && (b == 'C')) ++dm[3][1];
					if((a == 'T') && (b == 'G')) ++dm[3][2];
					if((a == 'T') && (b == 'T')) ++dm[3][3];
					if((a == 'T') && (b == 'U')) ++dm[3][3];
					if((a == 'U') && (b == 'A')) ++dm[3][0];
					if((a == 'U') && (b == 'C')) ++dm[3][1];
					if((a == 'U') && (b == 'G')) ++dm[3][2];
					if((a == 'U') && (b == 'T')) ++dm[3][3];
					if((a == 'U') && (b == 'U')) ++dm[3][3];
				}
				if(coding_type == 0)
					;
				if(coding_type == 1) {
/*
 pools A and G
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][2];
						dm[n][2] = dm[n][3];
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[2][m];
						dm[2][m] = dm[3][m];
						dm[3][m] = 0;
					}
				}
				if(coding_type == 2) {
/*
 pools C and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][1] = dm[n][1] + dm[n][3];
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[1][m] = dm[1][m] + dm[3][m];
						dm[3][m] = 0;
					}
				}
				if(coding_type == 3) {
/*
 pools C and G
 */
					for(n = 0; n < 4; n++) {
						dm[n][1] = dm[n][1] + dm[n][2];
						dm[n][2] = dm[n][3];
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[1][m] = dm[1][m] + dm[2][m];
						dm[2][m] = dm[3][m];
						dm[3][m] = 0;
					}
				}
				if(coding_type == 4) {
/*
 pools A and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][3];
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[3][m];
						dm[3][m] = 0;
					}
				}
				if(coding_type == 5) {
/*
 pools G and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][2] = dm[n][2] + dm[n][3];
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[2][m] = dm[2][m] + dm[3][m];
						dm[3][m] = 0;
					}
				}
				if(coding_type == 6) {
/*
 pools A and C
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][1];
						dm[n][1] = dm[n][3];
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[1][m];
						dm[1][m] = dm[3][m];
						dm[3][m] = 0;
					}
				}
				if(coding_type == 7) {
/*
 pools A and C as well as G and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][1];
						dm[n][1] = dm[n][2] + dm[n][3];
						dm[n][2] = 0;
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[1][m];
						dm[1][m] = dm[2][m] + dm[3][m];
						dm[2][m] = 0;
						dm[3][m] = 0;
					}
				}
				if(coding_type == 8) {
/*
 pools A and T as well as G and C
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][3];
						dm[n][1] = dm[n][1] + dm[n][2];
						dm[n][2] = 0;
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[3][m];
						dm[1][m] = dm[1][m] + dm[2][m];
						dm[2][m] = 0;
						dm[3][m] = 0;
					}
				}
				if(coding_type == 9) {
/*
 pools A and G as well as C and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][2];
						dm[n][1] = dm[n][1] + dm[n][3];
						dm[n][2] = 0;
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[2][m];
						dm[1][m] = dm[1][m] + dm[3][m];
						dm[2][m] = 0;
						dm[3][m] = 0;
					}
				}
				if(coding_type == 10) {
/*
 pools C, G and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][1] = dm[n][1] + dm[n][2] + dm[n][3];
						dm[n][2] = 0;
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[1][m] = dm[1][m] + dm[2][m] + dm[3][m];
						dm[2][m] = 0;
						dm[3][m] = 0;
					}
				}
				if(coding_type == 11) {
/*
 pools A, G and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][2] + dm[n][3];
						dm[n][2] = 0;
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[2][m] + dm[3][m];
						dm[2][m] = 0;
						dm[3][m] = 0;
					}
				}
				if(coding_type == 12) {
/*
 pools A, C and T
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][1] + dm[n][3];
						dm[n][1] = dm[n][2];
						dm[n][2] = 0;
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[1][m] + dm[3][m];
						dm[1][m] = dm[2][m];
						dm[2][m] = 0;
						dm[3][m] = 0;
					}
				}
				if(coding_type == 13) {
/*
 pools A, C and G
 */
					for(n = 0; n < 4; n++) {
						dm[n][0] = dm[n][0] + dm[n][1] + dm[n][2];
						dm[n][1] = dm[n][3];
						dm[n][2] = 0;
						dm[n][3] = 0;
					}
					for(m = 0; m < 4; m++) {
						dm[0][m] = dm[0][m] + dm[1][m] + dm[2][m];
						dm[1][m] = dm[3][m];
						dm[2][m] = 0;
						dm[3][m] = 0;
					}
				}
			}
			
/*
 Here starts the summation of characters into a 20 x 20 matrix. Note
 that ambiguous characters are accounted for in a proper manner.
*/
			
			if(character_type == AMINO_ACID) {
				for(h = 0; h < length_1; h++) {       /*assumes that length_1 = length_2*/
					a = sequence_1[h];
					b = sequence_2[h];
					if((a == 'A') && (b == 'A')) ++dm[0][0];
					if((a == 'A') && (b == 'C')) ++dm[0][1];
					if((a == 'A') && (b == 'D')) ++dm[0][2];
					if((a == 'A') && (b == 'E')) ++dm[0][3];
					if((a == 'A') && (b == 'F')) ++dm[0][4];
					if((a == 'A') && (b == 'G')) ++dm[0][5];
					if((a == 'A') && (b == 'H')) ++dm[0][6];
					if((a == 'A') && (b == 'I')) ++dm[0][7];
					if((a == 'A') && (b == 'K')) ++dm[0][8];
					if((a == 'A') && (b == 'L')) ++dm[0][9];
					if((a == 'A') && (b == 'M')) ++dm[0][10];
					if((a == 'A') && (b == 'N')) ++dm[0][11];
					if((a == 'A') && (b == 'P')) ++dm[0][12];
					if((a == 'A') && (b == 'Q')) ++dm[0][13];
					if((a == 'A') && (b == 'R')) ++dm[0][14];
					if((a == 'A') && (b == 'S')) ++dm[0][15];
					if((a == 'A') && (b == 'T')) ++dm[0][16];
					if((a == 'A') && (b == 'V')) ++dm[0][17];
					if((a == 'A') && (b == 'W')) ++dm[0][18];
					if((a == 'A') && (b == 'Y')) ++dm[0][19];
					if((a == 'C') && (b == 'A')) ++dm[1][0];
					if((a == 'C') && (b == 'C')) ++dm[1][1];
					if((a == 'C') && (b == 'D')) ++dm[1][2];
					if((a == 'C') && (b == 'E')) ++dm[1][3];
					if((a == 'C') && (b == 'F')) ++dm[1][4];
					if((a == 'C') && (b == 'G')) ++dm[1][5];
					if((a == 'C') && (b == 'H')) ++dm[1][6];
					if((a == 'C') && (b == 'I')) ++dm[1][7];
					if((a == 'C') && (b == 'K')) ++dm[1][8];
					if((a == 'C') && (b == 'L')) ++dm[1][9];
					if((a == 'C') && (b == 'M')) ++dm[1][10];
					if((a == 'C') && (b == 'N')) ++dm[1][11];
					if((a == 'C') && (b == 'P')) ++dm[1][12];
					if((a == 'C') && (b == 'Q')) ++dm[1][13];
					if((a == 'C') && (b == 'R')) ++dm[1][14];
					if((a == 'C') && (b == 'S')) ++dm[1][15];
					if((a == 'C') && (b == 'T')) ++dm[1][16];
					if((a == 'C') && (b == 'V')) ++dm[1][17];
					if((a == 'C') && (b == 'W')) ++dm[1][18];
					if((a == 'C') && (b == 'Y')) ++dm[1][19];
					if((a == 'D') && (b == 'A')) ++dm[2][0];
					if((a == 'D') && (b == 'C')) ++dm[2][1];
					if((a == 'D') && (b == 'D')) ++dm[2][2];
					if((a == 'D') && (b == 'E')) ++dm[2][3];
					if((a == 'D') && (b == 'F')) ++dm[2][4];
					if((a == 'D') && (b == 'G')) ++dm[2][5];
					if((a == 'D') && (b == 'H')) ++dm[2][6];
					if((a == 'D') && (b == 'I')) ++dm[2][7];
					if((a == 'D') && (b == 'K')) ++dm[2][8];
					if((a == 'D') && (b == 'L')) ++dm[2][9];
					if((a == 'D') && (b == 'M')) ++dm[2][10];
					if((a == 'D') && (b == 'N')) ++dm[2][11];
					if((a == 'D') && (b == 'P')) ++dm[2][12];
					if((a == 'D') && (b == 'Q')) ++dm[2][13];
					if((a == 'D') && (b == 'R')) ++dm[2][14];
					if((a == 'D') && (b == 'S')) ++dm[2][15];
					if((a == 'D') && (b == 'T')) ++dm[2][16];
					if((a == 'D') && (b == 'V')) ++dm[2][17];
					if((a == 'D') && (b == 'W')) ++dm[2][18];
					if((a == 'D') && (b == 'Y')) ++dm[2][19];
					if((a == 'E') && (b == 'A')) ++dm[3][0];
					if((a == 'E') && (b == 'C')) ++dm[3][1];
					if((a == 'E') && (b == 'D')) ++dm[3][2];
					if((a == 'E') && (b == 'E')) ++dm[3][3];
					if((a == 'E') && (b == 'F')) ++dm[3][4];
					if((a == 'E') && (b == 'G')) ++dm[3][5];
					if((a == 'E') && (b == 'H')) ++dm[3][6];
					if((a == 'E') && (b == 'I')) ++dm[3][7];
					if((a == 'E') && (b == 'K')) ++dm[3][8];
					if((a == 'E') && (b == 'L')) ++dm[3][9];
					if((a == 'E') && (b == 'M')) ++dm[3][10];
					if((a == 'E') && (b == 'N')) ++dm[3][11];
					if((a == 'E') && (b == 'P')) ++dm[3][12];
					if((a == 'E') && (b == 'Q')) ++dm[3][13];
					if((a == 'E') && (b == 'R')) ++dm[3][14];
					if((a == 'E') && (b == 'S')) ++dm[3][15];
					if((a == 'E') && (b == 'T')) ++dm[3][16];
					if((a == 'E') && (b == 'V')) ++dm[3][17];
					if((a == 'E') && (b == 'W')) ++dm[3][18];
					if((a == 'E') && (b == 'Y')) ++dm[3][19];
					if((a == 'F') && (b == 'A')) ++dm[4][0];
					if((a == 'F') && (b == 'C')) ++dm[4][1];
					if((a == 'F') && (b == 'D')) ++dm[4][2];
					if((a == 'F') && (b == 'E')) ++dm[4][3];
					if((a == 'F') && (b == 'F')) ++dm[4][4];
					if((a == 'F') && (b == 'G')) ++dm[4][5];
					if((a == 'F') && (b == 'H')) ++dm[4][6];
					if((a == 'F') && (b == 'I')) ++dm[4][7];
					if((a == 'F') && (b == 'K')) ++dm[4][8];
					if((a == 'F') && (b == 'L')) ++dm[4][9];
					if((a == 'F') && (b == 'M')) ++dm[4][10];
					if((a == 'F') && (b == 'N')) ++dm[4][11];
					if((a == 'F') && (b == 'P')) ++dm[4][12];
					if((a == 'F') && (b == 'Q')) ++dm[4][13];
					if((a == 'F') && (b == 'R')) ++dm[4][14];
					if((a == 'F') && (b == 'S')) ++dm[4][15];
					if((a == 'F') && (b == 'T')) ++dm[4][16];
					if((a == 'F') && (b == 'V')) ++dm[4][17];
					if((a == 'F') && (b == 'W')) ++dm[4][18];
					if((a == 'F') && (b == 'Y')) ++dm[4][19];
					if((a == 'G') && (b == 'A')) ++dm[5][0];
					if((a == 'G') && (b == 'C')) ++dm[5][1];
					if((a == 'G') && (b == 'D')) ++dm[5][2];
					if((a == 'G') && (b == 'E')) ++dm[5][3];
					if((a == 'G') && (b == 'F')) ++dm[5][4];
					if((a == 'G') && (b == 'G')) ++dm[5][5];
					if((a == 'G') && (b == 'H')) ++dm[5][6];
					if((a == 'G') && (b == 'I')) ++dm[5][7];
					if((a == 'G') && (b == 'K')) ++dm[5][8];
					if((a == 'G') && (b == 'L')) ++dm[5][9];
					if((a == 'G') && (b == 'M')) ++dm[5][10];
					if((a == 'G') && (b == 'N')) ++dm[5][11];
					if((a == 'G') && (b == 'P')) ++dm[5][12];
					if((a == 'G') && (b == 'Q')) ++dm[5][13];
					if((a == 'G') && (b == 'R')) ++dm[5][14];
					if((a == 'G') && (b == 'S')) ++dm[5][15];
					if((a == 'G') && (b == 'T')) ++dm[5][16];
					if((a == 'G') && (b == 'V')) ++dm[5][17];
					if((a == 'G') && (b == 'W')) ++dm[5][18];
					if((a == 'G') && (b == 'Y')) ++dm[5][19];
					if((a == 'H') && (b == 'A')) ++dm[6][0];
					if((a == 'H') && (b == 'C')) ++dm[6][1];
					if((a == 'H') && (b == 'D')) ++dm[6][2];
					if((a == 'H') && (b == 'E')) ++dm[6][3];
					if((a == 'H') && (b == 'F')) ++dm[6][4];
					if((a == 'H') && (b == 'G')) ++dm[6][5];
					if((a == 'H') && (b == 'H')) ++dm[6][6];
					if((a == 'H') && (b == 'I')) ++dm[6][7];
					if((a == 'H') && (b == 'K')) ++dm[6][8];
					if((a == 'H') && (b == 'L')) ++dm[6][9];
					if((a == 'H') && (b == 'M')) ++dm[6][10];
					if((a == 'H') && (b == 'N')) ++dm[6][11];
					if((a == 'H') && (b == 'P')) ++dm[6][12];
					if((a == 'H') && (b == 'Q')) ++dm[6][13];
					if((a == 'H') && (b == 'R')) ++dm[6][14];
					if((a == 'H') && (b == 'S')) ++dm[6][15];
					if((a == 'H') && (b == 'T')) ++dm[6][16];
					if((a == 'H') && (b == 'V')) ++dm[6][17];
					if((a == 'H') && (b == 'W')) ++dm[6][18];
					if((a == 'H') && (b == 'Y')) ++dm[6][19];
					if((a == 'I') && (b == 'A')) ++dm[7][0];
					if((a == 'I') && (b == 'C')) ++dm[7][1];
					if((a == 'I') && (b == 'D')) ++dm[7][2];
					if((a == 'I') && (b == 'E')) ++dm[7][3];
					if((a == 'I') && (b == 'F')) ++dm[7][4];
					if((a == 'I') && (b == 'G')) ++dm[7][5];
					if((a == 'I') && (b == 'H')) ++dm[7][6];
					if((a == 'I') && (b == 'I')) ++dm[7][7];
					if((a == 'I') && (b == 'K')) ++dm[7][8];
					if((a == 'I') && (b == 'L')) ++dm[7][9];
					if((a == 'I') && (b == 'M')) ++dm[7][10];
					if((a == 'I') && (b == 'N')) ++dm[7][11];
					if((a == 'I') && (b == 'P')) ++dm[7][12];
					if((a == 'I') && (b == 'Q')) ++dm[7][13];
					if((a == 'I') && (b == 'R')) ++dm[7][14];
					if((a == 'I') && (b == 'S')) ++dm[7][15];
					if((a == 'I') && (b == 'T')) ++dm[7][16];
					if((a == 'I') && (b == 'V')) ++dm[7][17];
					if((a == 'I') && (b == 'W')) ++dm[7][18];
					if((a == 'I') && (b == 'Y')) ++dm[7][19];
					if((a == 'K') && (b == 'A')) ++dm[8][0];
					if((a == 'K') && (b == 'C')) ++dm[8][1];
					if((a == 'K') && (b == 'D')) ++dm[8][2];
					if((a == 'K') && (b == 'E')) ++dm[8][3];
					if((a == 'K') && (b == 'F')) ++dm[8][4];
					if((a == 'K') && (b == 'G')) ++dm[8][5];
					if((a == 'K') && (b == 'H')) ++dm[8][6];
					if((a == 'K') && (b == 'I')) ++dm[8][7];
					if((a == 'K') && (b == 'K')) ++dm[8][8];
					if((a == 'K') && (b == 'L')) ++dm[8][9];
					if((a == 'K') && (b == 'M')) ++dm[8][10];
					if((a == 'K') && (b == 'N')) ++dm[8][11];
					if((a == 'K') && (b == 'P')) ++dm[8][12];
					if((a == 'K') && (b == 'Q')) ++dm[8][13];
					if((a == 'K') && (b == 'R')) ++dm[8][14];
					if((a == 'K') && (b == 'S')) ++dm[8][15];
					if((a == 'K') && (b == 'T')) ++dm[8][16];
					if((a == 'K') && (b == 'V')) ++dm[8][17];
					if((a == 'K') && (b == 'W')) ++dm[8][18];
					if((a == 'K') && (b == 'Y')) ++dm[8][19];
					if((a == 'L') && (b == 'A')) ++dm[9][0];
					if((a == 'L') && (b == 'C')) ++dm[9][1];
					if((a == 'L') && (b == 'D')) ++dm[9][2];
					if((a == 'L') && (b == 'E')) ++dm[9][3];
					if((a == 'L') && (b == 'F')) ++dm[9][4];
					if((a == 'L') && (b == 'G')) ++dm[9][5];
					if((a == 'L') && (b == 'H')) ++dm[9][6];
					if((a == 'L') && (b == 'I')) ++dm[9][7];
					if((a == 'L') && (b == 'K')) ++dm[9][8];
					if((a == 'L') && (b == 'L')) ++dm[9][9];
					if((a == 'L') && (b == 'M')) ++dm[9][10];
					if((a == 'L') && (b == 'N')) ++dm[9][11];
					if((a == 'L') && (b == 'P')) ++dm[9][12];
					if((a == 'L') && (b == 'Q')) ++dm[9][13];
					if((a == 'L') && (b == 'R')) ++dm[9][14];
					if((a == 'L') && (b == 'S')) ++dm[9][15];
					if((a == 'L') && (b == 'T')) ++dm[9][16];
					if((a == 'L') && (b == 'V')) ++dm[9][17];
					if((a == 'L') && (b == 'W')) ++dm[9][18];
					if((a == 'L') && (b == 'Y')) ++dm[9][19];
					if((a == 'M') && (b == 'A')) ++dm[10][0];
					if((a == 'M') && (b == 'C')) ++dm[10][1];
					if((a == 'M') && (b == 'D')) ++dm[10][2];
					if((a == 'M') && (b == 'E')) ++dm[10][3];
					if((a == 'M') && (b == 'F')) ++dm[10][4];
					if((a == 'M') && (b == 'G')) ++dm[10][5];
					if((a == 'M') && (b == 'H')) ++dm[10][6];
					if((a == 'M') && (b == 'I')) ++dm[10][7];
					if((a == 'M') && (b == 'K')) ++dm[10][8];
					if((a == 'M') && (b == 'L')) ++dm[10][9];
					if((a == 'M') && (b == 'M')) ++dm[10][10];
					if((a == 'M') && (b == 'N')) ++dm[10][11];
					if((a == 'M') && (b == 'P')) ++dm[10][12];
					if((a == 'M') && (b == 'Q')) ++dm[10][13];
					if((a == 'M') && (b == 'R')) ++dm[10][14];
					if((a == 'M') && (b == 'S')) ++dm[10][15];
					if((a == 'M') && (b == 'T')) ++dm[10][16];
					if((a == 'M') && (b == 'V')) ++dm[10][17];
					if((a == 'M') && (b == 'W')) ++dm[10][18];
					if((a == 'M') && (b == 'Y')) ++dm[10][19];
					if((a == 'N') && (b == 'A')) ++dm[11][0];
					if((a == 'N') && (b == 'C')) ++dm[11][1];
					if((a == 'N') && (b == 'D')) ++dm[11][2];
					if((a == 'N') && (b == 'E')) ++dm[11][3];
					if((a == 'N') && (b == 'F')) ++dm[11][4];
					if((a == 'N') && (b == 'G')) ++dm[11][5];
					if((a == 'N') && (b == 'H')) ++dm[11][6];
					if((a == 'N') && (b == 'I')) ++dm[11][7];
					if((a == 'N') && (b == 'K')) ++dm[11][8];
					if((a == 'N') && (b == 'L')) ++dm[11][9];
					if((a == 'N') && (b == 'M')) ++dm[11][10];
					if((a == 'N') && (b == 'N')) ++dm[11][11];
					if((a == 'N') && (b == 'P')) ++dm[11][12];
					if((a == 'N') && (b == 'Q')) ++dm[11][13];
					if((a == 'N') && (b == 'R')) ++dm[11][14];
					if((a == 'N') && (b == 'S')) ++dm[11][15];
					if((a == 'N') && (b == 'T')) ++dm[11][16];
					if((a == 'N') && (b == 'V')) ++dm[11][17];
					if((a == 'N') && (b == 'W')) ++dm[11][18];
					if((a == 'N') && (b == 'Y')) ++dm[11][19];
					if((a == 'P') && (b == 'A')) ++dm[12][0];
					if((a == 'P') && (b == 'C')) ++dm[12][1];
					if((a == 'P') && (b == 'D')) ++dm[12][2];
					if((a == 'P') && (b == 'E')) ++dm[12][3];
					if((a == 'P') && (b == 'F')) ++dm[12][4];
					if((a == 'P') && (b == 'G')) ++dm[12][5];
					if((a == 'P') && (b == 'H')) ++dm[12][6];
					if((a == 'P') && (b == 'I')) ++dm[12][7];
					if((a == 'P') && (b == 'K')) ++dm[12][8];
					if((a == 'P') && (b == 'L')) ++dm[12][9];
					if((a == 'P') && (b == 'M')) ++dm[12][10];
					if((a == 'P') && (b == 'N')) ++dm[12][11];
					if((a == 'P') && (b == 'P')) ++dm[12][12];
					if((a == 'P') && (b == 'Q')) ++dm[12][13];
					if((a == 'P') && (b == 'R')) ++dm[12][14];
					if((a == 'P') && (b == 'S')) ++dm[12][15];
					if((a == 'P') && (b == 'T')) ++dm[12][16];
					if((a == 'P') && (b == 'V')) ++dm[12][17];
					if((a == 'P') && (b == 'W')) ++dm[12][18];
					if((a == 'P') && (b == 'Y')) ++dm[12][19];
					if((a == 'Q') && (b == 'A')) ++dm[13][0];
					if((a == 'Q') && (b == 'C')) ++dm[13][1];
					if((a == 'Q') && (b == 'D')) ++dm[13][2];
					if((a == 'Q') && (b == 'E')) ++dm[13][3];
					if((a == 'Q') && (b == 'F')) ++dm[13][4];
					if((a == 'Q') && (b == 'G')) ++dm[13][5];
					if((a == 'Q') && (b == 'H')) ++dm[13][6];
					if((a == 'Q') && (b == 'I')) ++dm[13][7];
					if((a == 'Q') && (b == 'K')) ++dm[13][8];
					if((a == 'Q') && (b == 'L')) ++dm[13][9];
					if((a == 'Q') && (b == 'M')) ++dm[13][10];
					if((a == 'Q') && (b == 'N')) ++dm[13][11];
					if((a == 'Q') && (b == 'P')) ++dm[13][12];
					if((a == 'Q') && (b == 'Q')) ++dm[13][13];
					if((a == 'Q') && (b == 'R')) ++dm[13][14];
					if((a == 'Q') && (b == 'S')) ++dm[13][15];
					if((a == 'Q') && (b == 'T')) ++dm[13][16];
					if((a == 'Q') && (b == 'V')) ++dm[13][17];
					if((a == 'Q') && (b == 'W')) ++dm[13][18];
					if((a == 'Q') && (b == 'Y')) ++dm[13][19];
					if((a == 'R') && (b == 'A')) ++dm[14][0];
					if((a == 'R') && (b == 'C')) ++dm[14][1];
					if((a == 'R') && (b == 'D')) ++dm[14][2];
					if((a == 'R') && (b == 'E')) ++dm[14][3];
					if((a == 'R') && (b == 'F')) ++dm[14][4];
					if((a == 'R') && (b == 'G')) ++dm[14][5];
					if((a == 'R') && (b == 'H')) ++dm[14][6];
					if((a == 'R') && (b == 'I')) ++dm[14][7];
					if((a == 'R') && (b == 'K')) ++dm[14][8];
					if((a == 'R') && (b == 'L')) ++dm[14][9];
					if((a == 'R') && (b == 'M')) ++dm[14][10];
					if((a == 'R') && (b == 'N')) ++dm[14][11];
					if((a == 'R') && (b == 'P')) ++dm[14][12];
					if((a == 'R') && (b == 'Q')) ++dm[14][13];
					if((a == 'R') && (b == 'R')) ++dm[14][14];
					if((a == 'R') && (b == 'S')) ++dm[14][15];
					if((a == 'R') && (b == 'T')) ++dm[14][16];
					if((a == 'R') && (b == 'V')) ++dm[14][17];
					if((a == 'R') && (b == 'W')) ++dm[14][18];
					if((a == 'R') && (b == 'Y')) ++dm[14][19];
					if((a == 'S') && (b == 'A')) ++dm[15][0];
					if((a == 'S') && (b == 'C')) ++dm[15][1];
					if((a == 'S') && (b == 'D')) ++dm[15][2];
					if((a == 'S') && (b == 'E')) ++dm[15][3];
					if((a == 'S') && (b == 'F')) ++dm[15][4];
					if((a == 'S') && (b == 'G')) ++dm[15][5];
					if((a == 'S') && (b == 'H')) ++dm[15][6];
					if((a == 'S') && (b == 'I')) ++dm[15][7];
					if((a == 'S') && (b == 'K')) ++dm[15][8];
					if((a == 'S') && (b == 'L')) ++dm[15][9];
					if((a == 'S') && (b == 'M')) ++dm[15][10];
					if((a == 'S') && (b == 'N')) ++dm[15][11];
					if((a == 'S') && (b == 'P')) ++dm[15][12];
					if((a == 'S') && (b == 'Q')) ++dm[15][13];
					if((a == 'S') && (b == 'R')) ++dm[15][14];
					if((a == 'S') && (b == 'S')) ++dm[15][15];
					if((a == 'S') && (b == 'T')) ++dm[15][16];
					if((a == 'S') && (b == 'V')) ++dm[15][17];
					if((a == 'S') && (b == 'W')) ++dm[15][18];
					if((a == 'S') && (b == 'Y')) ++dm[15][19];
					if((a == 'T') && (b == 'A')) ++dm[16][0];
					if((a == 'T') && (b == 'C')) ++dm[16][1];
					if((a == 'T') && (b == 'D')) ++dm[16][2];
					if((a == 'T') && (b == 'E')) ++dm[16][3];
					if((a == 'T') && (b == 'F')) ++dm[16][4];
					if((a == 'T') && (b == 'G')) ++dm[16][5];
					if((a == 'T') && (b == 'H')) ++dm[16][6];
					if((a == 'T') && (b == 'I')) ++dm[16][7];
					if((a == 'T') && (b == 'K')) ++dm[16][8];
					if((a == 'T') && (b == 'L')) ++dm[16][9];
					if((a == 'T') && (b == 'M')) ++dm[16][10];
					if((a == 'T') && (b == 'N')) ++dm[16][11];
					if((a == 'T') && (b == 'P')) ++dm[16][12];
					if((a == 'T') && (b == 'Q')) ++dm[16][13];
					if((a == 'T') && (b == 'R')) ++dm[16][14];
					if((a == 'T') && (b == 'S')) ++dm[16][15];
					if((a == 'T') && (b == 'T')) ++dm[16][16];
					if((a == 'T') && (b == 'V')) ++dm[16][17];
					if((a == 'T') && (b == 'W')) ++dm[16][18];
					if((a == 'T') && (b == 'Y')) ++dm[16][19];
					if((a == 'V') && (b == 'A')) ++dm[17][0];
					if((a == 'V') && (b == 'C')) ++dm[17][1];
					if((a == 'V') && (b == 'D')) ++dm[17][2];
					if((a == 'V') && (b == 'E')) ++dm[17][3];
					if((a == 'V') && (b == 'F')) ++dm[17][4];
					if((a == 'V') && (b == 'G')) ++dm[17][5];
					if((a == 'V') && (b == 'H')) ++dm[17][6];
					if((a == 'V') && (b == 'I')) ++dm[17][7];
					if((a == 'V') && (b == 'K')) ++dm[17][8];
					if((a == 'V') && (b == 'L')) ++dm[17][9];
					if((a == 'V') && (b == 'M')) ++dm[17][10];
					if((a == 'V') && (b == 'N')) ++dm[17][11];
					if((a == 'V') && (b == 'P')) ++dm[17][12];
					if((a == 'V') && (b == 'Q')) ++dm[17][13];
					if((a == 'V') && (b == 'R')) ++dm[17][14];
					if((a == 'V') && (b == 'S')) ++dm[17][15];
					if((a == 'V') && (b == 'T')) ++dm[17][16];
					if((a == 'V') && (b == 'V')) ++dm[17][17];
					if((a == 'V') && (b == 'W')) ++dm[17][18];
					if((a == 'V') && (b == 'Y')) ++dm[17][19];
					if((a == 'W') && (b == 'A')) ++dm[18][0];
					if((a == 'W') && (b == 'C')) ++dm[18][1];
					if((a == 'W') && (b == 'D')) ++dm[18][2];
					if((a == 'W') && (b == 'E')) ++dm[18][3];
					if((a == 'W') && (b == 'F')) ++dm[18][4];
					if((a == 'W') && (b == 'G')) ++dm[18][5];
					if((a == 'W') && (b == 'H')) ++dm[18][6];
					if((a == 'W') && (b == 'I')) ++dm[18][7];
					if((a == 'W') && (b == 'K')) ++dm[18][8];
					if((a == 'W') && (b == 'L')) ++dm[18][9];
					if((a == 'W') && (b == 'M')) ++dm[18][10];
					if((a == 'W') && (b == 'N')) ++dm[18][11];
					if((a == 'W') && (b == 'P')) ++dm[18][12];
					if((a == 'W') && (b == 'Q')) ++dm[18][13];
					if((a == 'W') && (b == 'R')) ++dm[18][14];
					if((a == 'W') && (b == 'S')) ++dm[18][15];
					if((a == 'W') && (b == 'T')) ++dm[18][16];
					if((a == 'W') && (b == 'V')) ++dm[18][17];
					if((a == 'W') && (b == 'W')) ++dm[18][18];
					if((a == 'W') && (b == 'Y')) ++dm[18][19];
					if((a == 'Y') && (b == 'A')) ++dm[19][0];
					if((a == 'Y') && (b == 'C')) ++dm[19][1];
					if((a == 'Y') && (b == 'D')) ++dm[19][2];
					if((a == 'Y') && (b == 'E')) ++dm[19][3];
					if((a == 'Y') && (b == 'F')) ++dm[19][4];
					if((a == 'Y') && (b == 'G')) ++dm[19][5];
					if((a == 'Y') && (b == 'H')) ++dm[19][6];
					if((a == 'Y') && (b == 'I')) ++dm[19][7];
					if((a == 'Y') && (b == 'K')) ++dm[19][8];
					if((a == 'Y') && (b == 'L')) ++dm[19][9];
					if((a == 'Y') && (b == 'M')) ++dm[19][10];
					if((a == 'Y') && (b == 'N')) ++dm[19][11];
					if((a == 'Y') && (b == 'P')) ++dm[19][12];
					if((a == 'Y') && (b == 'Q')) ++dm[19][13];
					if((a == 'Y') && (b == 'R')) ++dm[19][14];
					if((a == 'Y') && (b == 'S')) ++dm[19][15];
					if((a == 'Y') && (b == 'T')) ++dm[19][16];
					if((a == 'Y') && (b == 'V')) ++dm[19][17];
					if((a == 'Y') && (b == 'W')) ++dm[19][18];
					if((a == 'Y') && (b == 'Y')) ++dm[19][19];
				}
			}
			
/*
 First, we set the size of sizeYZ (vectors Y and Z in the manual), which is
 the upper limit of the indices used in vectorY and vectorZ
 */
			
			if (character_type == NUCLEOTIDE) {
				if (coding_type == 0) sizeYZ = 6;
				if (coding_type == 1) sizeYZ = 3;
				if (coding_type == 2) sizeYZ = 3;
				if (coding_type == 3) sizeYZ = 3;
				if (coding_type == 4) sizeYZ = 3;
				if (coding_type == 5) sizeYZ = 3;
				if (coding_type == 6) sizeYZ = 3;
				if (coding_type == 7) sizeYZ = 1;
				if (coding_type == 8) sizeYZ = 1;
				if (coding_type == 9) sizeYZ = 1;
				if (coding_type == 10) sizeYZ = 1;
				if (coding_type == 11) sizeYZ = 1;
				if (coding_type == 12) sizeYZ = 1;
				if (coding_type == 13) sizeYZ = 1;
			} else {
				sizeYZ = 190;
			}

/*
 Next, we set the size of sizeUV (vectors U and V in the manual), which is
 the upper limit of the indices used in vectorU and vectorV
 */
			
			if (character_type == NUCLEOTIDE) {
				if (coding_type == 0) sizeUV = 4;
				if (coding_type == 1) sizeUV = 3;
				if (coding_type == 2) sizeUV = 3;
				if (coding_type == 3) sizeUV = 3;
				if (coding_type == 4) sizeUV = 3;
				if (coding_type == 5) sizeUV = 3;
				if (coding_type == 6) sizeUV = 3;
				if (coding_type == 7) sizeUV = 2;
				if (coding_type == 8) sizeUV = 2;
				if (coding_type == 9) sizeUV = 2;
				if (coding_type == 10) sizeUV = 2;
				if (coding_type == 11) sizeUV = 2;
				if (coding_type == 12) sizeUV = 2;
				if (coding_type == 13) sizeUV = 2;
			} else {
				sizeUV = 20;
			}

/*
 If data contains two sequences only, print divergence matrix to
 the display -- for debugging purposes
 */

			if(num_of_seq == 2 && i < j){
				printf("\n\n\nDivergence matrix:\n");
				for(m = 0; m < sizeUV; m++){
					for(n = 0; n < sizeUV; n++)
						printf("%d,",dm[m][n]);
					printf("\n");
				}
			}
			
/*
 From the divergence matrix, compute two vectors with matching counts
 in the l-th cells (as in the manual).
 
 The numbers of times zero occurs in vectorY and vectorZ is also
 computed.
 */
			for (l = 0; l < sizeYZ; l++) {
				vectorY[l] = 0.0;
                vectorZ[l] = 0.0;
			}
			l = 0;
			for (m = 0; m < sizeUV; m++) {
				for (n = m+1; n < sizeUV; n++) {
                    vectorY[l] = dm[m][n];
                    vectorZ[l] = dm[n][m];
					l++;
				}
			}
            zeroY = 0;
            zeroZ = 0;
            for (l = 0; l < sizeYZ; l++) {
                if (vectorY[l] < 1) ++zeroY;
                if (vectorZ[l] < 1) ++zeroZ;
            }

/*
 If data contains two sequences only, print vectorY and vectorZ to terminal
 for debugging and other purposes
 */
			
			
			if(num_of_seq == 2 && i < j){
				printf("\nvectorY & vectorZ:\n");
				for (l = 0; l < sizeYZ; l++) {
					printf("l = %3d; vectorY = %4.0f; vectorZ = %4.0f\n",l,vectorY[l],vectorZ[l]);
				}
			}

/*
 From the marginal values of the divergence matrix, compute two vectors
 with matching counts in the l-th cells (as in the manual)
*/
			
			for (m = 0; m < sizeUV; m++) {
				vectorU[m] = 0.0;
				vectorV[m] = 0.0;
			}
			for (m = 0; m < sizeUV; m++) {
				varU = 0;
				varV = 0;
				for (n = 0; n < sizeUV; n++) {
					varU = varU + dm[m][n];
					varV = varV + dm[n][m];
				}
				vectorU[m] = varU;
				vectorV[m] = varV;
			}
            zeroU = 0;
            zeroV = 0;
            for (m = 0; m < sizeUV; m++) {
                if (vectorU[m] < 1) ++zeroU;
                if (vectorV[m] < 1) ++zeroV;
            }

/*
 If data contains two sequences only, print vectorU and vectorV to terminal
 for debugging and other purposes
 */
			
			
			if(num_of_seq == 2 && i < j){
				printf("\nvectorU & vectorV:\n");
				for (l = 0; l < sizeUV; l++) {
					printf("l = %3d; vectorU = %4.0f; vectorV = %4.0f\n",l,vectorU[l],vectorV[l]);
				}
			}

/*
 Start of the calculation of different statistics. This is only done
 if sequence i != sequence j
 */

			if (i != j) {

/*
 Bowker's matched-pairs test of symmetry
 */
				df = 0;
				bowker = 0.0;
				for (l = 0; l < sizeYZ; l++) {
					if ((vectorY[l] + vectorZ[l]) > 0) {
						bowker = bowker + SQR(vectorY[l] - vectorZ[l])/(vectorY[l] + vectorZ[l]);
						df++;
					}
				}
				if (df > 0) {
					Q = xChi_Square_Distribution_Tail(bowker, df);
				} else {
					Q = 1.0L;
				}

/*
 Calculating the sum of all entries in the divergence matrix. Needed when
 calculating Euclidean distances
 */
				sum = 0;
				for(m = 0; m < sizeUV; m++)
					for(n = 0; n < sizeUV; n++)
						sum = sum + dm[m][n];
				
/*
 Calculate Euclidean distance based on frequencies of matching characters.
 NOTE, this is not a correct compositional distance because the Euclidean
 geometry is not the proper geometry for compositional data
 */
				
				d_Eucl_Full = 0.0;
				for (l = 0; l < sizeYZ; l++) {
					d_Eucl_Full = d_Eucl_Full + SQR((vectorY[l] - vectorZ[l])/sum);
				}
				d_Eucl_Full = sqrt(d_Eucl_Full);
				
/*
 Calculate Euclidean distance based on marginal frequencies of characters.
 NOTE, this is not a correct compositional distance because the Euclidean
 geometry is not the proper geometry for compositional data
 */

				d_Eucl_Marg = 0.0;
				for (m = 0; m < sizeUV; m++) {
					d_Eucl_Marg = d_Eucl_Marg + SQR((vectorU[m] - vectorV[m])/sum);
				}
				d_Eucl_Marg = sqrt(d_Eucl_Marg)/sqrt(2.0);

/*
 Calculate Aitchison distance based on frequencies of matching characters.
 NOTE, this is the correct compositional distance because the frequencies
 correspond to coordinates in a simplex

 If necessary, we first use Bayesian-multiplicative treatment to address the
 count zero problem, which would occur if any of the elements in vectorY and
 vectorZ equals zero. We used a square-root (SQ) prior
 */

				if (zeroY > 0) {
					sum = 0;
					for (l = 0; l < sizeYZ; l++) {
						sum = sum + (int) vectorY[l];
					}
					zeroAdj = 1.0/(sizeYZ * (1.0 + (double) sqrt(sum)));
					for (l = 0; l < sizeYZ; l++) {
						count = (int) vectorY[l];
						if (count == 0) {
							vectorY[l] = zeroAdj * sum;
//							printf("\nzeroY = %d; sizeYZ = %d; count = %d; sum = %d; zeroAdj = %f; vectorY[l] = %f; l = %d",zeroY,sizeYZ,count,sum,zeroAdj,vectorY[l],l);
						} else {
							vectorY[l] = (1.0 - zeroAdj * zeroY) * count;
//							printf("\nzeroY = %d; sizeYZ = %d; count = %d; sum = %d; zeroAdj = %f; vectorY[l] = %f; l = %d",zeroY,sizeYZ,count,sum,zeroAdj,vectorY[l],l);
						}
					}
				}
				if (zeroZ > 0) {
					sum = 0;
					for (l = 0; l < sizeYZ; l++) {
						sum = sum + (int) vectorZ[l];
					}
					zeroAdj = 1.0/(sizeYZ * (1.0 + (double) sqrt(sum)));
					for (l = 0; l < sizeYZ; l++) {
						count = (int) vectorZ[l];
						if (count == 0) {
							vectorZ[l] = zeroAdj * sum;
//							printf("\nzeroZ = %d; sizeYZ = %d; count = %d; sum = %d; zeroAdj = %f; vectorZ[l] = %f; l = %d",zeroZ,sizeYZ,count,sum,zeroAdj,vectorZ[l],l);
						} else {
							vectorZ[l] = (1.0 - zeroAdj * zeroZ) * count;
//							printf("\nzeroZ = %d; sizeYZ = %d; count = %d; sum = %d; zeroAdj = %f; vectorZ[l] = %f; l = %d",zeroZ,sizeYZ,count,sum,zeroAdj,vectorZ[l],l);
						}
					}
				}
				d_Aitc_Full = 0.0;
                if (zeroY < sizeYZ && zeroZ < sizeYZ) {
                    for (m = 0; m < sizeYZ; m++) {
                        for (n = 0; n < sizeYZ; n++) {
                            d_Aitc_Full = d_Aitc_Full + SQR(log(vectorY[m]/vectorY[n]) - log(vectorZ[m]/vectorZ[n]));
                        }
                    }
                    d_Aitc_Full = sqrt(d_Aitc_Full/(2 * sizeYZ));
                }
//                printf("\nzeroY = %d; zeroZ = %d; d_Aitc_Full = %10.6f\n",zeroY,zeroZ,d_Aitc_Full);

/*
 If data contains two sequences only, print vectorY and vectorZ to terminal
 for debugging and other purposes
 */

				if(num_of_seq == 2 && i < j){
					printf("\nvectorY & vectorZ:\n");
					for (l = 0; l < sizeYZ; l++) {
						printf("l = %3d; vectorY = %10.6f; vectorZ = %10.6f\n",l,vectorY[l],vectorZ[l]);
					}
				}

/*
 Calculate Aitchison distance based on marginal frequencies of characters.
 NOTE, this is the correct compositional distance because the frequencies
 correspond to coordinates in a simplex
 
 If necessary, we first use Bayesian-multiplicative treatment to address the
 count zero problem, which would occur if any of the elements in vectorU and
 vectorV equals zero. We used a square-root (SQ) prior
*/

				if (zeroU > 0) {
					sum = 0;
					for (l = 0; l < sizeUV; l++) {
						sum = sum + (int) vectorU[l];
					}
					zeroAdj = 1.0/(sizeUV * (1.0 + (double) sqrt(sum)));
					for (l = 0; l < sizeUV; l++) {
						count = (int) vectorU[l];
						if (count == 0) {
							vectorU[l] = zeroAdj * sum;
//                            printf("\nzeroU = %d; sizeUV = %d; count = %d; sum = %d; zeroAdj = %f; vectorU[l] = %f; l = %d",zeroU,sizeUV,count,sum,zeroAdj,vectorU[l],l);
						} else {
							vectorY[l] = (1.0 - zeroAdj * zeroU) * count;
//                            printf("\nzeroU = %d; sizeUV = %d; count = %d; sum = %d; zeroAdj = %f; vectorU[l] = %f; l = %d",zeroU,sizeUV,count,sum,zeroAdj,vectorU[l],l);
						}
					}
				}
				if (zeroV > 0) {
					sum = 0;
					for (l = 0; l < sizeUV; l++) {
						sum = sum + (int) vectorV[l];
					}
					zeroAdj = 1.0/(sizeUV * (1.0 + (double) sqrt(sum)));
					for (l = 0; l < sizeUV; l++) {
						count = (int) vectorV[l];
						if (count == 0) {
							vectorV[l] = zeroAdj * sum;
//                            printf("\nzeroV = %d; sizeUV = %d; count = %d; sum = %d; zeroAdj = %f; vectorV[l] = %f; l = %d",zeroV,sizeUV,count,sum,zeroAdj,vectorV[l],l);
						} else {
							vectorV[l] = (1.0 - zeroAdj * zeroV) * count;
//                            printf("\nzeroV = %d; sizeUV = %d; count = %d; sum = %d; zeroAdj = %f; vectorV[l] = %f; l = %d",zeroV,sizeUV,count,sum,zeroAdj,vectorV[l],l);
						}
					}
				}
				d_Aitc_Marg = 0.0;
                if (zeroU < sizeUV && zeroV < sizeUV) {
                    for (m = 0; m < sizeUV; m++) {
                        for (n = 0; n < sizeUV; n++) {
                            d_Aitc_Marg = d_Aitc_Marg + SQR(log(vectorU[m]/vectorU[n]) - log(vectorV[m]/vectorV[n]));
                        }
                    }
                    d_Aitc_Marg = sqrt(d_Aitc_Marg/(2 * sizeUV));
                }
//                printf("\nzeroU = %d; zeroV = %d; d_Aitc_Marg = %10.6f\n",zeroU,zeroV,d_Aitc_Marg);

                
/*
 If data contains two sequences only, print vectorY and vectorZ to terminal
 for debugging and other purposes
 */
                
                if(num_of_seq == 2 && i < j){
                    printf("\nvectorU & vectorV:\n");
                    for (l = 0; l < sizeUV; l++) {
                        printf("l = %3d; vectorU = %10.6f; vectorV = %10.6f\n",l,vectorU[l],vectorV[l]);
                    }
                }

			} else {

/*
 This is where default values are drawn from in case i == j
 */
				Q = 1.0L;
				sizeYZ = 0;
				sizeUV = 0;
				d_Eucl_Marg = 0.0;
				d_Eucl_Full = 0.0;
				d_Aitc_Marg = 0.0;
				d_Aitc_Full = 0.0;

			}

/*
 From now on, the focus is on printing the results to the output files
 */

			if(j < (num_of_seq - 1)){
				fprintf(mat1File,"%Le,",Q);
				fprintf(mat2File,"%e\t",d_Eucl_Marg);
				fprintf(mat3File,"%e\t",d_Eucl_Full);
				if (i != j) {
					if (sizeUV > 1) {
						fprintf(mat4File,"%e\t",d_Aitc_Marg);
					} else {
						fprintf(mat4File,"NaN\t");
					}
					if (sizeYZ > 1) {
						fprintf(mat5File,"%e\t",d_Aitc_Full);
					} else {
						fprintf(mat5File,"NaN\t");
					}
				} else {
					fprintf(mat4File,"%e\t",0.0);
					fprintf(mat5File,"%e\t",0.0);
				}
			}
			else{
				fprintf(mat1File,"%Le\n",Q);
				fprintf(mat2File,"%e\n",d_Eucl_Marg);
				fprintf(mat3File,"%e\n",d_Eucl_Full);
				if (i != j) {
					if (sizeUV > 1) {
						fprintf(mat4File,"%e\n",d_Aitc_Marg);
					} else {
						fprintf(mat4File,"NaN\n");
					}
					if (sizeYZ > 1) {
						fprintf(mat5File,"%e\n",d_Aitc_Full);
					} else {
						fprintf(mat5File,"NaN\n");
					}
				} else {
					fprintf(mat4File,"%e\n",0.0);
					fprintf(mat5File,"%e\n",0.0);
				}
			}
			if(i < j){
                ++counter;
				printf("\rNumber of comparisons left = %12lu",total - counter);
				fflush(NULL);
				fprintf(outFile,"%s,%s",seq_name_1, seq_name_2);
				if (df == 0) {
					fprintf(outFile,",No test,%d,%Le,%e,%e",df,Q,d_Eucl_Marg,d_Eucl_Full);
					if (sizeUV > 1) {
						fprintf(outFile,",%e",d_Aitc_Marg);
					} else {
						fprintf(outFile,",NaN");
					}
					if (sizeYZ > 1) {
						fprintf(outFile,",%e",d_Aitc_Full);
					} else {
						fprintf(outFile,",NaN");
					}
					fprintf(outFile,",%d\n",sum);
				} else {
					fprintf(outFile,",%f,%d,%Le,%e,%e",bowker,df,Q,d_Eucl_Marg,d_Eucl_Full);
					if (sizeUV > 1) {
						fprintf(outFile,",%e",d_Aitc_Marg);
					} else {
						fprintf(outFile,",NaN");
					}
					if (sizeYZ > 1) {
						fprintf(outFile,",%e",d_Aitc_Full);
					} else {
						fprintf(outFile,",NaN");
					}
					fprintf(outFile,",%d\n",sum);
				}
			}
			fflush(outFile);
			fflush(mat1File);
			fflush(mat2File);
			fflush(mat3File);
			fflush(mat4File);
			fflush(mat5File);
			
/*
 Summing up the number of bad cases
 */
			
			if(i < j && df != 0){
				if(Q < MIN_Q) MIN_Q = Q;
				if(Q < 0.05) ++sig_A;
				if(Q < 0.01) ++sig_B;
				if(Q < 0.005) ++sig_C;
				if(Q < 0.001) ++sig_D;
				if(Q < 0.0005) ++sig_E;
				if(Q < 0.0001) ++sig_F;
				if(Q < 0.00005) ++sig_G;
				if(Q < 0.00001) ++sig_H;
			}
		}
	}
	Screen_flush();
	
	printf("\n\n\n\n___________ Highlights from the Analysis ___________\n");
	if(MIN_Q < 0.05) {
		printf("                                         Num. Prop.");
		printf("\np-values < 0.05                   %10lu (%.3f)",sig_A, (float) sig_A/counter);
	}
	if(MIN_Q < 0.01)
		printf("\np-values < 0.01                   %10lu (%.3f)",sig_B, (float) sig_B/counter);
	if(MIN_Q < 0.005)
		printf("\np-values < 0.005                  %10lu (%.3f)",sig_C, (float) sig_C/counter);
	if(MIN_Q < 0.001)
		printf("\np-values < 0.001                  %10lu (%.3f)",sig_D, (float) sig_D/counter);
	if(MIN_Q < 0.0005)
		printf("\np-values < 0.0005                 %10lu (%.3f)",sig_E, (float) sig_E/counter);
	if(MIN_Q < 0.0001)
		printf("\np-values < 0.0001                 %10lu (%.3f)",sig_F, (float) sig_F/counter);
	if(MIN_Q < 0.00005)
		printf("\np-values < 0.00005                %10lu (%.3f)",sig_G, (float) sig_G/counter);
	if(MIN_Q < 0.00001)
		printf("\np-values < 0.00001                %10lu (%.3f)",sig_H, (float) sig_H/counter);
	printf("\n");
	printf("\nNumber of tests                        %d",counter);
	printf("\nSmallest p-value                       %e",MIN_Q);
	printf("\nFamily-wise error rate (0.05/tests)    %e",0.05/counter);
	printf("\n");
	if (MIN_Q < (0.05/counter)) {
		printf("\nWARNING:");
		printf("\nAt least one pair of sequences is unlikely to have evolved");
		printf("\nunder the same Markovian process. For further details, see");
		printf("\n%s.",outName);
		printf("\n");
	}
	printf("\nSpreadsheet with all results          %s",outName);
	printf("\nMatrix with p-values                  %s",mat1Name);
	printf("\nMatrix with Euclidean distances (m)   %s",mat2Name);
	printf("\nMatrix with Euclidean distances (f)   %s",mat3Name);
	printf("\nMatrix with Aitchison distances (m)   %s",mat4Name);
	printf("\nMatrix with Aitchison distances (f)   %s",mat5Name);
	printf("\n____________________________________________________\n\n");

}


/* Estimating degrees of freedom for Bowker's test */

unsigned int Bowker_DF(unsigned int temp[20][20])
{
	int           i, j;      /* counters */
	unsigned int  df = 0;    /* degrees of freedom */

	for(i = 0; i < 20; i++) {
		for(j = i + 1; j < 20; j++) {
			if((temp[i][j] + temp[j][i]) > MINIMUM)
				df++;
		}
	}
	return df;
}


/* Bowker's (1948) test for symmetry of a contingency table */

double Bowker_Test(unsigned int temp[20][20])
{
	int    i, j;      /* counters */
	double bts = 0.0; /* Bowker's test statistic */

	for(i = 0; i < 20; i++) {
		for(j = i + 1; j < 20; j++) {
			if((temp[i][j] + temp[j][i]) > MINIMUM) {
				bts = bts + ((float)(SQR(temp[i][j] - temp[j][i]))/(temp[i][j] + temp[j][i]));
			}
		}
	}
	return bts;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// NOTE PERTAINING TO THE FOLLOWING FOUR FUNCTIONS                            //
//                                                                            //
// Source: gaussian_distribution_tail.c                                       //
//                                                                            //
// Source: chi-square_distribution_tail.c                                     //
//                                                                            //
// Author: Dick Horn (mathretprogr@gmail.com)                                 //
//                                                                            //
// Note: Used with permission from the author (Wednesday, 9 July 2014 4:30)   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/*
 This function returns the probability that a random variable with a standard 
 Normal (Gaussian) distribution has a value greater than "x"
 */

long double xGaussian_Distribution_Tail( long double x )
{
   long double sqrt2 = 0.7071067811865475244008443621048490L;
   return  0.5L * erfcl(sqrt2 * x );
}

/*
 This function returns the probability that a random chi-squared-distributed 
 variable has a value greater than "x"
 */

long double xChi_Square_Distribution_Tail(long double x, int dof) {
   
   if (dof <= 0) return 0.0L;
	
   if (dof % 2 == 0)
		return Sum_Poisson_Terms(x/2.0L, dof/2);
   else
		return Sum_Over_Odd_Terms(x,dof);
};

/* The number of degrees of freedom, nu, is an even integer, nu = 2*n.        */
/* The argument x is chi^2 / 2.                                               */


static long double Sum_Poisson_Terms(long double x, int n) {
   int k;
   long double term;
   long double sum;
	
   term = 1.0L;
   sum = 1.0L;
   for (k = 1; k < n; k++) {
      term *= (x / k);
      sum += term;
   }
   return expl(-x)*sum;
};


static long double Sum_Over_Odd_Terms(long double x, int dof)
{
   int k;
   int n;
   long double term;
   long double sum;
   long double sqrtx;
   long double twooverpi;
	
   twooverpi = 0.6366197723675813430755350534900574L;
	
   if (dof == 1) return 2.0L * xGaussian_Distribution_Tail( sqrtl(x) );
   n = (dof - 1) / 2;
   sqrtx = sqrtl(x);
   term = sqrtx;
   sum = sqrtx;
   for (k = 2; k <=n; k++) {
      term *= ( x / (k + k - 1) );
      sum += term;
   };
	
   return 2.0L * xGaussian_Distribution_Tail(sqrtx)
	+ sqrtl(twooverpi) * expl(-x/2.0L)*sum;
};

