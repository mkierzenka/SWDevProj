//lang::C++

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <chrono>

#include "dataframe.h"
#include "helper.h"
#include "lengthrower.h"
#include "findrower.h"

/**
 * This is a benchmarking runner for the DataFrame class. Specifically, by running two different
 * rowers on a large DataFrame.
 * First step is to read in data from csv file (13 columns, 1100000 rows == ~100MB)
 * Then benchmarks runtime performance.
 * Part 1: singlethreaded, multithreaded
 * Part 2: singlethreaded, multithreaded
 *
 * @author kierzenka.m@husky.neu.edu & broder.c@husky.neu.edu
 */

/**
 * Takes element char* and its expected type. Sets specified index to the typed element.
 * No extra checks on types/conversions than those found in Row.
 */
void addToRow(Row* row, const char* elemStr, const char type, size_t idx) {
	switch (type) {
		case 'I':
			row->set(idx, atoi(elemStr));
			break;
		case 'F':
			row->set(idx, static_cast<float>(atof(elemStr))); // so it isn't interpretted as double
			break;
		case 'S':
		{
			String* tmp = new String(elemStr);
			row->set(idx, tmp);
			delete tmp;
			break;
		}
		case 'B':
			row->set(idx, atoi(elemStr) == 1);
			break;
		default:
			puts("bad");
	}
}

/**
 * Helper to get filesize of a file per name. Assume file exists
 */
size_t getFilesize(const char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

/**
 * Sets last column (expected to be int type) to value across whole df
 */
void setLastCol(DataFrame* df, int val) {
	for (size_t i = 0; i < df->nrows(); i++) {
		df->set(df->ncols() - 1, i, val);
	}
}

/**
 * Takes datafile (csv) and puts the values into the data frame. Assumed that the types/schemas 
 * of file and df match.
 */
// inspiration: https://techoverflow.net/2013/08/21/a-simple-mmap-readonly-example/
void parseDataFile(DataFrame* df, const char* dataFilename, size_t numLinesToRead) {
	size_t filesize = getFilesize(dataFilename);
    //Open file
    int fd = open(dataFilename, O_RDONLY, 0);
    assert(fd != -1);
    //Execute mmap
    char* mmappedData = (char*)mmap(NULL, filesize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    assert(mmappedData != MAP_FAILED);
	
	Schema& scm = df->get_schema();
	Row* row = new Row(scm);

	size_t numRowsAdded = 0;
	  
	char* curElem = new char[48];// just one element char*
	size_t curIdx = 0; //where you are in file
	size_t tmpIdx = 0; //where you are in curElem
	size_t colIdx = 0;
	while (mmappedData[curIdx] != '\0' && numRowsAdded < numLinesToRead) {
		memset(curElem, 0, 48);
		tmpIdx = 0;
		colIdx = 0;
		while (mmappedData[curIdx] != '\n' && mmappedData[curIdx] != '\r') {
			curElem[tmpIdx] = mmappedData[curIdx];
			curIdx++;
			tmpIdx++;
			
			if (mmappedData[curIdx] == ',') {
				curElem[tmpIdx] = 0;
				addToRow(row, curElem, scm.col_type(colIdx), colIdx);
				colIdx++;
				curIdx++;
				tmpIdx = 0;
				memset(curElem, 0, 48);
			}
		}
		curElem[tmpIdx] = 0;
		curIdx++;
		tmpIdx = 0;
		addToRow(row, curElem, scm.col_type(colIdx), colIdx);
		df->add_row(*row);
		numRowsAdded++;
	}
	
	setLastCol(df, 0);

	//Cleanup
    int rc = munmap(mmappedData, filesize);
    assert(rc == 0);
    close(fd);
}

void validateDF(DataFrame* df) {
  assert(df->get_int(0, 0) == -343033);
  assert(df->get_bool(1, 0) == false);
  assert(df->get_string(2, 0)->equals(new String("inflexive")));
  assert(df->get_float(3, 0) == -8184.767578f);
  assert(df->get_string(6, 675876)->equals(new String("Cynurus")));
  assert(df->get_float(11, 961309) == 8329.041016f);
  assert(df->get_bool(5, 1099999) == true);
  
}

void validateHaystack(DataFrame* df) {
	assert(df->nrows() == 100);
	assert(df->ncols() == 9);
	assert(df->get_int(0, 0) == 1573390);
	assert(df->get_string(1, 0)->equals(new String("crawl's")));
	assert(df->get_bool(2, 0) == true);
	assert(df->get_float(3, 0) == -5871.645508f);
	assert(df->get_string(5, 39)->equals(new String("Marist")));
	assert(df->get_int(4, 85) == -905910);
	assert(df->get_float(7, 92) == -3794.611572f);
}

int main()
{
  // Part 0 -- Reading datafile.txt into the DataFrame df
  const char* dataFilename = "datafile.txt";
  Schema* sch = new Schema("IBSFIBSFIBSFI"); // assumed file follows this
  DataFrame* df = new DataFrame(*sch);
  size_t lastColIdx = df->ncols() - 1;
  std::cout << "\nReading in data file: " << dataFilename << "\n";
  size_t expectedNumRows = 1100000;
  parseDataFile(df, dataFilename, expectedNumRows);
  assert(df->nrows() == expectedNumRows);
  assert(df->ncols() == 13);
  validateDF(df); // Crash if data isn't read in correctly

  std::cout << "Benchmarking...\n";

  // FindRower test, singlethreaded
  Schema* haystackSchema = new Schema("ISBFISBFI");
  DataFrame* haystackFrame = new DataFrame(*haystackSchema);
  const char* haystackFilename = "datafile_haystack.txt";
  parseDataFile(haystackFrame, haystackFilename, 100);
  validateHaystack(haystackFrame);

  // zero out last column
  setLastCol(df, 0);
  assert(df->get_int(12, 0) != 3);
  assert(df->get_int(12, 122995) != 4);
  assert(df->get_int(12, 1057134) != 5);
  
  FindRower* findRower = new FindRower(haystackFrame, df);
  auto startTime = std::chrono::high_resolution_clock::now();
  df->map(*findRower);
  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

  //some checks to make sure rower worked properly
  assert(df->get_int(12, 0) == 3);
  assert(df->get_int(12, 122995) == 4);
  assert(df->get_int(12, 1057134) == 5);
  std::cout << "FindRower singlethreaded duration (microseconds): " << duration.count() << std::endl;

  // zero out last column
  setLastCol(df, 0);
  assert(df->get_int(12, 0) != 3);
  assert(df->get_int(12, 122995) != 4);
  assert(df->get_int(12, 1057134) != 5);


  // FindRower test, multithreaded
  startTime = std::chrono::high_resolution_clock::now();
  df->pmap(*findRower);
  endTime = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
  
  //some checks to make sure rower worked properly
  assert(df->get_int(12, 0) == 3);
  assert(df->get_int(12, 122995) == 4);
  assert(df->get_int(12, 1057134) == 5);
  std::cout << "FindRower multithreaded duration (microseconds): " << duration.count() << std::endl;

  std::cout << "Benchmarking complete\n";

  delete findRower;
  delete haystackSchema;
  delete haystackFrame;
  delete df;
  
  return 0;
}
