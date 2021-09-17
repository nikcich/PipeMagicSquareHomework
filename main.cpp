#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <iostream>
#include <queue>

using namespace std;

//#define TEST_STRING "2 7 6 9 5 1 4 3 8" // MAGIC SQUARE
#define TEST_STRING "1 2 2 2 2 1 2 1 2" // NOT MAGIC SQUARE

bool isMagicSquare(int mat[3][3]);

int main(){

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    struct utsname uname_pointer;

    uname(&uname_pointer);

    cout << "System name: " << uname_pointer.sysname << endl;
    cout << "Node name: " << uname_pointer.nodename << endl;
    cout << "Release: " << uname_pointer.release << endl;
    cout << "Version: " << uname_pointer.version << endl;
    cout << "Machine: " << uname_pointer.machine << endl;
    cout << "Domain name via uname: " << uname_pointer.domainname << endl;

    // To retrieve hostname

    hostname = gethostname(hostbuffer, sizeof(hostbuffer));

    char hostbuffer2[256];
    int host_entry_two = getdomainname(hostbuffer2, sizeof(hostbuffer2));

    cout << "Domain via getdomainname: " << hostbuffer2 << endl;

    host_entry = gethostbyname(hostbuffer);

    IPbuffer = inet_ntoa(*((struct in_addr*)

    host_entry->h_addr_list[0]));

    cout << "Host name: " << hostbuffer << endl;
    cout << "Host IP: " << IPbuffer << endl;

    queue<int> *my_queue_please = new queue<int>;
  
    int fd1[2];  // Used to store two ends of first pipe
    int fd2[2];  // Used to store two ends of second pipe

    pid_t p;

    bool ismagic = false;
  
    if (pipe(fd1)==-1){
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }

    if (pipe(fd2)==-1){
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }

    p = fork();

    if (p < 0){ // error making second process
        fprintf(stderr, "fork Failed" );
        return 1;
    }else if (p > 0){ // Parent process
        close(fd1[0]);  // Close reading end of first pipe
  
        write(fd1[1], TEST_STRING, sizeof(TEST_STRING));
        close(fd1[1]);
  
        close(fd2[1]); // Close writing end of second pipe

        bool magreal;
        bool mag[256];
        void* recanswer;

        read(fd2[0], mag, sizeof(magreal));

        cout << "The given 9 digit matrix is: ";
        if(*mag){
            cout << "Magic" << endl;
        }else{
            cout << "NOT magic" << endl;
        }

        close(fd2[0]);
    }else {  // child process
        close(fd1[1]);  // Close writing end of first pipe

        char buf[sizeof(TEST_STRING)];

        read(fd1[0], buf, sizeof(TEST_STRING));

        char newbuf[sizeof(TEST_STRING)];
        int temp_i = 0;
        int idx = 0;
        while(buf[temp_i] != '\0'){
            
            if(buf[temp_i] != ' '){ // not a space
                
                newbuf[idx] = buf[temp_i];
                newbuf[idx+1] = '\0';
                idx++;
            }else{ // a space or anything else
                idx = 0;

                char **pEnd = NULL;
                long int num = strtol(newbuf, pEnd,10);
                my_queue_please->push(num);
            }
            
            temp_i++;
            if(buf[temp_i] == '\0'){
                char **pEnd = NULL;
                long int num = strtol(newbuf, pEnd,10);
                my_queue_please->push(num);
            }
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

        bool ismagic2 = isMagicSquare(my_matrix);

        close(fd1[0]);
        close(fd2[0]);
  
        void* answer = &ismagic2;
        write(fd2[1], answer, sizeof(ismagic2));
        close(fd2[1]);
  
        exit(0);
    }

    return 0;

}

bool isMagicSquare(int mat[3][3]){
    int n = 3;
    int i=0,j=0;
    int sumd1 = 0, sumd2=0;
    for (i = 0; i < n; i++){
        sumd1 += mat[i][i];
        sumd2 += mat[i][n-1-i];
    }

    if(sumd1!=sumd2)
        return false;
 
    for (i = 0; i < n; i++) {
         
        int rowSum = 0, colSum = 0;   
        for (j = 0; j < n; j++) {
            rowSum += mat[i][j];
            colSum += mat[j][i];
        }
        if (rowSum != colSum || colSum != sumd1)
            return false;
    }
   return true;
}
