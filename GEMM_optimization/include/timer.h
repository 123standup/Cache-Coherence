#include <time.h>

void start_timer(struct timespec *start) {
    if( clock_gettime( CLOCK_REALTIME, start) == -1 ) {
          perror( "clock gettime" );
          exit( EXIT_FAILURE );
    }
}

void stop_timer(struct timespec *start) {
    struct timespec stop;
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }

    // (stop.tv_nsec - start->tv_nsec) could be negative
    double elapsed = (stop.tv_sec - start->tv_sec) * 1000 +
                     (double) (stop.tv_nsec - start->tv_nsec) / 1000 / 1000;
    fprintf(stderr, "%f ms", elapsed);
}