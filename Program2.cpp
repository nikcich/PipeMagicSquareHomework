// g++ -pthread ./Program2.cpp -o Program2     -pthread fixes linker problem
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h> 
#include <time.h>

using namespace std;

// Defines some constants to use
#define GUESTS 5 // Number of people around the table
#define EATING 0 // To mark what state the guest is in, mark the eating state
#define HUNGRY 1 //  mark the eating state
#define PHILOSOPHISING 2 // mark the philosophising state

//Prototypes
void eat(int id);
void Decide_Action(int id);
void* Phil_Thread(void* id);


// Semaphores
sem_t sem;
sem_t guest_sem[GUESTS];


int philosophers[GUESTS] = { 0, 1, 2, 3, 4 }; // position on table
int guest_states[GUESTS]; // List of guest states

// Stats to save from running
int stats_number_eats[] = {0,0,0,0,0};
int stats_number_eat_times[] = {0,0,0,0,0};

int stats_number_think[] = {0,0,0,0,0};
int stats_number_think_times[] = {0,0,0,0,0};

// to store the beginning time
long long BEGIN_TIME;

// How long to run for (5 minutes in assignment document)
double run_time = 5.0; // minutes

int main(int argc, char *argv[]){

    srand(time(NULL)); // seed random numbers

    BEGIN_TIME = time(NULL); // get current time

    pthread_t thread_id_arr[GUESTS]; // creates an array for the pthreads

    sem_init(&sem, 0, 1); // Initializes the sem semaphore

    for( int i = 0; i < GUESTS; i++){ // Loops for each guest
        sem_init(&guest_sem[i], 0, 0); // Initializes the semaphore associated with this guest
    }

    for(int i = 0; i < GUESTS; i++){ // Loops for each guest
        pthread_create(&thread_id_arr[i], NULL, Phil_Thread, &philosophers[i]); // &philosophers[i] instead of i if its not working
        cout << "Philosopher number " << i+1 << " is currently philosophising..." << endl; // displays that the guest is philosophising to begin
    }

    for(int i = 0; i < GUESTS; i++){ // Loops through the guests
        pthread_join(thread_id_arr[i], NULL); // gets the thread associated with that guest and joins it back into the process
    }

    for(int i = 0; i < GUESTS; i++){ // loops through guests
        // displays how long and how many times the philosopher ate for
        cout << "Philosopher " << i+1 << " spent " << stats_number_eat_times[i] << "ms eating and ate a total of " << stats_number_eats[i] << " times." << endl;
    }

    for(int i = 0; i < GUESTS; i++){ // loops through guests
        // displays how long and how many times the philosopher ate for
        cout << "Philosopher " << i+1 << " spent " << stats_number_think_times[i] << "ms thinking and thought a total of " << stats_number_think[i] << " times." << endl;
    }

    return 0;
}

void* Phil_Thread(void* id){ // Philosopher handler
    while(time(NULL) < BEGIN_TIME+(60*run_time)){ // Repeat forever
        int* thread_id = (int*)id; // pull the thread id
        //sleep(1); // wait a second
        Decide_Action(*thread_id); // call decide action function and send thread id
    }
}

void pick_up(int id){ // pick up fork function
    sem_wait(&sem); // blocks
    // state that hungry
    guest_states[id] = HUNGRY;

    cout << "Philosopher " << id+1 << "is hungry"<< endl;
 
    // eat if neighbours are not eating
    eat(id);
 
    sem_post(&sem); // unblocks
 
    // if unable to eat wait to be signalled
    sem_wait(&guest_sem[id]); // blocks
 
    int milliseconds = (rand() % 24) + 25; // random sleep times
    int microseconds = 1000*milliseconds;
    usleep(microseconds); // sleeps for microseconds
}

void put_down(int id){
    sem_wait(&sem); // blocks
 
    // state that thinking
    guest_states[id] = PHILOSOPHISING;
 
    cout << "Philosopher " << id+1 << "is thinking" << endl; // displays they are thinking


    int LEFT = (id + 4) % GUESTS; // get left guest
    int RIGHT = (id + 1) % GUESTS; // get right guest

    eat(LEFT); // call eat function for left guest
    eat(RIGHT); // call eat function for right guest
 
    sem_post(&sem); // unblock
}

void Decide_Action(int id){
    ///// Pick Up utensiles ////////
    pick_up(id);
    ///// Drop utensiles ///////////
    put_down(id);
    ////////////////////////////////

    int milliseconds = (rand() % 24) + 25; // get random sleep time
    int microseconds = 1000*milliseconds;
    usleep(microseconds); // sleep for microseconds
    
    stats_number_think[id] = stats_number_eats[id]+1; // add to number of times ate
    stats_number_think_times[id] = stats_number_eat_times[id] + milliseconds; // add to time amount eating
}

void eat(int id){
    int LEFT = (id + 4) % GUESTS; // get left guest
    int RIGHT = (id + 1) % GUESTS; // get right guest
    if(guest_states[id] == HUNGRY && guest_states[LEFT] != EATING && guest_states[RIGHT] != EATING){ // if hungry and left and right guests not eating
        guest_states[id] = EATING; // Set the philosophers state to eating

        int milliseconds = (rand() % 24) + 25; // get random time to sleep
        int microseconds = 1000*milliseconds;
        usleep(microseconds); // sleep for microseconds
        stats_number_eats[id] = stats_number_eats[id]+1; // add stats
        stats_number_eat_times[id] = stats_number_eat_times[id] + milliseconds; // add stats


        cout << "Philosopher " << id+1 << " is eating..." << endl; // display that the philosopher is eating
        sem_post(&guest_sem[id]); // Unblock
    }
}