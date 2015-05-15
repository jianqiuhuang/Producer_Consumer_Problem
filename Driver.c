#include"source.h"
const static int DEBUG = 0;

int main(int argc, char **argv){
	
	//Random number seed
	srand(time(NULL));
	extern int optind;
	int c, tmp;
	int num_assignings = 10,
	min_prof_wait = 1,
	max_prof_wait = 5,
	min_num_assignments = 1,
	max_num_assignments = 10,
	min_assignment_hours = 1,
	max_assignment_hours = 5,
	num_professors = 2,
	num_students = 2,
	students_per_assignment,
	queue_size = 8;
	
	int dflag = 0;

	while((c = getopt(argc, argv, "a:w:W:n:N:h:H:p:s:d:q:")) != -1){
		switch(c){
			case 'a':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 100000)){
					fprintf(stderr, "Invalid input for <num_assignings>: 1~100000\n");
					usage();
				}
				num_assignings = tmp;
				break;
			case 'w':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 10)){
					fprintf(stderr, "Invalid input for <min_prof_wait>: 1~10\n");
					usage();
				}
				min_prof_wait = tmp;
				break;
			case 'W':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 100)){
					fprintf(stderr, "Invalid input for <max_prof_wait>: 1~100\n");
					usage();
				}
				max_prof_wait = tmp;
				break;
			case 'n':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 10)){
					fprintf(stderr, "Invalid input for <min_num_assignments>: 1~10\n");
					usage();
				}
				min_num_assignments = tmp;
				break;
			case 'N':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 100)){
					fprintf(stderr, "Invalid input for <max_num_assignments>: 1~100\n");
					usage();
				}
				max_num_assignments = tmp;
				break;
			case 'h':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 6)){
					fprintf(stderr, "Invalid input for <min_assignment_hours>: 1~6\n");
					usage();
				}
				min_assignment_hours = tmp;
				break;
			case 'H':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 10)){
					fprintf(stderr, "Invalid input for <max_assignment_hours>: 1~10\n");
					usage();
				}
				max_assignment_hours = tmp;
				break;
			case 'p':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 10)){
					fprintf(stderr, "Invalid input for <num_professors>: 1~10\n");
					usage();
				}
				num_professors = tmp;
				break;
			case 's':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 10)){
					fprintf(stderr, "Invalid input for <num_students>: 1~10\n");
					usage();
				}
				num_students = tmp;
				break;
			case 'd':
				tmp = atoi(optarg);

				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 10)){
					fprintf(stderr, "Invalid input for <students_per_assignment>: 1~10\n");
					usage();
				}
				dflag = 1;
				students_per_assignment = tmp;
				break;
			case 'q':
				tmp = atoi(optarg);
				if((tmp == 0 && optarg[0] != '0') || (tmp < 1 || tmp > 256)){
					fprintf(stderr, "Invalid input for <queue_size>: 1~256\n");
					usage();
				}
				queue_size = tmp;
				break;
			default:
				fprintf(stderr, "Invalid option\n");
				usage();
		}
	}
	//If students_per_assignment is not set by user, set it to num_students
	if(!dflag){
		students_per_assignment = num_students;
	}
	//Check min & max constraints
	if(min_prof_wait > max_prof_wait){
		fprintf(stderr, "min_prof_wait cannot be greater than max_prof_wait\n");
		exit(1);
	}
	if(min_num_assignments > max_num_assignments){
		fprintf(stderr, "min_num_assignments cannot be greater than max_num_assignments\n");	
		exit(1);
	}
	if(min_assignment_hours > max_assignment_hours){
		fprintf(stderr, "min_assignment_hours cannot be greater than max_assignment_hours\n");
	}
	if(DEBUG){
	fprintf(stdout, "num_assignings: %d\nmin_prof_wait: %d\nmax_prof_wait: %d\nmin_num_assignments: %d\nmax_num_assignments: %d\nmin_assignment_hours: %d\nmax_assignment_hours: %d\nnum_professors: %d\nnum_students: %d\nstudents_per_assignment: %d\nqueue_size: %d\n",num_assignings, min_prof_wait, max_prof_wait, min_num_assignments, max_num_assignments,min_assignment_hours, max_assignment_hours, num_professors, num_students,students_per_assignment, queue_size);
	}
	
	//First element holds professor id or student id
	int write_param[] = {0, num_assignings, min_prof_wait, max_prof_wait, min_num_assignments, max_num_assignments, min_assignment_hours, max_assignment_hours, students_per_assignment, queue_size};
	int read_param[] = {0, queue_size, num_professors, num_students};

	init(queue_size, num_professors, num_students, write_param, read_param);
	//print function for debugging purposes
	if(DEBUG) print(queue_size);
	clear(num_professors, num_students);
	//print function for debugging purposes
	if(DEBUG) print(queue_size);
	return 0;
}
