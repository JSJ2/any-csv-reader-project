#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <Windows.h>
#include <io.h> // access() : 파일이나 디렉토리(폴더)의 존재 유무, 혹은 파일의 권한 및 속성을 조사할 때 사용한다.

#define MAX 128

void str_clear(char string[], int size)
{
	for (int i = 0; i < size; i++)
		string[i] = '\0';
}

// csv파일 경로만 전달하면 column수를 반환함.
int auto_find_column(char filename[]) {
	FILE* fpR = fopen(filename, "r");

	int column_cnt = 0;

	char line[MAX];
	line[0] = '\0';

	fgets(line, MAX, fpR);

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

	char line[MAX];
	line[0] = '\0';
	while (feof(fpR) == 0)
	{
		fgets(line, MAX, fpR);
		row_cnt++;
	}
	fclose(fpR);

	return row_cnt - 1; // feof()는 함수의 끝의 기준은 마지막 줄이 아닌 마지막 \n이다. 그러므로 \n을 찾기까지 반복한다. 결론 : 마지막 줄에 도달해도 한 번더 반복하므로 1을 빼줘야 한다.
}


// 정수 0, 실수 1, 문자열 2, None -1
int check_type(char ptr[])
{
	// 정수 = 0
	if (strcmp("정수", ptr) == 0) {
		return 0;
	}
	// 실수 = 1
	else if (strcmp("실수", ptr) == 0) {
		return 1;
	}
	// 문자열 = 2
	else if (strcmp("문자열", ptr) == 0) {
		return 2;
	}
	// None = -1
	else
		return -1;
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
	sprintf(s2,"./csv/%s.csv", s1);
}

int exist_csv_file(char* filename) {
	char csv_name[MAX];
	csv_name[0] = '\0';
	filename[0] = '\0';
	printf("< csv 디렉토리에 있는 *.csv 파일 >\n\n");
	system("dir csv /b");

	printf("\nEnter your csv name : ");
	scanf_s("%s", csv_name, sizeof(csv_name));

	connect_string_with_csv(csv_name, filename);

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

void print_csv(int** ptr_ptr_int, double** ptr_ptr_double, char*** ptr_ptr_string, char** ptr_ptr_header, int total_column, int total_row, char* order_column_type)
{
	int i = 0, j = 0, k = 0;
	for (int i = 0; i < total_column; i++)
		printf("%-10s\t", ptr_ptr_header[i]);
	printf("\n");
	for (int row_index = 0; row_index < total_row; row_index++)
	{
		for (int column_index = 0; column_index < total_column; column_index++)
		{
			
			switch (order_column_type[column_index])
			{
			case '0': // 정수
				printf("%-10d\t", ptr_ptr_int[row_index][i]);
				i++;
				break;

			case '1': // 실수
				printf("%-10lf\t", ptr_ptr_double[row_index][j]);
				j++;
				break;

			case '2': // 문자열
				printf("%-10s\t", ptr_ptr_string[row_index][k]);
				k++;
				break;

			default:
				break;
			}
		}
		i = 0, j = 0, k = 0;
		printf("\n");
	}
}

void csv_format(char filename[]) {
	int total_column = 0, total_row = 0, return_value = 0;
	int int_cnt = 0, double_cnt = 0, string_cnt = 0;

	/*system("cls");

	printf("csv의 총 행의 수는 몇 입니까? : ");
	scanf_s("%d", &total_column);


	printf("csv의 총 열의 수는 몇 입니까? : ");
	scanf_s("%d", &total_row);*/

	total_column = auto_find_column(filename);
	total_row = auto_find_row(filename);

	char* order_column_type = (char*)malloc(sizeof(char) * total_column + 1);

	char type_name[7];
	type_name[0] = '\0';

	system("cls");
	printf("\t       <자료형 보기>\n\n정수\t실수\t문자열\n\n만약, 자료형이 '정수'라면 '정수'를 입력하세요.\n\n");
	for (int i = 0; i < total_column; i++)
	{
		printf("%d 번째 행의 데이터 자료형은 무엇입니까?\n>>> ", i + 1);
		scanf_s("%s", type_name, sizeof(type_name));
		return_value = check_type(type_name);
		switch (return_value)
		{
		// 정수
		case 0:
			int_cnt++;
			*(order_column_type + i) = '0';
			break;

		// 실수
		case 1:
			double_cnt++;
			*(order_column_type + i) = '1';
			break;

		// 문자열
		case 2:
			string_cnt++;
			*(order_column_type + i) = '2';
			break;

		// None
		default:
			printf("\n잘못 입력 하셨습니다. 다시 입력해주세요.\n\n");
			i--;
			break;
		}
	}
	order_column_type[total_column] = '\0';

	//printf("int_cnt = %d, float_cnt = %d, string_cnt = %d\n\n%s", int_cnt, float_cnt, string_cnt, order_column_type);

	// 실수 2차원 배열 할당
	double** ptr_ptr_double;
	ptr_ptr_double = (double**)malloc(sizeof(double*) * total_row);

	// 정수 2차원 배열 할당
	int** ptr_ptr_int;
	ptr_ptr_int = (int**)malloc(sizeof(int*) * total_row);

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

	for (int i = 0; i < total_row; i++)
	{
		ptr_ptr_int[i] = (int*)malloc(sizeof(int) * int_cnt);
		ptr_ptr_double[i] = (double*)malloc(sizeof(double) * double_cnt);

		for (int j = 0; j < int_cnt; j++)
			ptr_ptr_int[i][j] = 0;
		for (int j = 0; j < double_cnt; j++)
			ptr_ptr_double[i][j] = 0.0;
	}

	// 주의 사항
	// 1. csv는 ANSI(아스키코드)와 호환된다. csv을 저장할 때 유니코드가 아닌 아스키코드로 저장할 것
	//		(1.) 을 지키지 않을 경우 문자열이 깨져 한글이 아닌 쓰레기값을 가져온다.

	FILE* fpR = fopen(filename, "r");
	char line[MAX];
	line[0] = '\0';

	fgets(line, MAX, fpR); // 첫 줄은 각 column의 카테고리 이름이므로 2차원 배열을 만들어 저장한다.

	char* header_ptr = strtok(line, ",");

	char** ptr_ptr_header = (char**)malloc(sizeof(char*) * total_column);
	for (int i = 0; i < total_column; i++)
	{
		ptr_ptr_header[i] = (char*)malloc(sizeof(char) * strlen(header_ptr) + 1); // 각 column별 이름의 바이트 크기를 구해 각각 다른 크기를 할당한다.
		strcpy(ptr_ptr_header[i], header_ptr);
		//printf("%s\n", ptr_ptr_header[i]);
		header_ptr = strtok(NULL, ",");
	}
	
	str_clear(line, MAX);

	int index = 0;
	int i = 0, j = 0, k = 0, l = 0;
	while (feof(fpR) == 0)
	{
		fgets(line, MAX, fpR);
		char* ptr = strtok(line, ",");
		while(ptr != NULL)
		{
			switch (order_column_type[index]) 
			{
			// 정수
			case '0':
				sscanf(ptr, "%d", &ptr_ptr_int[l][i]);
				i++;
				break;

				// 실수
			case '1':
				sscanf(ptr, "%lf", &ptr_ptr_double[l][j]);
				j++;
				break;

				// 문자열
			case '2':
				strcpy(ptr_ptr_string[l][k], ptr);
				break;
			}
			ptr = strtok(NULL, ",");
			index++;
		}
		str_clear(line, MAX);
		index = 0, i = 0, j = 0, k = 0;
		l++;
	}
	
	fclose(fpR);
	system("cls");
	printf("모든 csv 데이터를 가져왔습니다!\n\n");
	print_csv(ptr_ptr_int, ptr_ptr_double, ptr_ptr_string, ptr_ptr_header, total_column, total_row, order_column_type);
	printf("\n\n1. 특정 열(column) 계산기\t2. 특정 행(row) 또는 열(column) 추출하기");
	for (int i = 0; i < total_row; i++)
	{
		free(ptr_ptr_int[i]);
		free(ptr_ptr_double[i]);
	}
	free(ptr_ptr_int);
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
	char filename[MAX + 10]; // + ./csv/*.txt(=10bytes)
	mkdir_csv(); // csv 디렉토리가 있는지
	exist_csv_file(filename); // csv 파일이 있는지

	csv_format(filename); // csv파일 format 묻기
	return 0;
}