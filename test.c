#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <Windows.h>
#include <io.h> // access() : �����̳� ���丮(����)�� ���� ����, Ȥ�� ������ ���� �� �Ӽ��� ������ �� ����Ѵ�.

#define MAX 128

void str_clear(char string[], int size)
{
	for (int i = 0; i < size; i++)
		string[i] = '\0';
}

// csv���� ��θ� �����ϸ� column���� ��ȯ��.
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

// csv���� ��θ� �����ϸ� row���� ��ȯ��.
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

	return row_cnt - 1; // feof()�� �Լ��� ���� ������ ������ ���� �ƴ� ������ \n�̴�. �׷��Ƿ� \n�� ã����� �ݺ��Ѵ�. ��� : ������ �ٿ� �����ص� �� ���� �ݺ��ϹǷ� 1�� ����� �Ѵ�.
}


// ���� 0, �Ǽ� 1, ���ڿ� 2, None -1
int check_type(char ptr[])
{
	// ���� = 0
	if (strcmp("����", ptr) == 0) {
		return 0;
	}
	// �Ǽ� = 1
	else if (strcmp("�Ǽ�", ptr) == 0) {
		return 1;
	}
	// ���ڿ� = 2
	else if (strcmp("���ڿ�", ptr) == 0) {
		return 2;
	}
	// None = -1
	else
		return -1;
}

void mkdir_csv() {
	int nResult = _access("./csv", 0);
	// csv ���丮�� ���ٸ� ����� 1�� ��ȯ��
	// �ִٸ� 0�� ��ȯ��.
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
	printf("< csv ���丮�� �ִ� *.csv ���� >\n\n");
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
			case '0': // ����
				printf("%-10d\t", ptr_ptr_int[row_index][i]);
				i++;
				break;

			case '1': // �Ǽ�
				printf("%-10lf\t", ptr_ptr_double[row_index][j]);
				j++;
				break;

			case '2': // ���ڿ�
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

	printf("csv�� �� ���� ���� �� �Դϱ�? : ");
	scanf_s("%d", &total_column);


	printf("csv�� �� ���� ���� �� �Դϱ�? : ");
	scanf_s("%d", &total_row);*/

	total_column = auto_find_column(filename);
	total_row = auto_find_row(filename);

	char* order_column_type = (char*)malloc(sizeof(char) * total_column + 1);

	char type_name[7];
	type_name[0] = '\0';

	system("cls");
	printf("\t       <�ڷ��� ����>\n\n����\t�Ǽ�\t���ڿ�\n\n����, �ڷ����� '����'��� '����'�� �Է��ϼ���.\n\n");
	for (int i = 0; i < total_column; i++)
	{
		printf("%d ��° ���� ������ �ڷ����� �����Դϱ�?\n>>> ", i + 1);
		scanf_s("%s", type_name, sizeof(type_name));
		return_value = check_type(type_name);
		switch (return_value)
		{
		// ����
		case 0:
			int_cnt++;
			*(order_column_type + i) = '0';
			break;

		// �Ǽ�
		case 1:
			double_cnt++;
			*(order_column_type + i) = '1';
			break;

		// ���ڿ�
		case 2:
			string_cnt++;
			*(order_column_type + i) = '2';
			break;

		// None
		default:
			printf("\n�߸� �Է� �ϼ̽��ϴ�. �ٽ� �Է����ּ���.\n\n");
			i--;
			break;
		}
	}
	order_column_type[total_column] = '\0';

	//printf("int_cnt = %d, float_cnt = %d, string_cnt = %d\n\n%s", int_cnt, float_cnt, string_cnt, order_column_type);

	// �Ǽ� 2���� �迭 �Ҵ�
	double** ptr_ptr_double;
	ptr_ptr_double = (double**)malloc(sizeof(double*) * total_row);

	// ���� 2���� �迭 �Ҵ�
	int** ptr_ptr_int;
	ptr_ptr_int = (int**)malloc(sizeof(int*) * total_row);

	// ���ڿ� 3���� �迭 �Ҵ�
	char*** ptr_ptr_string;
	ptr_ptr_string = (char***)malloc(sizeof(char**) * total_row);
	for (int i = 0; i < total_row; i++)
	{
		ptr_ptr_string[i] = (char**)malloc(sizeof(char*) * string_cnt);
		for (int j = 0; j < string_cnt; j++)
		{
			ptr_ptr_string[i][j] = (char*)malloc(sizeof(char) * MAX + 1); // ������ �迭 ĭ�� NULL���� ���
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

	// ���� ����
	// 1. csv�� ANSI(�ƽ�Ű�ڵ�)�� ȣȯ�ȴ�. csv�� ������ �� �����ڵ尡 �ƴ� �ƽ�Ű�ڵ�� ������ ��
	//		(1.) �� ��Ű�� ���� ��� ���ڿ��� ���� �ѱ��� �ƴ� �����Ⱚ�� �����´�.

	FILE* fpR = fopen(filename, "r");
	char line[MAX];
	line[0] = '\0';

	fgets(line, MAX, fpR); // ù ���� �� column�� ī�װ� �̸��̹Ƿ� 2���� �迭�� ����� �����Ѵ�.

	char* header_ptr = strtok(line, ",");

	char** ptr_ptr_header = (char**)malloc(sizeof(char*) * total_column);
	for (int i = 0; i < total_column; i++)
	{
		ptr_ptr_header[i] = (char*)malloc(sizeof(char) * strlen(header_ptr) + 1); // �� column�� �̸��� ����Ʈ ũ�⸦ ���� ���� �ٸ� ũ�⸦ �Ҵ��Ѵ�.
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
			// ����
			case '0':
				sscanf(ptr, "%d", &ptr_ptr_int[l][i]);
				i++;
				break;

				// �Ǽ�
			case '1':
				sscanf(ptr, "%lf", &ptr_ptr_double[l][j]);
				j++;
				break;

				// ���ڿ�
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
	printf("��� csv �����͸� �����Խ��ϴ�!\n\n");
	print_csv(ptr_ptr_int, ptr_ptr_double, ptr_ptr_string, ptr_ptr_header, total_column, total_row, order_column_type);
	printf("\n\n1. Ư�� ��(column) ����\t2. Ư�� ��(row) �Ǵ� ��(column) �����ϱ�");
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


// ������ ������ �ٷ� ����ȴ�.

int main() {
	char filename[MAX + 10]; // + ./csv/*.txt(=10bytes)
	mkdir_csv(); // csv ���丮�� �ִ���
	exist_csv_file(filename); // csv ������ �ִ���

	csv_format(filename); // csv���� format ����
	return 0;
}