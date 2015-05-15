#ifndef _SOURCE_H
#define _SOURCE_H

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<getopt.h>
#include<unistd.h>
#include<time.h>
#include<semaphore.h>
#include<sys/wait.h>
struct assignment{
	//number of students for this assignment
	int numberOfStudents;
	//number of hours for this assignment
	int hours;
	//professor ID
	int prof_id;
	
	int assignment_id;
};

//Print usage message and exit the program
void usage();

//Initialize the queue and create all threads
void init(int queue_size, int num_professors, int num_students, int *write_param, int *read_param);

//Check if all threads are finished
//Print error and exit if all threads didn't exit sucessfully
void clear(int num_professors, int num_students);

void print(int size);

//Professor processing function
void *professor_write();

//Student processing function
void *student_read();
#endif 

