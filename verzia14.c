#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

/* open file and show information about all events */
FILE *v() {

	FILE *fr;
	int line, maxLength;
	char *p_string;

	// 30 for letters, 1 for break line, 1 for end sign
	maxLength = 32;

	if ((fr = fopen("podujatia.txt", "r")) == NULL) {
		printf("Neotvoreny subor\n");
	}
	else {
		p_string = (char *)malloc(maxLength * sizeof(char));

		line = 0;
		while (fgets(p_string, maxLength, fr) != NULL) {
			// always 6 details per event
			switch (line) {
			case 0:
				printf("nazov: %s", p_string);
				break;
			case 1:
				printf("organizator: %s", p_string);
				break;
			case 2:
				printf("pre deti: %s", p_string);
				break;
			case 3:
				printf("cena: %s", p_string);
				break;
			case 4:
				printf("datum: %s", p_string);
				break;
			case 5:
				printf("cas: %s", p_string);
				break;
			case 6:
				putchar('\n');
				line = -1;
				break;
			default:
				break;
			}
			line++;
		}
		free(p_string);
	}

	return fr;
}

/* find and show the latest kids event's time of the last year */
void c(FILE *fr) {

	char ch;
	int line, thisYear, latestYear, isForKids; // the last variable is boolean
	float thisTime, latestTime;

	if (fr != NULL) {
		latestYear = -1;
		latestTime = -1.0;

		fseek(fr, 0, SEEK_SET);
		line = 0;
		while ((ch = getc(fr)) != EOF) {
			if (ch == '\n') {
				line++;
				switch (line % 7) {
				case 2:
					isForKids = 0;
					fscanf(fr, "%d", &isForKids);
					break;
				case 4:
					if (isForKids) {
						fseek(fr, 4, SEEK_CUR);
						fscanf(fr, "%d", &thisYear);
						fscanf(fr, "%f", &thisTime);
							
						if (thisYear >= latestYear) {
							latestYear = thisYear;
							if (thisTime > latestTime)
								latestTime = thisTime;
						}

						/* another increment required since 1 line was skipped */
						line++;
					}
					break;
				default:
					break;
				}
			}
		}
		if (latestTime >= 0.0)
			printf("%.2f\n", latestTime);
	}
}

/* create array with event prices */
double *e(FILE *fr, double *p_prices, int *p_numEvents) {

	char ch;
	int line, priceIndex, lineCount;

	if (fr != NULL) {

		// count lines
		fseek(fr, 0, SEEK_SET);
		lineCount = 0;
		while ((ch = getc(fr)) != EOF) {
			if (ch == '\n') {
				lineCount++;
			}
		}

		// prepare array for required number of elements
		*p_numEvents = lineCount / 7;
		if (p_prices != NULL)
			p_prices = (double *)realloc(p_prices, (*p_numEvents) * sizeof(double));
		p_prices = (double *)malloc((*p_numEvents) * sizeof(double));

		// note all prices
		fseek(fr, 0, SEEK_SET);
		line = priceIndex = 0;
		while ((ch = getc(fr)) != EOF) {
			if (ch == '\n') {
				if (line % 7 == 2) {
					fscanf(fr, "%lf", p_prices + priceIndex);
					priceIndex++;
				}
				line++;
			}
		}
	}

	return p_prices;
}

/* show formatted price list */
void r(double *p_prices, int numEvents) {

	int i, numDigits;
	double biggest;

	if(p_prices == NULL)
		printf("Pole nie je vytvorene\n");
	else if (numEvents > 0) {

		// find out value with the most digits
		biggest = *p_prices;
		for (i = 1; i < numEvents; i++) {
			if (*(p_prices + i) > biggest)
				biggest = *(p_prices + i);
		}

		// count total length to output
		i = (int) floor(biggest);
		numDigits = 3; // at least decimal point and hundredths
		while (i > 0) {
			i /= 10;
			numDigits++;
		}

		for (i = 0; i < numEvents; i++) {
			printf("%*.2f\n", numDigits, *(p_prices + i));
		}
	}
}

/* show histogram of digits in price bases */
void h(double *p_prices, int numEvents) {

	int i, digit, priceFloor, *digitCount;

	if (p_prices == NULL)
		printf("Pole nie je vytvorene\n");
	else if (numEvents > 0) {

		digitCount = (int *)calloc(10, sizeof(int));

		for (i = 0; i < numEvents; i++) {
			priceFloor = (int)floor(*(p_prices + i));
			while (priceFloor > 0) {
				digit = priceFloor % 10;
				(*(digitCount + digit))++;
				priceFloor /= 10;
			}
		}

		for (i = 9; i >= 0; i--) {
			printf("%d:%d\n", i, *(digitCount + i));
		}

		free(digitCount);
	}
}

/* order prices descending via bubbleSort */
void z(double *p_prices, int numEvents) {

	int i, change; // the last variable is boolean
	double temp;

	if (p_prices != NULL && numEvents > 0) {
		do {
			change = 0;
			for (i = 0; i + 1 < numEvents; i++) {
				if (*(p_prices + i) < *(p_prices + i + 1)) {
					temp =  *(p_prices + i);
					*(p_prices + i) = *(p_prices + i + 1);
					*(p_prices + i + 1) = temp;
					change = 1;
				}
			}
		} while (change);
	}
}

/* show max. 3 organizators with most vocals ascending */
void s(FILE *fr) {

	int i, j, count, maxLength, line, temp, *authorVocals, change; // the last variable is boolean
	char ch, *tempStr, **authorNames;

	if (fr != NULL) {

		// 30 for letters, 1 for break line, 1 for end sign
		maxLength = 32;

		tempStr = (char *)malloc(maxLength * sizeof(char));
		// 1 for temporary values, 3 for sought ones
		authorVocals = (int *)calloc(4, sizeof(int));
		authorNames = (char **)malloc(4 * sizeof(char*));
		for (i = 0; i < 4; i++) {
			authorNames[i] = (char *)calloc(maxLength, sizeof(char));
		}

		fseek(fr, 0, SEEK_SET);
		line = 0;
		while (fgets(authorNames[0], maxLength, fr) != NULL) {

			if (line % 7 == 1) {

				// count vocals for each author
				count = 0;
				for (i = 0; (ch = authorNames[0][i]) != '\n' && i < 30; i++) {
					ch = tolower(ch);
					if (ch == 'a' ||
						ch == 'e' ||
						ch == 'i' ||
						ch == 'o' ||
						ch == 'u' ||
						ch == 'y') {
						count++;
					}
				}

				// inner bubbleSort after each author
				authorVocals[0] = count;
				do {
					change = 0;
					for (i = 0; i < 3; i++) {
						if (authorVocals[i] > authorVocals[i + 1]) {

							temp = authorVocals[i];
							authorVocals[i] = authorVocals[i + 1];
							authorVocals[i + 1] = temp;
							
							/*	after change in count array,
								change names array correspondingly	*/
							tempStr = authorNames[i];
							authorNames[i] = authorNames[i + 1];
							authorNames[i + 1] = tempStr;

							change = 1;
						}
					}
				} while (change);
				
			}
			line++;
		}

		// output only items with entry
		for (i = 1; i < 4; i++) {
			j = 0;
			while ((ch = authorNames[i][j]) != '\n' && ch != '\0' && j < 30) {
				putchar(ch);
				j++;
			}
			if (ch != '\0')
				printf(":%d\n", authorVocals[i]);
		}
	}
}

int main()
{
	char ch;
	FILE *fr;
	int numEvents, *p_numEvents;
	double *p_prices;

	fr = NULL;
	numEvents = 0;
	p_numEvents = &numEvents;
	p_prices = NULL;

	while ((ch = tolower(getchar())) != 'k') {
		switch (ch) {
		case 'v':
			fr = v();
			break;
		case 'c':
			c(fr);
			break;
		case 'e':
			p_prices = e(fr, p_prices, p_numEvents);
			break;
		case 'r':
			r(p_prices, numEvents);
			break;
		case 'h':
			h(p_prices, numEvents);
			break;
		case 'z':
			z(p_prices, numEvents);
			break;
		case 's':
			s(fr);
			break;
		default:
			// only specified letters matter
			break;
		}
	}

	if (fr != NULL && fclose(fr) == EOF)
		printf("Nezatvoreny subor\n");

	return 0;
}
