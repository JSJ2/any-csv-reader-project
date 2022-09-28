#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <Windows.h>
#include <conio.h>
#include <io.h> // access() : 파일이나 디렉토리(폴더)의 존재 유무, 혹은 파일의 권한 및 속성을 조사할 때 사용한다.

#define MAX 128
#define MORE_MAX 1024

void fullscreen() {
	SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, 0);
}

void str_clear(char string[], int size)
{
	for (int i = 0; i < size; i++)
		string[i] = '\0';
}

// csv파일 경로만 전달하면 column수를 반환함.
int auto_find_column(char filename[]) {
	FILE* fpR = fopen(filename, "r");

	int column_cnt = 0;

	char line[MORE_MAX];
	line[0] = '\0';

	fgets(line, MORE_MAX, fpR);

	char* ptr = strtok(line, ",");

	while (ptr != NULL)
	{
		column_cnt++;
		ptr = strtok(NULL, ",");
	}
	fclose(fpR);

	return column_cnt;
}

// csv파일 경로만 전달하면 row수를 반환함.
int auto_find_row(char filename[]) {
	FILE* fpR = fopen(filename, "r");

	int row_cnt = 0;

	char line[MORE_MAX];
	line[0] = '\0';
	fgets(line, MORE_MAX, fpR); // header 버리기.
	while (feof(fpR) == 0)
	{
		fgets(line, MORE_MAX, fpR);
		row_cnt++;
	}
	fclose(fpR);

	return row_cnt; // feof()는 함수의 끝의 기준은 마지막 줄이 아닌 마지막 \n이다. 그러므로 \n을 찾기까지 반복한다. 결론 : 마지막 줄에 도달해도 한 번더 반복하므로 1을 빼줘야 한다.
}

// 숫자면 0을 반환
// 그 이외의 값이면 1을 반환
int check_word(char ptr2[])
{
	int IsString = false;
	int test = strlen(ptr2);
	for (unsigned int i = 0; i < strlen(ptr2); i++)
	{
		if (ptr2[i] >= 48 && ptr2[i] <= 57) // 숫자
			IsString = false;
		else if (ptr2[i] == '.')
			IsString = false;
		else if (ptr2[i] == '\n')
			IsString = false;
		else
		{
			IsString = true;
			break;
		}
	}

	if (IsString)
		return 1;
	else
		return 0;
}


// 실수 1, 문자열 2, None -1
void check_type(char filename[], char* order_column_type, int* double_cnt, int* string_cnt, int total_column)
{
	FILE* fpR = fopen(filename, "r");
	
	char line[MORE_MAX];
	line[0] = '\0';

	fgets(line, MORE_MAX, fpR); // header 버림

	fgets(line, MORE_MAX, fpR);
	char* ptr2 = strtok(line, ",");
	for (int i = 0; i<total_column; i++)
	{
		int return_value = check_word(ptr2);
		switch (return_value)
		{
		case 0:
			order_column_type[i] = '0';
			(*double_cnt)++;
			break;
		case 1:
			order_column_type[i] = '1';
			(*string_cnt)++;
			break;
		default:
			break;
		}
		ptr2 = strtok(NULL, ",");
	}
	order_column_type[total_column] = '\0';
	fclose(fpR);
}

void mkdir_csv() {
	int nResult = _access("./csv", 0);
	// csv 디렉토리가 없다면 만들고 1을 반환함
	// 있다면 0을 반환함.
	switch (nResult)
	{
	case 0:
		return;
	case -1:
		system("mkdir csv");
		printf("Success!! : Directory is created.\n\n");
		break;
	}
}

void connect_string_with_csv(char s1[], char* s2) {
	sprintf(s1,"./csv/%s.csv", s2);
}

int exist_csv_file(char* filename, char csv_name[]) {
	connect_string_with_csv(filename, csv_name);

	FILE* fpR = fopen(filename, "r");
	if (fpR == NULL)
	{
		printf("\nFailed.. : We can't find '%s'..\n\n", filename);
		exit(0);
	}
	else
	{
		fclose(fpR);

		printf("\nSuccess!! : Exist '%s'!\n\n", filename);
		return 1;
	}
}

void print_csv(double** ptr_ptr_double, char*** ptr_ptr_string, char** ptr_ptr_header, int total_column, int total_row, char* order_column_type)
{
	int i = 0, j = 0, k = 0;
	for (int index = 0; index < total_column; index++)
		printf("%-10s\t", ptr_ptr_header[index]);
	printf("\n");
	int one_by_one = 1;
	for (int row_index = 0; row_index < total_row; row_index++)
	{
		if (one_by_one)
		{
			char key = '\0';
			key = _getch();
			switch (key) {
			case '\r':
				for (int column_index = 0; column_index < total_column; column_index++)
				{

					switch (order_column_type[column_index])
					{
					case '0': // 실수
						printf("%-10.2lf\t", ptr_ptr_double[row_index][i]);
						i++;
						break;

					case '1': // 문자열
						printf("%-10s\t", ptr_ptr_string[row_index][j]);
						j++;
						break;

					default:
						break;
					}
				}
				i = 0, j = 0;
				printf("\n");
				break;
			default:
				one_by_one = 0;
				break;
			}
		}
		else
		{
			for (int column_index = 0; column_index < total_column; column_index++)
			{

				switch (order_column_type[column_index])
				{
				case '0': // 실수
					printf("%-10.2lf\t", ptr_ptr_double[row_index][i]);
					j++;
					break;

				case '1': // 문자열
					printf("%-10s\t", ptr_ptr_string[row_index][j]);
					k++;
					break;

				default:
					break;
				}
			}
			i = 0, j = 0;
			printf("\n");
		}
	}
}

void csv_format(char filename[]) {
	int total_column = 0, total_row = 0, return_value = 0;
	int double_cnt = 0, string_cnt = 0;

	/*system("cls");

	printf("csv의 총 행의 수는 몇 입니까? : ");
	scanf_s("%d", &total_column);


	printf("csv의 총 열의 수는 몇 입니까? : ");
	scanf_s("%d", &total_row);*/

	total_column = auto_find_column(filename);
	total_row = auto_find_row(filename);

	char* order_column_type = (char*)malloc(sizeof(char) * total_column + 1);

	system("cls");

	check_type(filename, order_column_type, &double_cnt, &string_cnt, total_column);


	// 실수 2차원 배열 할당
	double** ptr_ptr_double;
	ptr_ptr_double = (double**)malloc(sizeof(double*) * total_row);
	for (int i = 0; i < total_row; i++)
		ptr_ptr_double[i] = (double*)malloc(sizeof(double) * double_cnt);

	// 문자열 3차원 배열 할당
	char*** ptr_ptr_string;
	ptr_ptr_string = (char***)malloc(sizeof(char**) * total_row);
	for (int i = 0; i < total_row; i++)
	{
		ptr_ptr_string[i] = (char**)malloc(sizeof(char*) * string_cnt);
		for (int j = 0; j < string_cnt; j++)
		{
			ptr_ptr_string[i][j] = (char*)malloc(sizeof(char) * MAX + 1); // 마지막 배열 칸에 NULL문자 고려
			ptr_ptr_string[i][j][0] = '\0';
		}
	}

	// 주의 사항
	// 1. csv는 ANSI(아스키코드)와 호환된다. csv을 저장할 때 유니코드가 아닌 아스키코드로 저장할 것
	//		(1.) 을 지키지 않을 경우 문자열이 깨져 한글이 아닌 쓰레기값을 가져온다.

	FILE* fpR = fopen(filename, "r");
	char line[MORE_MAX];
	line[0] = '\0';

	fgets(line, MORE_MAX, fpR); // 첫 줄은 각 column의 카테고리 이름이므로 2차원 배열을 만들어 저장한다.

	char* header_ptr = strtok(line, ",");

	char** ptr_ptr_header = (char**)malloc(sizeof(char*) * total_column);
	for (int i = 0; i < total_column; i++)
	{
		ptr_ptr_header[i] = (char*)malloc(sizeof(char) * strlen(header_ptr) + 1); // 각 column별 이름의 바이트 크기를 구해 각각 다른 크기를 할당한다.
		strcpy(ptr_ptr_header[i], header_ptr);
		//printf("%s\n", ptr_ptr_header[i]);
		header_ptr = strtok(NULL, ",");
	}
	
	str_clear(line, MORE_MAX);

	int index = 0;
	int i = 0, j = 0, k = 0;
	while (feof(fpR) == 0)
	{
		fgets(line, MORE_MAX, fpR);
		char* ptr = strtok(line, ",");
		while(ptr != NULL)
		{
			switch (order_column_type[index]) 
			{
			// 실수
			case '0':
				sscanf(ptr, "%lf", &ptr_ptr_double[k][i]);
				i++;
				break;

			// 문자열
			case '1':
				strcpy(ptr_ptr_string[k][j], ptr);
				j++;
				break;
			}
			ptr = strtok(NULL, ",");
			index++;
		}
		str_clear(line, MORE_MAX);
		index = 0, i = 0, j = 0;
		k++;
	}
	
	fclose(fpR);
	system("cls");
	
	printf("Success!, '%s' data!\n\n", filename);
	print_csv(ptr_ptr_double, ptr_ptr_string, ptr_ptr_header, total_column, total_row, order_column_type);
	printf("\n\n1. You use the particular column calculator\t2. You sample the particular row or column!");
	for (int i = 0; i < total_row; i++)
		free(ptr_ptr_double[i]);

	free(ptr_ptr_double);

	for (int i = 0; i < total_row; i++)
	{
		for (int j = 0; j < string_cnt; j++)
		{
			free(ptr_ptr_string[i][j]);
		}
		free(ptr_ptr_string[i]);
	}
	free(ptr_ptr_string);

	for (int i = 0; i < total_column; i++)
		free(ptr_ptr_header[i]);



	free(ptr_ptr_header);
	free(order_column_type);
}


// 오류가 있으면 바로 종료된다.

int main() {
	fullscreen();
	char filename[MAX + 10]; // + ./csv/*.txt(=10bytes)
	filename[0] = '\0';

	char csv_name[MAX];
	csv_name[0] = '\0';

	mkdir_csv(); // csv 디렉토리가 있는지
	while (1)
	{
		system("cls");

		printf("< They are *.csv file in csv dir >\n\n");
		system("dir csv /b");

		printf("\nEnter your csv name [exit : -1]: ");
		scanf_s("%s", csv_name, sizeof(csv_name));
		if (strcmp(csv_name, "-1") == 0)
			exit(1);

		exist_csv_file(filename, csv_name); // csv 파일이 있는지

		csv_format(filename); // csv파일 format 묻기
		printf("\n\n");
		system("pause");
	}
	return 0;
}