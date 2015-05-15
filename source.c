#include"source.h"

struct assignment *queue = NULL;
int slot_count = 0, assign_count = 0, in = 0, out = 0, professor_count = 0;
pthread_mutex_t queue_lock, prof_id_update, stud_id_update, professor_count_update;
pthread_cond_t full, empty;
pthread_t *professors, *students;

//Print usage string
void usage(){
	fprintf(stderr, "./program2 [-a <num_assignings>]\n[-w <min_prof_wait>]\n[-W <max_prof_wait>]\n[-n <min_num_assignments>]\n[-N <max_num_assignments>]\n[-h <min_assignment_hours>]\n[-H <max_assignment_hours>]\n[-p <num_professors>]\n[-s <num_students>]\n[-d <students_per_assignment>]\n[-q<queue_size>]\n ");
	exit(1); 
}
//Initialize the queue and create all threads
void init(int queue_size, int num_professors, int num_students, int *write_param, int *read_param){
	//Allocate queue to queue_size
	queue = malloc(sizeof(struct assignment) * queue_size);
	for(int i = 0; i < queue_size; i++){
                queue[i].hours = 0;
                queue[i].prof_id = -1;
                queue[i].numberOfStudents = 0;
                queue[i].assignment_id = 0;
        }
	//Alllocate a collection of professor threads and student threads
	professors = malloc(sizeof(pthread_t) * num_professors);
        students = malloc(sizeof(pthread_t) * num_students);

	//Creating professors
        for(int i = 0; i < num_professors; i++){
                if(pthread_create(&professors[i], NULL, professor_write, write_param) != 0 ){
                        perror("pthread_create");
                        exit(1);
                }
        }

        //Creating students
        for(int i = 0; i < num_students; i++){
               if(pthread_create(&students[i], NULL, student_read, read_param) != 0 ){
                       perror("pthread_create");
                       exit(1);
               }
        }

}

//Check if all threads are finished
//Print error and exit if all threads didn't exit sucessfully
void clear(int num_professors, int num_students){

	for(int i = 0; i < num_professors; i++){
                if(pthread_join(professors[i], NULL) != 0){
                        perror("pthread_join_professor");
                        exit(1);
                }
        }
        for(int i = 0; i < num_students; i++){
                if(pthread_join(students[i], NULL) != 0){
                        perror("pthread_join_student");
                        exit(1);
                }
        }

}
//param contains: id, num_assignings, min_prof_wait, max_prof_wait, min_num_assignments, max_num_assignments, min_assignment_hours, max_assignment_hours, num_professors, num_students, students_per_assignment, queue_size
//Produce assignments and store them in queue
void *professor_write(int *param){
	//Variable to hold random values and professor id
	int assignment_count = 0, wait_time = 0, hours = 0, num_assignments = 0, id = 0;
	//Lock professor id access
	pthread_mutex_unlock(&prof_id_update);
	id = ++param[0];
	pthread_mutex_unlock(&prof_id_update);
	//Unlock prof_id_update

	printf("STARTING Professor %d\n", id);
	
	//Each professor assigning <num_assignings> times
	for(int i = 0; i < param[1]; i++){
		//Random number of assignments in range min_num_assignment ~ max_num_assignment
		num_assignments = rand() % (param[5] - param[4] + 1) + param[4];
		for(int j = 0; j < num_assignments; j++){
			wait_time = rand() % (param[3] - param[2] + 1) + param[2];
			//Pause the thread by sleep(wait_time) before putting new assignment to queue
			sleep(wait_time);
			//Random hours range in min_assignment_hours ~ max_assignment_hours
			hours = rand() % (param[7] - param[6] + 1) + param[6];

			struct assignment tmp;
			tmp.hours = hours;
			tmp.numberOfStudents = param[8];
			tmp.prof_id = id;
			tmp.assignment_id = ++assignment_count;
			
			//Lock access of the queue 
  			pthread_mutex_lock(&queue_lock);
			//queue is full wait for students to read
			while(param[9] == slot_count){
				pthread_cond_wait(&empty, &queue_lock);
			}
			fprintf(stdout, "ASSIGN Professor %d adding Assignment %d: %d Hours\n", id, assignment_count, hours);
			queue[in] = tmp;
			//Update write index, number of assginments in queue, and number of students needed to finish all assignments in queue
			in = (in + 1) % param[9];
			slot_count++;
			assign_count += param[8];
	
			//Since every assignment needs <students_per_assignment> students, signal <students_per_assignment> students that queue is ready
			for(int i = 0; i < param[8]; i++){
				//signal student the queue is not empty
				pthread_cond_signal(&full);
			}
			pthread_mutex_unlock(&queue_lock);
			//Unlock queue
		}
	}

	printf("EXITING Professor %d\n", id);

	//Count the number of professors have exited
	pthread_mutex_lock(&professor_count_update);
	professor_count++;
	pthread_mutex_unlock(&professor_count_update);

	pthread_exit(0);
}
//Param is an array of integers: id, queue_size, num_professors, num_students
//Perform assignments assigned by professors
//Must wait for all professors to exit
void *student_read(int *param){
	//Local variable to store student id#
	int id = 0;
	struct assignment tmp;
	//Lock access to param[0] -- retrieve student id
	pthread_mutex_lock(&stud_id_update);
	id = ++param[0];
	pthread_mutex_unlock(&stud_id_update);

	printf("STARTING Student %d\n", id);

//Stay in the loop while professors have not exited and there's assignments in queue
//Professors exited does not imply the queue is empty
//Queue is empty does not imply professors finished assigning
while((professor_count != param[2]) || assign_count != 0){
	//Lock queue access
	pthread_mutex_lock(&queue_lock);
	//Wait for professors to write more assignments because queue in empty
	while(assign_count == 0){
		//Special case:
		//All professors exited and currently no work in queue
		//Safe to return the lock because in order for students to modify content in the queue, assign_count must be non-zero
		if(professor_count == param[2]){
			pthread_mutex_unlock(&queue_lock);
			break;
		}
		pthread_cond_wait(&full, &queue_lock);
	}
	//Block students trying to access the queue when there is no assignments and professors are exited
	if(assign_count != 0){
		//Same assignment skip
		if(queue[out].prof_id == tmp.prof_id && queue[out].assignment_id == tmp.assignment_id){
			pthread_mutex_unlock(&queue_lock);
		}else{
			//Update the queue for students to perform assignment
			tmp = queue[out];
			queue[out].numberOfStudents--;
			assign_count--;
			//Decrement slot_count iff numberOfStudents of current assignment is zero
			//Signal professors that there's an empty slot
			if(queue[out].numberOfStudents == 0){
				out = (out + 1) % param[1];
				slot_count--;
				pthread_cond_signal(&empty);
			}
			//Unlock queue access
			pthread_mutex_unlock(&queue_lock);
		
			//Perform assignment work
			//No lock needed because assignment is stored in tmp previously
			printf("Begin Student %d working on Assignment %d from Professor %d\n", id, tmp.assignment_id, tmp.prof_id);
			for(int i = 0; i < tmp.hours; i++){
				printf("WORK Student %d working on Assignment %d Hour %d from Professor %d\n", id, tmp.assignment_id, i+1, tmp.prof_id);
				//Sleep one second for each hour of work
				sleep(1);
			}
			printf("END Student %d working on Assignment %d from Professor %d\n", id, tmp.assignment_id, tmp.prof_id);
		}
	}
}
//	signal all students dont wait on empty because all professors have exited	
	if(professor_count == param[2]){
		for(int i = 0; i < param[3]; i++)
			pthread_cond_signal(&full);
	}

	printf("EXITING Student %d\n", id);
	pthread_exit(0);
}

void print(int size){
	for(int i = 0; i < size; i++){
                printf("prof_id: %d, assigment: %d, hours: %d, #students:%d\n ", queue[i].prof_id, queue[i].assignment_id, queue[i].hours, queue[i].numberOfStudents);
        }
}
