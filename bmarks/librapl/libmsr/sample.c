#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "msr_rapl.h"
#include "msr_common.h"
#include "msr_clocks.h"


static struct rapl_state_s no_caller_rapl_state;

extern struct rapl_state_s *rs;

#define max(a,b) ((a)>(b)?(a):(b))

//static double PKG_max_watts[NUM_PACKAGES] = {0}, PP0_max_watts[NUM_PACKAGES] = {0};
uint64_t lastNonzeroTick = 0;
//static uint64_t last_raw_joules[NUM_DOMAINS], last_raw_joules_tmp[NUM_DOMAINS];
double tsc_rate = 0;
static struct timeval now;
static double currentTime = 0;
//static double joules[NUM_PACKAGES][NUM_DOMAINS];
static const uint64_t zero = 0;
static char *logFilename;

static void asyncHandler(int sig){
  static int init = 0;
  uint64_t tsc;
 if(sig != SIGALRM)
    return;

  tsc = rdtsc();
  if(!init){
    init = 1;
    lastNonzeroTick = tsc;
  }
  
  rapl_tick(rs, logFilename);

  double nzDelta = tsc_delta(&lastNonzeroTick, &tsc, &tsc_rate);

  lastNonzeroTick = tsc;

  currentTime += nzDelta;

  /*
  for(package = 0; package < NUM_PACKAGES; package++){
    fprintf(logFile, "%lf\t%15.10lf\t%15.10lf"
	    "\t%15.10lf"
	    "\n", 
	    currentTime,
	    rs.total_joules[package][PKG_DOMAIN],
	    rs.total_joules[package][PP0_DOMAIN]
#ifdef ARCH_062A
	    ,rs.total_joules[package][PP1_DOMAIN]
#endif
#ifdef ARCH_062D
	    ,rs.total_joules[package][DRAM_DOMAIN]
#endif
	    );
    PKG_max_watts[package] = max(PKG_max_watts[package], 
				 joules[package][PKG_DOMAIN]/nzDelta);
    PP0_max_watts[package] = max(PP0_max_watts[package], 
				 joules[package][PP0_DOMAIN]/nzDelta);
  }
  */
}

static void handler(int sig){
  if(msr_debug)
    printf("caught signal: %d\n", sig);
}

void sampleAsync(char *filename, unsigned int msPeriod){
  printf("\nIn sampleAsync: ");

  logFilename = filename;
 
 if(rs == NULL) {
	rs = &no_caller_rapl_state;
}


  FILE *rateFile = fopen("/tmp/tsc_rate", "r");
  //! @todo measure/read tsc rate
  if(!rateFile && errno == ENOENT){
    tsc_rate = measure_tsc();
    rateFile = fopen("/tmp/tsc_rate", "w");
    fprintf(rateFile, "%lf\n", tsc_rate);
  }else if(rateFile){
    // get rate from file
    fscanf(rateFile, "%lf", &tsc_rate);
  } else {
    perror("error opening /tmp/tsc_rate");
    exit(1);
  }
  fclose(rateFile);

  fprintf(stderr, "tsc rate: %lf\n", tsc_rate);

  /*
  fprintf(logFile, "timestamp\ttotal_pkg_J\ttotal_pp0_J\t"
#ifdef ARCH_062A
	  "pp1_J"
#endif
#ifdef ARCH_062D
	  "dram_J"
#endif
	  "\n");
  fprintf(logFile, "%lf\t%15.10lf\t%15.10lf"
	  "\t%15.10lf"
	  "\n", 
	  0.0, 0.0, 0.0
#ifdef ARCH_062A
	  ,0.0
#endif
#ifdef ARCH_062D
	  ,0.0
#endif
	  );
  */

  sigset_t s;
  sigemptyset(&s);
  sigaddset(&s, SIGALRM);

  struct sigaction sa = {.sa_handler= &asyncHandler,
			 .sa_mask = s, 
			 .sa_flags = 0, 
			 .sa_restorer = 0};
  int status = sigaction(SIGALRM, &sa, 0);

  timer_t timerID;
  status = timer_create(CLOCK_MONOTONIC, 0, &timerID);
  struct itimerspec ts = 
    {{msPeriod / 1000, // seconds
      (msPeriod % 1000) * 1000000UL}, // nanoseconds
     {msPeriod / 1000, 
      (msPeriod % 1000) * 1000000UL}};
  
  gettimeofday(&now, NULL);
 


  rapl_tick(rs, filename);
 
  status = timer_settime(timerID, 0, &ts, 0);
  
  return;
}

void msSample(const char * const filename, int log){
  struct power_unit_s units;
  struct power_info_s info[NUM_DOMAINS];
  double joules[NUM_DOMAINS]; 
  uint64_t last_raw_joules[NUM_DOMAINS], last_raw_joules_tmp[NUM_DOMAINS];
  struct timeval now;
  uint64_t tsc;
  double time = 0;

  double tsc_rate;
  FILE *rateFile = fopen("/tmp/tsc_rate", "r");
  //! @todo measure/read tsc rate
  if(!rateFile && errno == ENOENT){
    tsc_rate = measure_tsc();
    rateFile = fopen("/tmp/tsc_rate", "w");
    fprintf(rateFile, "%lf\n", tsc_rate);
  }else if(rateFile){
    // get rate from file
    fscanf(rateFile, "%lf", &tsc_rate);
  } else {
    perror("error opening /tmp/tsc_rate");
    exit(1);
  }
  fclose(rateFile);

  fprintf(stderr, "tsc rate: %lf\n", tsc_rate);

  FILE *logFile = 0;
  if(log){
    logFile = fopen(filename, "w");
    assert(logFile);
  }
  
  if(log){
    fprintf(logFile, "timestamp\tpkg_J\tpp0_J\t"
#ifdef ARCH_062A
	    "pp1_J"
#endif
#ifdef ARCH_062D
	    "dram_J"
#endif
	    "\n");
    fprintf(logFile, "%lf\t%15.10lf\t%15.10lf"
	    "\t%15.10lf"
	    "\n", 
	    0.0, 0.0, 0.0
#ifdef ARCH_062A
	    ,0.0
#endif
#ifdef ARCH_062D
	    ,0.0
#endif
	    );
  }

  sigset_t s;
  sigemptyset(&s);
  sigaddset(&s, SIGALRM);

  struct sigaction sa = {.sa_handler= &handler, 
			 .sa_mask = s, 
			 .sa_flags = 0, 
			 .sa_restorer = 0};
  int status = sigaction(SIGALRM, &sa, 0);

  timer_t timerID;
  status = timer_create(CLOCK_MONOTONIC, 0, &timerID);
  struct itimerspec ts = {{0, 100000}, // .1ms
			  {0, 100000}};

  msr_debug=1;
  get_rapl_power_unit(0, &units);

  get_power_info(0, PKG_DOMAIN, &info[PKG_DOMAIN],&units);

  msr_debug = 0;

  get_energy_status(0, PKG_DOMAIN, &joules[PKG_DOMAIN], &units, 
		    &last_raw_joules[PKG_DOMAIN]);

  get_energy_status(0, PKG_DOMAIN, &joules[PKG_DOMAIN], &units, 
		    &last_raw_joules[PKG_DOMAIN]);
  // synchronize with an update
  while(!joules[PKG_DOMAIN]){    
    usleep(10);
    get_energy_status(0, PKG_DOMAIN, &joules[PKG_DOMAIN], &units, 
		      &last_raw_joules[PKG_DOMAIN]);
  }
  gettimeofday(&now, NULL);
  tsc = rdtsc();  
  
  status = timer_settime(timerID, 0, &ts, 0);

  get_energy_status(0, PKG_DOMAIN, &joules[PKG_DOMAIN], &units, 
		    &last_raw_joules[PKG_DOMAIN]);
  get_energy_status(0, PP0_DOMAIN, &joules[PP0_DOMAIN], &units,
		    &last_raw_joules[PP0_DOMAIN]);
#ifdef ARCH_062D
  get_power_info(0, DRAM_DOMAIN, 	&info[DRAM_DOMAIN],	&units);
#endif
#ifdef ARCH_062A
  get_energy_status(0, PP1_DOMAIN, &joules[PP1_DOMAIN], &units,
		    &last_raw_joules[PP1_DOMAIN]);
#endif



  
  double PKG_max_watts = 0, PP0_max_watts = 0;
  double PKG_total_joules = 0, PP0_total_joules = 0, delta;
  uint64_t lastPrint = 0, lastNonzero = tsc;
  int glitch = 0;

  while(1){
    sigwaitinfo(&s, 0); // timer will wake us up
    tsc = rdtsc();

    get_raw_energy_status(0, PKG_DOMAIN, &last_raw_joules_tmp[PKG_DOMAIN]);
    get_raw_energy_status(0, PP0_DOMAIN, &last_raw_joules_tmp[PP0_DOMAIN]);
#ifdef ARCH_062A
    get_raw_energy_status(0, PP1_DOMAIN, &last_raw_joules_tmp[PP1_DOMAIN]);
#endif
#ifdef ARCH_062D
    get_raw_energy_status(0, DRAM_DOMAIN, &last_raw_joules_tmp[DRAM_DOMAIN]);
#endif
    //! @todo freq
    //read_aperf_mperf(0, &aperf, &mperf);

    // wait for an update
    //! @todo this needs fixing
    if(last_raw_joules_tmp[PKG_DOMAIN] == last_raw_joules[PKG_DOMAIN]){
      continue;
    }

    double nzDelta = tsc_delta(&lastNonzero, &tsc, &tsc_rate);
    if(nzDelta < .001){ // wait at least 1ms
      /*! @todo flag these in the log.
	Updates seem to come in two time bases, ~1 KHz and ~100 Hz.
	I'm guessing they correspond to distinct segments of the chip.
	If I sample frequently enough, I can separate the updates by frequency.	
       */
      if(!glitch){
	/*
	fprintf(logFile, "#%lf\t%lf\tglitch \n", 
		time + nzDelta,
		nzDelta
		);
	*/
	glitch = 1;
      }
      last_raw_joules_tmp[PKG_DOMAIN] = last_raw_joules[PKG_DOMAIN];
      continue;
    }
    glitch = 0;

    lastNonzero = tsc;

    // convert raw joules
    joules[PKG_DOMAIN] = 
      convert_raw_joules_delta(&last_raw_joules[PKG_DOMAIN], 
			       &last_raw_joules_tmp[PKG_DOMAIN], 
			       &units);
    joules[PP0_DOMAIN] = 
      convert_raw_joules_delta(&last_raw_joules[PP0_DOMAIN], 
			       &last_raw_joules_tmp[PP0_DOMAIN], 
			       &units);
#ifdef ARCH_062A
    joules[PP1_DOMAIN] = 
      convert_raw_joules_delta(&last_raw_joules[PP1_DOMAIN], 
			       &last_raw_joules_tmp[PP1_DOMAIN], 
			       &units);
#endif
#ifdef ARCH_062D
    joules[DRAM_DOMAIN] = 
      convert_raw_joules_delta(&last_raw_joules[DRAM_DOMAIN], 
			       &last_raw_joules_tmp[DRAM_DOMAIN], 
			       &units);
#endif

    last_raw_joules[PKG_DOMAIN] = last_raw_joules_tmp[PKG_DOMAIN];
    last_raw_joules[PP0_DOMAIN] = last_raw_joules_tmp[PP0_DOMAIN];
#ifdef ARCH_062A
    last_raw_joules[PP1_DOMAIN] = last_raw_joules_tmp[PP1_DOMAIN];
#endif
#ifdef ARCH_062D
    last_raw_joules[DRAM_DOMAIN] = last_raw_joules_tmp[DRAM_DOMAIN];
#endif

    time += nzDelta;

    // don't log the suspect readings
    // && joules[PKG_DOMAIN] < info[PKG_DOMAIN].thermal_spec_power_watts
    if(log){
      fprintf(logFile, "%lf\t%15.10lf\t%15.10lf"
	      "\t%15.10lf"
	      "\n", 
	      time,
	      joules[PKG_DOMAIN],
	      joules[PP0_DOMAIN]
#ifdef ARCH_062A
	      ,joules[PP1_DOMAIN]
#endif
#ifdef ARCH_062D
	      ,joules[DRAM_DOMAIN]
#endif
	      );
    }
    PKG_max_watts = max(PKG_max_watts, joules[PKG_DOMAIN]/nzDelta);
    PP0_max_watts = max(PP0_max_watts, joules[PP0_DOMAIN]/nzDelta);
    PKG_total_joules += joules[PKG_DOMAIN];
    PP0_total_joules += joules[PP0_DOMAIN];
    delta = tsc_delta(&lastPrint, &tsc, &tsc_rate);
    if(delta > 1){
      fprintf(stderr, "max 1ms-power, average power in last second: "
	      "PKG: %10lf, %10lf, PP0: %10lf, %10lf\n", 
	      PKG_max_watts, PKG_total_joules / delta, 
	      PP0_max_watts, PP0_total_joules / delta);
      lastPrint = tsc;
      PKG_max_watts = 0;
      PP0_max_watts = 0;
      PKG_total_joules = 0;
      PP0_total_joules = 0;
    }
  } // while(1)
  
  //! @todo calculate average power
  
  return;
}
