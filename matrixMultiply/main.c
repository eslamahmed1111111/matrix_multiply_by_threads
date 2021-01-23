#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


struct address
{
int **arrA ;
int **arrB ;
int **arrC ;
int **arrD ;
int rowC;
int colC;
int colA;
};
//for chaning rows of first method
int step = 0;

void* multi(void* arg)
{
    struct address *p = (struct address*)arg;
    int sum=0;
	// Each thread computes a row of matrix multiplication
	for (int i = step ; i < (step + 1)  ; i++){
		for (int j = 0; j < p->colC; j++){
			for (int k = 0; k < p->colA; k++){
				sum += p->arrA[i][k] * p->arrB[k][j];
			}
			p->arrC[i][j]=sum;
			sum=0;
		}
	}
        step++;
        return NULL;
}

void *mult2(void* arg)
{
    int *elements = (int *)arg;
    int k = 0, i = 0;
    //(x) number of elements , (k) the element in arrD
    int x = elements[0];
    for (i = 1; i <= x; i++)
           k += elements[i]*elements[i+x];

    int *ret_val = (int*)malloc(sizeof(int));
        *ret_val = k;

    pthread_exit(ret_val);
}
//get rows & cols from first line
void getRowsCols(char *m,int matrc[2]){

    FILE *fptr;
    if ((fptr = fopen(m, "r")) == NULL) {
        printf("Error   opening file");
        exit(1);
    }
    char line[1000];
    int i=0 , x;
    //(x) to convert from string to int
    //taking first line and split it upon space or equal signs
    fgets(line, sizeof(line), fptr);
        const char* val = strtok(line, "= ");
        while(val!=NULL){
                x=atoi(val);
                if(x!=0){
                    if(i==0){
                        matrc[0]=x;
                        i++;
                            }
                    else
                        matrc[1]=x;
                }
                val = strtok(NULL, "=  ");
        }
        fclose(fptr);
}

void getMatrix(int rows,int cols,char *m,int **mat ){

    FILE *fptr;
    if ((fptr = fopen(m, "r")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
        int i=0,j=0;
        int x;
        char line[1000];
    fgets(line, sizeof(line), fptr);

    while(!feof(fptr)){
        fgets(line, sizeof(line), fptr);
        const char* val = strtok(line, "\t");
        j=0;
        while(val!=NULL){
                x=atoi(val);
                if( (x!=0 || strcmp(val,"0")==0 )&& i<rows && j<cols){
                   mat[i][j]=x;
                    j++;
                }
                val = strtok(NULL, "\t");
        }
       if(cols!=(j) && j!=0){ //if there is missed elements
           printf("ERROR : there is elements not exist \n");
            exit(0);
       }
        i++;
    }
    fclose(fptr);
}

int main()
{
    char user_input[1000],m[100],x[100],out[100];//(m) name of first file , (x) name of second file
    printf(" enter name of files like (mat1 mat2 out) or press ENTER for default\n");
    if (fgets (user_input, sizeof (user_input), stdin) == NULL || strcmp (user_input, "\n") == 0 ) //press ENTER
    {
        strcpy(m,"a.txt");
        strcpy(x,"b.txt");
        strcpy(out,"c.txt");
    }
    else {
        user_input[ strcspn( user_input, "\n" ) ] = '\0';
        const char* val = strtok(user_input, " ");
        int z=1;
        while(val!=NULL){
                if(z==1)
                    strcpy(m,val);
                else if(z==2)
                    strcpy(x,val);
                else if(z==3)
                    strcpy(out,val);
                val = strtok(NULL, " ");
                z++;
        }
            }

    int matrc1[2]; //for saving rows and columns
    getRowsCols(m,matrc1);

    struct address p;
    struct timeval stop,start;

     p.arrA= (int **)malloc(matrc1[0] * sizeof(int *)); //making 2d array for A
        for (int i=0; i<matrc1[0]; i++)
             p.arrA[i] = (int *)malloc(matrc1[1] * sizeof(int));

     getMatrix(matrc1[0],matrc1[1],m,p.arrA);
        p.rowC=matrc1[0];
        p.colA=matrc1[1];

        getRowsCols(x,matrc1);
        if(p.colA != matrc1[0]){
            printf("ERROR : can't multiply(columns of first array != rows of second array) ");
            exit(0);
        }

        p.colC=matrc1[1];
        p.arrB= (int **)malloc(matrc1[0] * sizeof(int *));
        for (int i=0; i<matrc1[0]; i++)
             p.arrB[i] = (int *)malloc(matrc1[1] * sizeof(int));

     getMatrix(matrc1[0],matrc1[1],x,p.arrB);

    p.arrC= (int **)malloc(p.rowC * sizeof(int *));
        for (int i=0; i<p.rowC; i++)
             p.arrC[i] = (int *)malloc(p.colC * sizeof(int));

    p.arrD= (int **)malloc(p.rowC * sizeof(int *));//making 2d array for A
        for (int i=0; i<p.rowC; i++)
             p.arrD[i] = (int *)malloc(p.colC * sizeof(int));

    gettimeofday(&start,NULL);

    pthread_t threads[p.rowC];//creating number of threads equal to rows of output
	for (int i = 0; i < p.rowC; i++) {
		pthread_create(&threads[i], NULL, multi, &p);
	}
	// joining and waiting for all threads to complete
	for (int i = 0; i < p.rowC; i++)
		pthread_join(threads[i], NULL);

    gettimeofday(&stop,NULL);
    printf("\n time for first method in seconds : %lu \n",stop.tv_sec - start.tv_sec);
    printf("\n time for first method in M.S : %lu \n",stop.tv_usec - start.tv_usec);
    printf("\n number of threads is : %d \n \n \n",p.rowC);
     FILE *fptr;
    if ((fptr = fopen(out, "wb")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    fprintf(fptr,"FIRST MATRIX:\n");
    for(int i=0;i<p.rowC;i++) {
    for(int j=0;j<p.colC;j++) {
        fprintf(fptr,"%d\t",p.arrC[i][j]);
    }
    fprintf(fptr,"\n");}
    fclose(fptr);

    int max = p.rowC*p.colC; //number of threads to be created for second method
    int dataSize=p.colA+matrc1[0]+2;//number of elements in rows and columns

    gettimeofday(&start,NULL);

    pthread_t *threads2;
    threads2 = (pthread_t*)malloc(max*sizeof(pthread_t));

    int count = 0;//for threads
    int* data = NULL;//for saving data of rows and columns
    for (int i = 0; i < p.rowC; i++)
        for (int j = 0; j < p.colC; j++)
               {
               //storing row and column elements in data
            data = (int *)malloc((dataSize)*sizeof(int));
            data[0] = p.colA; //first element is number of how multiply will be done

            for (int k = 0; k < p.colA; k++) //elements of rows in first array
                data[k+1] = p.arrA[i][k];

            for (int k = 0; k < matrc1[0]; k++) //elements of columns in second array
                data[k+ p.colA+1] = p.arrB[k][j];

             //creating threads
                pthread_create(&threads2[count++], NULL,mult2, (void*)(data));

                    }
        int rowD=0,colD=0;
        for (int i = 0; i < max; i++)
            {
              void *k;//for the returning value
              pthread_join(threads2[i], &k);

              int *elemnt = (int *)k;
              p.arrD[rowD][colD]=*elemnt;
              colD++;
              if ((i + 1) % p.colC == 0) { //changing rows
                  rowD++;
                  colD=0;}
            }

    gettimeofday(&stop,NULL);
    printf("\n time for second method in seconds : %lu\n",stop.tv_sec - start.tv_sec);
    printf("\n time for second method in M.S : %lu\n",stop.tv_usec - start.tv_usec);
    printf("\n number of threads is : %d\n",p.rowC*p.colC);

    if ((fptr = fopen(out, "a")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    fprintf(fptr,"\n");
    fprintf(fptr,"SECOND MATRIX:\n");
    for(int i=0;i<p.rowC;i++) {
    for(int j=0;j<p.colC;j++) {
        fprintf(fptr,"%d\t",p.arrD[i][j]);
    }
    fprintf(fptr,"\n");}
    fclose(fptr);
    return 0;
}
