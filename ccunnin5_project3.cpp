/*
Name: Conor Cunningham
Lab: Project 3
Compiling instructions: g++ -lpthreads ccunnin5_project3.c

Deviations from Design Document:
-uses 2d vectors to store matrices gotten from file
-uses 3d vector to store output matrix
-struct is given pointers to output matrix only
-did not use semaphores
*/

#include <iostream>
#include <pthread.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdlib.h>

using namespace std;

typedef struct information {
	vector< vector< vector<int> > > *output_matrix;
	vector< vector<int> > input_matrixA;
	vector< vector<int> > input_matrixB;
	int indexO;
	int indexA;
	int indexB;
	int indexC;
	information() {
		indexA = 0;
		indexB = 0;
		indexC = 0;
		indexO = 0;
	}
} info;

void* multiply(void *id);

// Requests file name for program to use. Opens files, semaphores, and creates threads.
int main(int argc, char const *argv[]) {

	ifstream fileInput;
	ofstream fileOutput;
	int ret, exit;
	int ACol = 0;
	int ARow = 0;
	int BCol = 0;
	int BRow = 0;
	char input[255];							//buffers for scanf
	char output[255];							//buffers for scanf	
	vector< vector<int> > AMat;					//vector for first matrix
	vector< vector<int> > BMat;					//vector for second matrix
	vector< vector< vector<int> > > OMat;		//vector for output matrix

	printf("Enter the name of the input file (cannot exceed 255 chars): ");
	fgets(input, sizeof(input), stdin);
	size_t len = strlen(input) - 1; 			//removes newline from input
	if (input[len] == '\n') input[len] = '\0';

	printf("Enter the name of the output file (cannot exceed 255 chars): ");
	fgets(output, sizeof(output), stdin);
	len = strlen(output) - 1; 					//removes newline from input
	if (output[len] == '\n') output[len] = '\0';

	fileInput.open(input);
	fileOutput.open(output);

	if (!fileInput) {
		perror("Error opening input file for reading.");
		return 1;
	}

	if (!fileOutput) {
		perror("Error opening output file for reading.");
		return 1;
	}

	string line;
	int new_matrix = 1;
	int element_count = 1;
	string temp_element;
	int element; 
	int skip = 0;
	stringstream linestream;
	while (getline(fileInput, line)) {
		vector<int> row;
    	linestream.str(line);
		while (linestream >> temp_element) {
			if (temp_element.at(0) == '*') {
				new_matrix = 0;
				skip = 1;
			}
			else {
				element = atoi(temp_element.c_str());
				row.push_back(element);
			}
		}
		if (!skip) {
			if (new_matrix) {
				AMat.push_back(row);
			} else {
				BMat.push_back(row);
			}
		}
		skip = 0;
		linestream.clear();	
	}

	ACol = AMat[0].size();
	ARow = AMat.size();
	BCol = BMat[0].size();
	BRow = BMat.size();


	// check matrix values	
	// cout << "A Row: " << ARow << endl;
	// cout << "A Col: " << ACol << endl;
	// cout << "B Row: " << BRow << endl;
	// cout << "B Col: " << BCol << endl;

	//instantiate all values to 0 for output matrix
	for (int i = 0; i < ARow; i++) {
		vector< vector<int> > output_row;
		for (int j = 0; j < BCol; j++) {
			vector<int> add_nums;
			for (int k = 0; k < BRow; k++) {
				add_nums.push_back(0);
			}
			output_row.push_back(add_nums);
		}
		OMat.push_back(output_row);
	}

    pthread_t thr_id[ARow][BCol][BRow];
	if (ACol == BRow) { 
		for (int i = 0; i < ARow; i++) {
			for (int j = 0; j < BCol; j++) {
				for(int k = 0; k < BRow; k++){
		 			info *data = new info;
					data->output_matrix = &OMat;
					data->input_matrixA = AMat;
					data->input_matrixB = BMat;
					data->indexO = 0;
					data->indexA = i;
					data->indexB = j;
					data->indexC = k;
					ret = pthread_create(&thr_id[i][j][k], NULL, &multiply, (void*) data);
					if (ret != 0) {
						perror("Error: pthread_create() on cross_product failed\n");
						free (data);
						return 1;
					}
				}
			}
		}
		for (int i = 0; i < ARow; i++) {
			for (int j = 0; j < BCol; j++) {
				for(int k = 0; k < BRow; k++){
					pthread_join(thr_id[i][j][k], NULL);
				}
			}
		}
	} else {
		cout << "Cannot perform the matrix multiplication. The columns in Matrix A are not equal to the rows in Matrix B." << endl;
		fileInput.close();
		fileOutput.close();
		return 0;
	}


	vector< vector< vector<int> > >::iterator row; 
	vector< vector<int> >::iterator col; 
	vector<int>::iterator add_nums; 
	for (row = OMat.begin(); row != OMat.end(); row++) { 
		for (col = row->begin(); col != row->end(); col++) { 
			int num = 0;
			for (add_nums = col->begin(); add_nums != col->end(); add_nums++) { 
				num += *add_nums;
			} 
			cout << num << " ";
			fileOutput << num << " ";
		} 
		cout << endl;
		fileOutput << endl;
	}


	printf("PRESS ANY KEY AND ENTER TO EXIT PROGRAM: \n");
	scanf("%x", &exit);
	fileInput.close();
	fileOutput.close();
	return 0;
}

void* multiply(void *data) {	
    info *matrix_data = (info*)data; 
    matrix_data->output_matrix->at(matrix_data->indexA).at(matrix_data->indexB).at(matrix_data->indexC) = (matrix_data->input_matrixA.at(matrix_data->indexA).at(matrix_data->indexC) * matrix_data->input_matrixB.at(matrix_data->indexC).at(matrix_data->indexB));	
	delete matrix_data;
	return 0;
}
