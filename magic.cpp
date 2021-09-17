#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <typeinfo>
#include <stdlib.h>
#include <queue>

using namespace std;

#define TEST_STRING "2 7 6 9 5 1 4 3 8"

bool print_stuff(int my_matrix[][3]);
bool isMagicSquare(int mat[3][3]);

# define my_sizeof(type) ((char *)(&type+1)-(char*)(&type))

int main(){
    queue<int> *my_queue_please = new queue<int>;
    int fd[2];
    pipe(fd);

    pid_t pid;

    if((pid = fork()) == 0){ // Child process
        close(0);
        close(fd[1]);
        int myCin = dup2(fd[0], 0);

        char buf[sizeof(TEST_STRING)];

        while(cin >> buf){
            char **pEnd;
            long int num = strtol(buf, pEnd,10);
            my_queue_please->push(num);
        }

        int my_n = 0;
        int my_m = 0;
        int my_matrix[3][3];

        while(!my_queue_please->empty()){
            int x = my_queue_please->front();
            if(my_n == 3){
                my_n = 0;
                my_m++;
            }
            my_matrix[my_m][my_n] = x;
            my_n++;
            my_queue_please->pop();
        }

        bool rv = print_stuff(my_matrix);
        
    }else{ // Parent process
        write(fd[1], TEST_STRING, sizeof(TEST_STRING));
        close(fd[1]);

        waitpid(pid, NULL, 0);
        cout << "Hello from the parent process" << endl;
    }

    close(0);

    return 0;
}

bool print_stuff(int my_matrix[][3]){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            cout << my_matrix[i][j] << " ";
        }
        cout << endl;
    }

    bool ismag = isMagicSquare(my_matrix);

    return ismag;
    
}

bool isMagicSquare(int mat[3][3]){
    int n = my_sizeof(mat)/my_sizeof(mat[0]);
    cout << n << endl;
    int i=0,j=0;
    int sumd1 = 0, sumd2=0;

    for (i = 0; i < n; i++){
        sumd1 += mat[i][i];
        sumd2 += mat[i][n-1-i];
    }

    if(sumd1!=sumd2)
        return false;
 
    for (i = 0; i < n; i++){
        int rowSum = 0, colSum = 0;   
        for (j = 0; j < n; j++){
            rowSum += mat[i][j];
            colSum += mat[j][i];
        }
        if (rowSum != colSum || colSum != sumd1)
            return false;
    }
   return true;
}
