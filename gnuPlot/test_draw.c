#include <stdlib.h>
#include <stdio.h>
#define NUM_POINTS 1000
#define NUM_COMMANDS 2

int main()
{
    char * commandsForGnuplot[] = {"set title \"TITLEEEEE\"", "plot 'data.temp' with line"};
    double xvals[NUM_POINTS];// = {0.0, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0};
    double yvals[NUM_POINTS];// = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    
    int i;
    for(i = 0; i < NUM_POINTS; i++)
    {
        xvals[i] = i;
        yvals[i] = rand()%1024;
    }
    FILE * temp = fopen("data.temp", "w");
    /*Opens an interface that one can use to send commands as if they were typing into the
     *     gnuplot command line.  "The -persistent" keeps the plot open even after your
     *     C program terminates.
     */
    FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    // nt i;
    for (i=0; i < NUM_POINTS; i++)
    {
    fprintf(temp, "%lf %lf \n", xvals[i], yvals[i]); //Write the data to a temporary file
    }
    fclose(temp);
    for (i=0; i < NUM_COMMANDS; i++)
    {
    fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
    }
    pclose(gnuplotPipe);
    return 0;
}