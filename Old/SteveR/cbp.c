/* Copyright (C) 1988-1991, Barak Pearlmutter */
/* Continuous Barak Propagation.  KISS.  Funny how the features creep in... */

/* To do:

   analytic exactness by converging entire trajectory through metatime,
   maybe with variable grid methods

   auto weight equivalences for symmetry

   check each pattern within tolerance in trajectory case (?)

   interpolate environments

   vectorize environment loops

   rethink environment files soon.  Current scheme sucks.  Specify
   each pattern by itself.

   Add Rumelhart's complexity reduction measure, 

      complexity = K1*w_cost + K2*u_cost
      w_cost =  sum_{i,j}  w_{ij}^2 / (1 + w_{ij}^2)
      u_cost = w(i,j)^2 / (1 + sum_k( w(i,k)^2 ))

   */

#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/param.h>

#ifdef __hpux

# include <unistd.h>

#endif

/* CMU CS things: */
#include <c.h>
#include <ci.h>
#include <del.h>
#include <libc.h>

/* local include file. */
#include "cbp.h"

int Epoch = 0;

int matherr(exc)
struct exception *exc;
{
    char *s;
    switch(exc->type) {
    case DOMAIN:	s = "domain"; break;
    case SING:		s = "singularity"; break;
    case OVERFLOW:	s = "overflow"; break;
    case UNDERFLOW:	s = "underflow"; break;
    }
    fprintf(stdout, " %s(%f,%f): %s error\n", 
		exc->name, exc->arg1, exc->arg2, s);
    abort();
}




char *must_malloc(l)
     long l;
{
  char *p = malloc(l);
  if (p==NULL)
    {
      fprintf(stderr, "Failed to allocate %d bytes.\n", l);
      abort();
      exit(1);
    }
  return p;
}


#define MALLOC(typ,count) ((typ *)must_malloc(sizeof(typ)*(count)))

char *must_realloc(p,l)
     char *p;
     long l;
{
  p = realloc(p,l);
  if (p==NULL)
    {
      fprintf(stderr, "Failed to reallocate %d bytes.\n", l);
      exit(1);
    }
  return p;
}

#define REALLOC(ptr,typ,count) \
  (ptr = (typ *)must_realloc((char *)(ptr),sizeof(typ)*(count)))




bool read_by_pattern(fd, s, pi)
     FILE *fd;
     char *s;
     int *pi;
{
  if (fscanf(fd, s, pi) != 1)
    {
      fprintf(stderr, "Read error -- expected %s", s);
      return false;
    }
  else
    return true;
}



bool read_and_check(fd, s, n)
     FILE *fd;
     char *s;
     int n;
{
  int temp;

  if (!read_by_pattern(fd,s,&temp) || (temp != n))
    {
      fprintf(stderr, "Read error -- expected\n    ");
      fprintf(stderr, s, n);
      fprintf(stderr, "but found\n    ");
      fprintf(stderr, s, temp);
      return false;
    }
  else
    return true;
}



int read_int(fd)
     FILE *fd;
{
  int i;
  if (!read_by_pattern(fd,"%d",&i)) exit(1);
  return i;
}



void skip_whitespace(fd)
     FILE *fd;
{
  int c;
  do {
    c=getc(fd);
  } while (isspace(c));
  ungetc(c,fd);
}


bool char_p(fd, x)
     FILE *fd;
     char x;
{
  int c;
  skip_whitespace(fd);
  c = getc(fd);
  if (c == x)
    return true;
  else
    {
      ungetc(c,fd);
      return false;
    }
}






void fscanf_1(fd,str,p)
     FILE *fd;
     char *str;
     int *p;
{
  if (fscanf(fd,str,p)!=1)
    {
      fprintf(stderr, "Unable to parse input against template %s.\n", str);
      exit(1);
    }
}




void read_filename(s)
     char *s;
{
  fscanf(ciinput, "%s", s);
}





#ifdef USE_DOUBLES

#define CREAL		CDOUBLE
#define CICREAL		CICDOUBLE
#define INFINITY	Huge	/* was HUGE until that generated an error. */

#else

#define CREAL		CFLOAT
#define CICREAL		CICFLOAT
#define INFINITY	Huge

#endif




#ifdef convex
#ifndef USE_DOUBLES
#define EXP(x)	sexp(x)
#define LOG(x)	slog(x)
#endif
#endif

#ifndef EXP
#define EXP(x)	exp((double) x)
#define LOG(x)	log((double) x) 

#endif





#define SQRT2		1.4142135623730950488   /* sqrt(2) */
#define SQRT_PI_O2	0.8862269254527580136	/* + sqrt(pi)/2 */





/* #define USE_SIGMOID_TABLE */



#ifdef USE_SIGMOID_TABLE


#define sigmoid_table_resolution 1000
#define sigmoid_table_extent 10
#define sigmoid_table_size (sigmoid_table_resolution*sigmoid_table_extent)

#define SIGMOID(z,index) \
  (((index = ((int) (sigmoid_table_resolution * (z))))	\
   >= sigmoid_table_size) ? 1.0 :			\
  (index < -sigmoid_table_size ? 0.0 :			\
  sigmoid_table[index+sigmoid_table_size]))

REAL sigmoid_table [2*sigmoid_table_size];

REAL compute_sigmoid(x)
     double x;
{
  REAL   e;
  /* if (x > 30) return 1; */
  /* if (x < -30) return (0); */
  e = exp ((double) x);
  return (e / (e + 1));
}

void make_sigmoid_table()
{
  int x;
  for (x = -sigmoid_table_size; x < sigmoid_table_size; x++)
    sigmoid_table[x+sigmoid_table_size]
      = compute_sigmoid (x/(REAL)sigmoid_table_resolution);
}


#else

/* STEVER */
/* #define SIGMOID(x,index) (1/(1+(REAL)EXP(-(x))))  */
#define SIGMOID_LIMIT 50
#define SIGMOID(x,index) ((x) > SIGMOID_LIMIT ? 1.0 \
			 : ((x) < -SIGMOID_LIMIT ? 0.0 \
			 : (1/(1+(REAL)EXP(-(x))))))

#endif








/* Different sorts of transfer functions: */
#define STANDARD_SIGMOID	0
#define SYMMETRIC_SIGMOID	1
#define LINEAR			2
#define EXPONENTIAL		3

REAL trans(x,t)
     REAL x;
     int t;
{
#ifdef USE_SIGMOID_TABLE	/* avoid unused variable warning */
  int i;
#endif

  switch (t) {
  case STANDARD_SIGMOID:
    return SIGMOID(x,i);
  case SYMMETRIC_SIGMOID:
    /* return atanh(x); */
    return tanh(x);
  case LINEAR:
    return x;
  case EXPONENTIAL:
    return EXP(x);
  default:
    fprintf(stderr, "Internal error: transfer function number %d.\n", t);
    break;
  }
}

REAL d_trans(x,t)
     REAL x;
     int t;
{
  switch (t) {
  case STANDARD_SIGMOID:
    return x*(1-x);
  case SYMMETRIC_SIGMOID:
    return (1+x)*(1-x);
  case LINEAR:
    return 1;
  case EXPONENTIAL:
    return x;
  default:
    fprintf(stderr, "Internal error: dtransfer function number %d.\n", t);
    break;
  }
}













int input_count, output_count, io_count;

unit *io_array, *input_array, *output_array;

int unit_count, conn_count;
struct unit_str *unit_array;
struct wet_str *conn_array;




/* Environments */

int patt_count = 0;		/* Number of patterns in the environment */
int env_steps = 2;		/* Number of steps in the environment */
REAL *env_times;		/* When the timesteps are; first
				   always -Huge, last always Huge. */
pattern *envs_array;		/* what the various environments are.
				   Doesn't include the one at t=Huge. */


int wet_equiv_count;
struct wet_equiv_str *wet_equivs;

#define UNIT_NUMBER(u)	(u) /* (unit_array+(i)) */
#define WET_NUMBER(w)	(w) /* (conn_array+(i)) */
#define PAT_NUMBER(p)	(*envs_array+(p))

#define NUMBER_UNIT(u)	(u) /* ((u)-unit_array) */
#define NUMBER_WET(w)	(w) /* ((w)-conn_array) */
#define NUMBER_PAT(p)	((p)-*envs_array)


/* the number of time steps in a trajectory.  Used by NET_TRAJ_BACKWARDS. */
int total_ticks, allocated_ticks;




#define FORE_THINGS(start,size,v) \
  for ((v)=(start); (v)<&(start)[size]; (v)++)
#define BACK_THINGS(start,size,v) \
  for ((v)= &(start)[size-1]; (v)>=(start); (v)--)

#define FORE_INGS(start,size,v) for ((v)=0; (v)<(size); (v)++)
#define BACK_INGS(start,size,v) for ((v)=(size)-1; (v)>=0; (v)--)

#define FOREUNITS(u)	FORE_INGS(unit_array, unit_count, (u))
#define BACKUNITS(u)	BACK_INGS(unit_array, unit_count, (u))
#define FOREWETS(w)	FORE_INGS(conn_array, conn_count, (w))
#define BACKWETS(w)	BACK_INGS(conn_array, conn_count, (w))
#define FOREENV(e)	FORE_THINGS(*envs_array, patt_count, (e))
#define BACKENV(e)	BACK_THINGS(*envs_array, patt_count, (e))

#define FORINS(pu,rp,pat)				\
  for (pu=(pat)->input_array, rp=(pat)->input;		\
       pu < &(pat)->input_array[(pat)->input_count];	\
       pu++, rp++)

#define FOROUTS(pu,rp,pat)				\
  for (pu=(pat)->output_array, rp=(pat)->output;	\
       pu < &(pat)->output_array[(pat)->output_count];	\
       pu++, rp++)

#define FOREINCOMING(u,w,i) for (i=0; w=u_ins(u)[i],  i<u_in_count(u);  i++)
#define FOREOUTGOING(u,w,i) for (i=0; w=u_outs(u)[i], i<u_out_count(u); i++)
#define BACKINCOMING(u,w,i) for (i=u_in_count(u)-1;  w=u_ins(u)[i],  i>=0; i--)
#define BACKOUTGOING(u,w,i) for (i=u_out_count(u)-1; w=u_outs(u)[i], i>=0; i--)

#define DOUNITS(u)		BACKUNITS(u)
#define DOWETS(w)		BACKWETS(w)
#define DOINCOMING(u,w,i)	BACKINCOMING(u,w,i)
#define DOOUTGOING(u,w,i)	BACKOUTGOING(u,w,i)
#define DOENV(p)		FOREENV(p)

/* \zeta_{ij} */

#define zeta(u_i,u_j)	(u_zetarray((u_i))[NUMBER_UNIT(u_j)])




#include <RANDOM.h>





CINT(s_epoch, epoch, 0, MAX_COOL_INT, "current epoch number");
CINT(s_batchsize, batchsize, -MAX_COOL_INT, MAX_COOL_INT, "patterns per batch, 0=all, negative=random");
CINT(s_online_pat_num, online_pat_num, 0, MAX_COOL_INT, "next online pattern");
CSTRING(s_run_name, run_name, 100, "designation for this run");
CINT(s_check_interval, check_interval, 0, MAX_COOL_INT, "checkpoint interval");
CSTRING(s_check_file, check_file, 100, "checkpoint filename");
CINT(s_scheck_interval, scheck_interval, 0, MAX_COOL_INT, "state checkpoint interval");
CSTRING(s_scheck_file, scheck_file, 100, "state checkpoint filename");
CINT(s_tcheck_interval, tcheck_interval, 0, MAX_COOL_INT, "trajectory checkpoint interval");
CSTRING(s_tcheck_file, tcheck_file, 100, "trajectory checkpoint filename");
CREAL(s_epsilon, epsilon, -INFINITY, INFINITY, "epsilon");
CREAL(s_T_epsilon, T_epsilon, -INFINITY, INFINITY, "epsilon for time constants");
CREAL(s_momentum, momentum, -2, 2, "momentum");
CREAL(s_bmomentum, bmomentum, -1, 1, "bmomentum (second order momentum)");
CREAL(s_delta_t, delta_t, 0, INFINITY, "delta t");
CREAL(s_for_criterion, for_criterion, 0, 0.5, "stability criterion for states");
CREAL(s_back_criterion, back_criterion, 0, 0.5, "stability criterion for derivatives");
CBOOL(s_stop_slosh, stop_slosh, "suspend simulation on sloshing");
CBOOL(s_autopilot, autopilot, "adjust epsilon automatically; disables stop_slosh");
CREAL(s_autoraise, autoraise, 1, Huge, "autopilot epsilon multiplier when cos>autolimit");
CREAL(s_autolower, autolower, 0, 1, "autopilot epsilon multiplier when cos<autolimit");
CREAL(s_automuchlower, automuchlower, 0, 1, "autopilot epsilon multiplier when cos<0");
CREAL(s_autolimit, autolimit, 0, 1, "autopilot cos target; raises/lowers epsilon when cos above/below");
CBOOL(s_stop_zero_errors, stop_zero_errors, "suspend simulation when no wrong cases");
CBOOL(s_stop_high_B, stop_high_B, "suspend simulation on high B");
CBOOL(s_display_time, display_time, "display convergence times");
CBOOL(s_display_B, display_B, "display B");
CINT(s_B_subepoch_count, B_subepoch_count, -1, MAX_COOL_INT, "B subepoch count");
CREAL(s_max_B, max_B, 0, INFINITY, "value of B that stops simulation");
CREAL(s_dB_hack, dB_hack, 0, INFINITY, "hack for B minimization routine");
CREAL(s_perturb_interval, perturb_interval, 0, INFINITY, "Interval between perturbations");
CREAL(s_perturb_mag, perturb_mag, 0, INFINITY, "Magnitude of perturbations");
CINT(s_low_printing_unit, low_printing_unit, 0, MAX_COOL_INT, "index of lowest unit dumped");
CINT(s_high_printing_unit, high_printing_unit, 0, MAX_COOL_INT, "index of highest unit dumped");


#define NET_STANDARD		0
#define NET_PINEDA		1
#define NET_MEAN_FIELD		2
#define NET_TRAJ_GUESS_Z	3
#define NET_TRAJ_BACKWARDS	4

#define NET_TRAJ(net_type)	\
  ((net_type)==NET_TRAJ_GUESS_Z || (net_type)==NET_TRAJ_BACKWARDS)

char *net_type_table[] =
{
  "standard backpropagation",
  "fixpoint learning (recurrent network)",
  "mean field theory (recurrent network)",
  "trajectory learning (recurrent network, guess z(t0))",
  "trajectory learning (recurrent network, run z backwards)",
  NULL,
};

CSEARCH(s_net_type, net_type, net_type_table, "network type");


CBOOL(s_display_cases, display_cases, "display incorrect case count");
CBOOL(s_display_Ev, display_Ev, "display expected velocity");
CBOOL(s_display_len_w, display_len_w, "display weight vector length");
CREAL(s_correct_margin, correct_margin, 0, 0.5, "deviance that counts as correct");
char *etype_table[] =
{
  "simple square",
  "marginated square",
  "cross entrophy",
  "normalized distribution",
  "gaussian: -log(p(gaussian)), mean in declared output and stdev in next",
  NULL,
};
CSEARCH(s_error_type, error_type, etype_table, "error function");
CREAL(s_square_margin, square_margin, 0, 0.5, "square error margins");
CREAL(s_weight_decay, weight_decay, 0, 1, "weight decay");
CREAL(s_T_decay, T_decay, 0, 1, "time constant decay towards 1");
CREAL(s_end_of_time, end_of_time, 0, INFINITY, "end of time");
CBOOL(s_normalize_outputs, norm_outputs, "normalize output activations");
CINT(s_seed, weight_seed, 0, MAX_COOL_INT, "seed for wt randomize (hack)");
CREAL(s_excursion, weight_excursion, 0, INFINITY, "excursion for wt randomize (hack)");



CIENTRY ci_list[] = {

  CICMD("read_weights", read_wet),
  CICMD("read_environment", read_env),
  CICMD("read_z0s", read_z0s),
  CICMD("write_z0s", write_z0s),
  CICMD("write_weights", write_wet),
  CICMD("write_states", write_states),
  CICMD("write_trajectory", write_trajectory),

  CICMD("checkpoint_weights", checkpoint_weights),
  CICMD("checkpoint_states", checkpoint_states),
  CICMD("checkpoint_trajectory", checkpoint_trajectory),

  CICMD("go",go),

  CICMD("randomize_weights", randomize_weights),
  CICMD("weight_stats", weight_stats),

  CICMD("one_Ts", one_Ts),
  CICMD("clear_momentum", clear_momentum),
  CICMD("multiply_epsilon", multiply_epsilon),
  CICMD("divide_epsilon", divide_epsilon),

  CICMD("cd", cd_cmd),
  CICMD("pwd", pwd_cmd),

  CICSTRING("run_name", s_run_name),
  CICINT("epoch", s_epoch),
  CICINT("batchsize", s_batchsize),

  CICREAL("epsilon", s_epsilon),
  CICREAL("T_epsilon", s_T_epsilon),
  CICREAL("momentum", s_momentum),
  CICREAL("bmomentum", s_bmomentum),
  CICREAL("weight_decay", s_weight_decay),
  CICREAL("T_decay", s_T_decay),
  CICREAL("end_of_time", s_end_of_time),
  CICREAL("delta_t", s_delta_t),
  CICREAL("forward_criterion", s_for_criterion),
  CICREAL("backward_criterion", s_back_criterion),
  CICSEARCH("error_type", s_error_type),
  CICREAL("square_margin", s_square_margin),
  CICBOOL("stop_slosh", s_stop_slosh),

  CICBOOL("autopilot", s_autopilot),
  CICREAL("autoraise", s_autoraise),
  CICREAL("autolower", s_autolower),
  CICREAL("automuchlower", s_automuchlower),
  CICREAL("autolimit", s_autolimit),

  CICBOOL("stop_high_B", s_stop_high_B),
  CICBOOL("display_time", s_display_time),
  CICBOOL("display_B", s_display_B),
  CICINT("B_subepoch_count", s_B_subepoch_count),
  CICREAL("max_B", s_max_B),
  CICREAL("dB_hack", s_dB_hack),
  CICSEARCH("net_type", s_net_type),
  CICBOOL("display_cases", s_display_cases),
  CICREAL("correct_margin", s_correct_margin),
  CICBOOL("stop_zero_errors", s_stop_zero_errors),
  CICBOOL("display_expected_velocity", s_display_Ev),
  CICBOOL("display_weight_length", s_display_len_w),
  CICINT("weight_check_interval", s_check_interval),
  CICSTRING("weight_check_file", s_check_file),
  CICINT("state_check_interval", s_scheck_interval),
  CICSTRING("state_check_file", s_scheck_file),
  CICINT("traj_check_interval", s_tcheck_interval),
  CICSTRING("traj_check_file", s_tcheck_file),
  CICINT("low_printing_unit", s_low_printing_unit),
  CICINT("high_printing_unit", s_high_printing_unit),
  CICREAL("perturb_interval", s_perturb_interval),
  CICREAL("perturb_magnitude", s_perturb_mag),
  CICBOOL("normalize_outputs", s_normalize_outputs),
  CICINT("weight_seed", s_seed),
  CICREAL("weight_excursion", s_excursion),
  CIEND,
};



void cd_cmd(arglist)
     char *arglist;
{
  if (chdir(arglist))
    fprintf(stderr, "Can't switch to %s.\n", arglist);
}

#ifdef __hpux
	/* system 5 */
				/* SUPPRESS 590 *//* arglist unused */
void pwd_cmd(arglist)
     char *arglist;
{
  char buff[MAXPATHLEN+1];
  printf("%s\n", getcwd(buff, MAXPATHLEN+1));
}

#else
				/* SUPPRESS 590 *//* arglist unused */
void pwd_cmd(arglist)
     char *arglist;
{
  char buff[MAXPATHLEN];
  printf("%s\n", getwd(buff));
}

#endif




FILE *prompt_fopen_with_ext(file, ext, mode, prompt)
     char *file, *ext, *mode, *prompt;
{
  char temp_buff[MAXPATHLEN];
  FILE *fd;

  strncpy(temp_buff, file, MAXPATHLEN);
  if (temp_buff[0] == '\0')
    {
      if (*ext == '\0')
	printf("%s: ", prompt);
      else
	printf("%s (%s): ", prompt, ext);
      read_filename(temp_buff);
    }
  strncat(temp_buff, ext, MAXPATHLEN);
  fd = fopen(temp_buff, mode);
  if (fd == NULL)
    fprintf(stderr, "Unable to open %s.\n", temp_buff);
  return fd;
}




/* cbp environment files are just like bpla environment files, except
   that some patterns can be "strange", indicated by an initial ':'.  In
   such patterns, a '?' before a value marks it as output, and otherwise
   it's input. */

bool read_env_line(fd, p)
     FILE *fd;
     pattern p;
{
  int j;
  double r;
  REAL *rp;

  if (char_p(fd, ':'))
    {
      /* "strange" case. */
      unit *piu, *pou, *pu;
      REAL *pir, *por;

      /* Running the inputs in one direction and the outputs in the other
	 simplifies the parsing. */

      p->input_array = MALLOC(unit,io_count);
      p->input_count = p->output_count = 0;
      for (pir=p->input,       por=pir + io_count,
	   piu=p->input_array, pou=piu + io_count, pu=io_array,
	   j=0; j<io_count; j++)
	{
	  bool outy = char_p(fd, '?');

	  if (!outy && char_p(fd, '*'))
	    r = DONTCARE;
	  else if (fscanf(fd, "%F", &r) != 1)
	    {
	      fprintf(stderr, "Error reading value %d", j);
	      return false;
	    }

	  if (outy)
	    {
	      p->output_count += 1;
	      *--por = r;
	      *--pou = *pu++;
	    }
	  else
	    {
	      p->input_count += 1;
	      *pir++ = r;
	      *piu++ = *pu++;
	    }
	}
      p->output = por;
      p->output_array = pou;
    }
  else
    {
      for (j=0, rp=p->input; j<io_count; j++, rp++)
	{
	  if (char_p(fd, '*'))
	    r = DONTCARE;
	  else if (fscanf(fd, "%F", &r) != 1)
	    {
	      fprintf(stderr, "Error reading value %d", j);
	      return false;
	    }
	  *rp = r;
	}
      p->input_count = input_count;
      p->output_count = output_count;
    }
  return true;
}




pattern malloc_env_array(n)
     int n;
{
  pattern new_env_array = MALLOC(struct patt_str, n), p;
  int i;
  REAL *rp;

  for (i=0, p=new_env_array; i<n; i++,p++)
    {
      rp = MALLOC(REAL,io_count);
      p->input = rp;
      p->output = rp + input_count;

      p->input_count = input_count;
      p->output_count = output_count;
      p->input_array = input_array;
      p->output_array = output_array;
    }
  return new_env_array;
}




void write_real(fd, s, r)
     FILE *fd;
     char *s;
     REAL r;
{
#ifdef USE_DOUBLES
  fprintf(fd, "%s%G ", s, r);
#else
# ifdef __hpux
    fprintf(fd, "%s%g ", s, ((double) r));
# else
    fprintf(fd, "%s%g ", s, r);
# endif
#endif
}









pattern read_environment(fd, patts)
     FILE *fd;
     int patts;
{
  int i;
  pattern new_env = malloc_env_array(patts), p;

  for (i=0, p=new_env; i<patts; i++,p++)
    if (!read_env_line(fd,p))
      {
	fprintf(stderr, " while reading pattern %d.\n", i);
	/* Free all the allocated stuff here. */
	return NULL;
      }
  return new_env;
}





void read_env(file)
     char *file;
{
  FILE *fd = prompt_fopen_with_ext(file, ".env", "r", "Environment file to read");
  int new_patt_count, i, j;
  pattern p, temp_env;
  REAL *rp;
  double r;

  if ((fd == NULL) ||
      !read_and_check(fd, "%d inputs\n", input_count) || 
      !read_and_check(fd, "%d outputs\n", output_count) ||
      !read_by_pattern(fd, "%d patterns\n", &new_patt_count))
    return;

  /* if (patt_count)
    {
      for (i=0, p= *envs_array; i<patt_count; i++,p++)
	free(p->input);
      free(*envs_array);
    } */

  patt_count = new_patt_count;
  temp_env = read_environment(fd, patt_count);
  if (temp_env == NULL)
    {
      env_steps = 2;
      patt_count = 0;
      env_times[0] = -Huge;
      env_times[1] = Huge;
      return;
    }

  if (char_p(fd, ':') && read_by_pattern(fd, "%d extrasteps\n", &i))
    env_steps = i+2;
  else if (net_type == NET_STANDARD) 
    {
      /* don't allocate extra environments */
      envs_array[0] = temp_env;
      return;
    }
  else
    env_steps = 2;

  REALLOC(env_times, REAL, env_steps);
  REALLOC(envs_array, pattern, env_steps);

  env_times[0] = -Huge;
  env_times[env_steps-1] = Huge;
  envs_array[0] = temp_env;

  for (i=1; i<env_steps-1; i++)		/* SUPPRESS 68 *//* int *arg[3] */
    if (!read_by_pattern(fd, " t=%F\n", &r) ||
	(envs_array[i] = read_environment(fd, patt_count)) == NULL)
      return;
    else
      env_times[i] = r;

  if (net_type == NET_TRAJ_GUESS_Z) 
    {
      /* Space for the zetas; only needed by NET_TRAJ_GUESS_Z. */
      for (i=0, p= *envs_array; i<patt_count; i++,p++)
        {
          p->z0_array = MALLOC(REAL, unit_count);
          for (j=0, rp=p->z0_array; j<unit_count; j++, rp++)
	    *rp = 0;
        }
    }

  fclose(fd);

  printf("Patterns: %d.  Timesteps: %d.\n", patt_count, env_steps-1);
}




void read_wet(file)
     char *file;
{
  FILE *fd = prompt_fopen_with_ext(file, "", "r", "Weight file to read");
  int new_epoch, temp;
  wet w;
  unit u;
  double r;

  if (fd == NULL) return;

  if (fscanf(fd, "%d epochs\n", &new_epoch) != 1)
    {
      fprintf(stderr, "Error reading epoch number.\n");
      fclose(fd);
      return;
    }
  
  if (fscanf(fd, "%d weights\n", &temp) != 1)
    {
      fprintf(stderr, "Error reading weight count.\n");
      fclose(fd);
      return;
    }

  if (temp != conn_count)
    {
      fprintf(stderr, "File provides %d weights; architecture has %d.\n",
	      temp, conn_count);
      fclose(fd);
      return;
    }

  epoch = new_epoch;

  FOREWETS(w)
    {
      if (fscanf(fd, "%F", &r) != 1)
	{
	  fprintf(stderr, "Error; only %d weights were read.\n", NUMBER_WET(w));
	  fclose(fd);
	  return;
	}
      w_w(w) = r;
    }

  /* Read time constants (optional) */
  if (fscanf(fd, "%d timeconstants\n", &temp) == 1 && temp == unit_count)
    FOREUNITS(u)
      {
	if (fscanf(fd, "%F", &r) != 1)
	  {
	    fprintf(stderr, "Error: only %d time constants read.\n", NUMBER_UNIT(u));
	    fclose(fd);
	    return;
	  }
	else
	  u_T(u) = r;
      }
  else if (net_type)
    {
      printf("Warning: no time constants in weight file; all set to 1.\n");
      one_Ts();
    }

  fclose(fd);
}



void read_z0s(file)
     char *file;
{
  FILE *fd = prompt_fopen_with_ext(file, "", "r", "File to read z0s from");
  int j;
  pattern e;
  REAL *rp;

  if (fd == NULL) return;

  if (!read_and_check(fd, "%d units\n", unit_count) ||
      !read_and_check(fd, "%d patterns\n", patt_count))
    return;

  FOREENV(e)
    {
      for (j=0, rp=e->z0_array; j<unit_count; j++, rp++)/* SUPPRESS 68 */
	if (!read_by_pattern(fd, SCAN_REAL, rp))
	  {
	    fprintf(stderr, "While reading pattern %d number %d.\n",
		    NUMBER_PAT(e), j);
	    return;
	  }
    }

  fclose(fd);
}



void write_wet(file)
     char *file;
{
  FILE *fd = prompt_fopen_with_ext(file, "", "w", "Weight file to write");
  wet w;
  unit u;

  if (fd == NULL) return;

  fprintf(fd, "%d epochs\n%d weights\n", epoch, conn_count);
  FOREWETS(w) fprintf(fd, "%g\n", (double) w_w(w));

  if (NET_TRAJ(net_type))
    {
      fprintf(fd, "%d timeconstants\n", unit_count);
      FOREUNITS(u) fprintf(fd, "%g\n", (double) u_T(u));
    }

  fclose(fd);
}



void write_z0s(file)
     char *file;
{
  FILE *fd = prompt_fopen_with_ext(file, "", "w", "File to write z0s to");
  int i;
  pattern e;
  REAL *rp;

  if (fd == NULL) return;

  fprintf(fd, "%d units\n%d patterns\n", unit_count, patt_count);

  FOREENV(e)
    {
      for (i=0, rp=e->z0_array; i<unit_count; i++, rp++)
	write_real(fd, "", *rp);
      fprintf(fd, "\n");
    }

  fclose(fd);
}




void clear_momentum()
{
  wet w;
  unit u;

  DOWETS(w) w_velocity(w) = 0;
  DOUNITS(u) u_velocity(u) = 0;
  DOWETS(w) w_acceleration(w) = 0;
  DOUNITS(u) u_acceleration(u) = 0;
}



void one_Ts()
{
  unit u;

  DOUNITS(u)
    {
      u_T(u) = 1;
      u_velocity(u) = 0;
      u_acceleration(u) = 0;
    }
}





void equiv_weights(d_flag)
     bool d_flag;
{
  struct wet_equiv_str *p;
  int i,j;

  for (i=0, p=wet_equivs; i<wet_equiv_count; i++, p++)
    {
      int n = p->count;
      wet *pw;
      REAL r = 0, nw;

      for (j=0, pw = p->guys; j<n; j++, pw++)
	r += (d_flag ? w_dE_dw(*pw) : w_w(*pw));

      nw = r/n;

      for (j=0, pw = p->guys; j<n; j++, pw++)
	*(d_flag ? &w_dE_dw(*pw) : &w_w(*pw)) = nw;
    }
}


void print_epsilon()
{
  write_real(stdout, "epsilon:", epsilon);
  if (NET_TRAJ(net_type))
    write_real(stdout, "   T_epsilon:", T_epsilon);
  printf("\n");
}




void update_weights()
{
  wet w;
  unit u;
  REAL dot = 0, w2 = 0, v2 = 0;
  double cosine;

  if (weight_decay) DOWETS(w) w_dE_dw(w) += weight_decay * w_w(w);
  /* Deal with time constants for networks that so require. */
  if (NET_TRAJ(net_type))
    if (T_decay) DOUNITS(u) u_dE_dT(u) += T_decay * (u_T(u) - 1);

  if (wet_equiv_count) equiv_weights(TRUE);

  /* Compute the cosine before updating the weights so the simulation
   * can be stopped upon sloshing before the weights have been updated. */

  DOWETS(w)
    {
      dot -= w_dE_dw(w) * w_velocity(w);
      w2 += w_dE_dw(w) * w_dE_dw(w);
      v2 += w_velocity(w) * w_velocity(w);
    }

  /* Deal with time constants for networks that so require. */
  if (NET_TRAJ(net_type))
    DOUNITS(u)
      {
	w2 += u_dE_dT(u) * u_dE_dT(u);
	dot -= u_dE_dT(u) * u_velocity(u);
	v2 += u_velocity(u) * u_velocity(u);
      }

  /* Some machines can't divide zero by zero. */
#if IEEE || ieee || sun || ibmrt
  cosine = dot/sqrt((double) w2*v2);
#else
  cosine = w2>0 && v2>0 ? dot/sqrt((double) w2*v2) : 1;
#endif

  write_real(stdout, " cos:", cosine);
  write_real(stdout, "|grad|:", sqrt((double) w2));
  write_real(stdout, "|v|:", sqrt((double) v2));

  if (display_Ev) {
    /* Compute expected speed if velocity is at equilibrium and there is no
       sloshing.  This would be E(v) = |dE/dw|/(1-momentum). */

    /* This should be fixed sometime for higher order momentum. */

    write_real(stdout, "v/E(v):", sqrt((double) v2)*(1-momentum)/sqrt((double)
    w2));
  }
  
  if (display_len_w) {
    REAL lenw2 = 0;
    DOWETS(w) lenw2 += w_w(w)*w_w(w);
    if (NET_TRAJ(net_type))
      DOUNITS(u) lenw2 += u_T(u)*u_T(u);
    
    write_real(stdout, "|w|:", sqrt((double) lenw2));
  }

  printf("\n");

  if (autopilot && !isnan(cosine)) {
    if (cosine < 0) {
      epsilon *= automuchlower;
      T_epsilon *= automuchlower;
      clear_momentum();
      printf("Clearing momentum, ");
      print_epsilon();
    }
    else if (cosine < autolimit) {
      epsilon *= autolower;
      T_epsilon *= autolower;
      print_epsilon();
    }
    else {
      epsilon *= autoraise;
      T_epsilon *= autoraise;
      print_epsilon();
    }
  }
  else if (stop_slosh && dot < 0) {
    /* return before updating weights or incrementing epoch number. */
    printf("Sloshing...\n");
    _del_ += 1;
    return;
  }

  epoch += 1;

  /*
    a(t) = \alpha_1 a(t-1) - dE/dw(t)
    v(t) = a(t) + \alpha_0 v(t-1)
    dw	 = \eta v(t)
    */

  DOWETS(w) w_acceleration(w) = - w_dE_dw(w) + bmomentum * w_acceleration(w);
  DOWETS(w) w_velocity(w) = w_acceleration(w) + momentum * w_velocity(w);
  DOWETS(w) w_w(w) += epsilon * w_velocity(w);

  if (NET_TRAJ(net_type))
    {
      DOUNITS(u) u_acceleration(u) = - u_dE_dT(u) + bmomentum * u_acceleration(u);
      DOUNITS(u) u_velocity(u) = u_acceleration(u) + momentum * u_velocity(u);
      DOUNITS(u) u_T(u) += T_epsilon * u_velocity(u);
    }
}





/* Return the sum squared weights times the weight decay over two. */
REAL decay_error()
{
  REAL E1=0, E2=0;
  wet w;
  unit u;

  if (weight_decay)
    DOWETS(w) { REAL x = w_w(w); E1 += x*x; }
  if (NET_TRAJ(net_type) && T_decay)
    DOUNITS(u) { REAL x = u_T(u)-1; E2 += x*x; }

  return weight_decay*E1/2 + T_decay*E2/2;
}


				/* SUPPRESS 590 *//* arglist unused */
void weight_stats(arglist)
     char *arglist;
{
  wet w;
  REAL total = 0, total2 = 0, totala = 0, mmin = Huge, mmax = -Huge, mean;

  /* BUG: shouldn't this do the time constants too? */

  DOWETS(w) {
    REAL x = w_w(w);
    total += x;
    total2 += x*x;
    totala += (x<0 ? -x : x);
    if (x<mmin) mmin = x;
    if (x>mmax) mmax = x;
  }

  mean = total/conn_count;

  printf("Number of weights: %d\n", conn_count);
  printf("\tmean: %g, stdev: %g\n", (double) mean, 
	(double) sqrt((double) (total2/conn_count - mean*mean)));
  printf("\tmin: %g, max: %g, mean abs %g.\n", (double) mmin, 
	(double) mmax, (double) (totala/conn_count));
}

void randomize_weights(arglist)
     char *arglist;
{
  REAL excursion;
  wet w;
  long seed = 0;
  if (*arglist != '\0') {
      excursion = doublearg(&arglist,0,"excursion:", 0.0, INFINITY, 1.0);
  } else if (weight_excursion > 0.0) {
      excursion = weight_excursion;
  } else {
      excursion = 1.0;
  }
  while (isspace(*arglist))
    arglist++;
  if (*arglist != '\0')
    seed = longarg(&arglist,0,"seed:", MININT, MAXINT, seed);

  printf("Randomizing Weights, excursion %f", (double) excursion); 

  if (seed)
    {
      printf(", seed %u.\n", seed);
      SRANDOM(seed);
    }
  else if (weight_seed >= 0) 
    {
      printf(", seed %u.\n", weight_seed);
      SRANDOM((long)weight_seed);
    }
  else
    printf(".\n");

  epoch = 0;

  DOWETS(w) w_w(w) = excursion*RANDOM();
  DOWETS(w) w_velocity(w) = 0;
  DOWETS(w) w_acceleration(w) = 0;
  one_Ts();
  equiv_weights(FALSE);
  online_pat_num = 0;
}



void multiply_epsilon(arglist)
     char *arglist;
{
  REAL factor;

  factor = doublearg(&arglist,0,"factor:", 0.9, INFINITY, 1.0);
  epsilon *= factor;
  T_epsilon *= factor;
  print_epsilon();
}


void divide_epsilon(arglist)
     char *arglist;
{
  REAL divisor;

  divisor = doublearg(&arglist,0,"divisor:", 0.9, INFINITY, 1.0);
  epsilon /= divisor;
  T_epsilon /= divisor;
  print_epsilon();
}




void compute_dz_dt()
{
  unit u;
  wet w;
  int i;

  /* First the first two terms... */
  DOUNITS(u)
    {
      u_dz_dt(u) = u_z(u)/u_T(u) - u_external_dE_dy(u);
      /* a temporary used in the big sum below. */
      u_temp(u) = - d_trans(u_sig_x(u),u_transfer(u)) * u_z(u) / u_T(u);
    }

  /* And then the big sum. */

  /* this seems cool: */

  DOUNITS(u)
    {
      REAL sum = u_dz_dt(u);
      DOOUTGOING(u, w, i)
	sum += w_w(w) * u_temp(w_to(w));
      u_dz_dt(u) = sum;
    }

  /* This seems uncool:

  DOWETS(w)
    u_dz_dt(w_from(w)) += w_w(w) * u_temp(w_to(w));
  */
}




void trajectory_accumulate_dw()
{
  unit u;
  wet w;

  /* Accumulate the dE/dw's.
     constant factor removed:  - delta_t/ u_T(w_to(w)) */
  DOWETS(w) w_dE_dw(w) += u_y(w_from(w)) * u_z(w_to(w))
			     * d_trans(u_sig_x(w_to(w)),u_transfer(w_to(w)));

  /* Accumulate the dE_dT's.
     constant factor removed:  - delta_t/u_T(u) */   
  DOUNITS(u) u_dE_dT(u) += u_z(u) * u_dy_dt(u);
}








void correct_dw_constant_factors()
{
  wet w;
  unit u;

  if (NET_TRAJ(net_type))
    {
      DOWETS(w) w_dE_dw(w) *= delta_t / u_T(w_to(w));
      DOUNITS(u) u_dE_dT(u) *= - delta_t / u_T(u);
    }
}




void compute_x()
{
  unit u;
  wet w;

  DOUNITS(u) u_x(u) = 0;
  DOWETS(w) u_x(w_to(w)) += w_w(w) * u_y(w_from(w));
}



void compute_dy_dt()
{
  unit u;

  DOUNITS(u)
    {
      u_sig_x(u) = trans(u_x(u),u_transfer(u));
      u_dy_dt(u) = (- u_y(u) + u_sig_x(u) + u_I(u)) / u_T(u);
    }
}






void update_zetas()
{
  unit u, u2;
  wet w;
  int i;

  /* Update the zeta's. */
  DOUNITS(u)
    DOUNITS(u2)
      {
	REAL dzeta_dt = zeta(u,u2)/u_T(u);
	DOOUTGOING(u,w,i)
	  {
	    dzeta_dt -=
	      w_w(w) * zeta(w_to(w),u2) *
		d_trans(u_sig_x(w_to(w)),u_transfer(w_to(w)))
		  / u_T(w_to(w));
	  }

	zeta(u,u2) += delta_t * dzeta_dt;
      }
}


REAL forward_timestep(tick_or_save_dw)
     /* in NET_TRAJ_GUESS_Z case this says whether to bother with dE/dx's.
	in NET_TRAJ_BACKWARDS case it is the time tick number. */
     int tick_or_save_dw;
{
  unit u;
  REAL total_rate_o_change = 0;

  /* Update the states: */

  compute_x();
  compute_dy_dt();

  if (!NET_TRAJ(net_type))
    DOUNITS(u) total_rate_o_change += ABS(u_dy_dt(u));

  DOUNITS(u) u_y(u) += delta_t * u_dy_dt(u);


  /* Maybe do other stuff: */

  switch (net_type)
    {
    case NET_TRAJ_GUESS_Z:
      compute_dz_dt();
      DOUNITS(u) u_z(u) += delta_t * u_dz_dt(u);
      update_zetas();
      if (tick_or_save_dw) trajectory_accumulate_dw();
      break;

    case NET_TRAJ_BACKWARDS:
      DOUNITS(u)
	{
	  u_y_movie(u)[tick_or_save_dw] = u_y(u);
	  u_sig_x_movie(u)[tick_or_save_dw] = u_sig_x(u);
	  u_dy_dt_movie(u)[tick_or_save_dw] = u_dy_dt(u);
	}
      break;

    default:
      return total_rate_o_change / unit_count;
    }
}




/* This does a standard backpropagation forward pass.  Pineda
   formalism instead of clamping of inputs. */

void forward_propagate()
{
  register unit u;
  register wet w;

  DOUNITS(u)
    {
      u_y_valid(u) = false;
      u_x(u) = 0;
    }

  FOREWETS(w)
    {
      u = w_from(w);
      if (!u_y_valid(u))
	{
	  u_y_valid(u) = true;
	  u_sig_x(u) = trans(u_x(u),u_transfer(u));
	  u_y(u) = u_sig_x(u) + u_I(u);
	}
      
      u_x(w_to(w)) += w_w(w) * u_y(u);
    }

  DOUNITS(u)			/* Maybe just run through output units here? */
    if (!u_y_valid(u))
      {
	u_y_valid(u) = true;
	u_sig_x(u) = trans(u_x(u),u_transfer(u));
	u_y(u) = u_sig_x(u) + u_I(u);
      }
}



/* This does a standard backpropagation backward pass.  Pineda
   formalism instead of clamping of error. */

void backward_propagate()
{
  register unit u;
  register wet w;

  /* Here, y_valid flags whether dE_dx has been computed yet. */

  DOUNITS(u) u_y_valid(u) = false;

  BACKWETS(w)
    {
      u = w_to(w);
      if (!u_y_valid(u))
	{
	  u_y_valid(u) = true;
	  u_dE_dx(u) = d_trans(u_sig_x(u),u_transfer(u)) * u_z(u);
	}
      
      /* Note: this is useless if w_from(w) has no incoming weights. */
      u_z(w_from(w)) += w_w(w) * u_dE_dx(u);
    }

  /*
    DOUNITS(u)
    if (!u_y_valid(u)) { copy of similar if from above }
    */

  DOWETS(w) w_dE_dw(w) += u_y(w_from(w)) * u_dE_dx(w_to(w));
}





#define clear_states()		\
{				\
  unit u;			\
				\
  DOUNITS(u)			\
    {				\
      u_I(u) = 0;	\
      u_x(u) = 0;	\
    }					\
}

#define clear_ys()			\
{					\
  unit u;				\
  DOUNITS(u) u_y(u) = 0.5;	\
}

#define I_ys()					\
{						\
  unit u;					\
  DOUNITS(u) u_y(u) = u_I(u) + 0.5;	\
}






void setup_inputs (pat)
     pattern pat;
{
  REAL *rp;
  unit *pu;

  FORINS(pu,rp,pat)
    if (*rp != DONTCARE)
      u_I(*pu) = *rp - 0.5;
}




void dump_current_state(fd,t,show_t)
     FILE *fd;
     REAL t;
     bool show_t;
{
  unit u;

  if (show_t) fprintf(fd, "%f ", (double) t);
  /* FOREUNITS(u) */
  for (u=low_printing_unit; u<=high_printing_unit; u++)
    fprintf(fd, "%f ", (double) u_y(u));
  fprintf(fd, "\n");
}






/* This is used for recurrent nets where we only care about the fixpoint. */

void fixpoint_forward_sweep (fd)
     FILE *fd;
{
  unit u;
  REAL delta, t = 0;

  DOUNITS(u) u_y(u) = 0.5;

  if (fd) dump_current_state(fd,t,true);

  do {
    delta = forward_timestep(false);
    t += delta_t;
    if (_del_>1) break;
    if (fd) dump_current_state(fd,t,true);
  } while (delta > for_criterion);

  if (display_time) printf("%f ", (double) t);
}






/* Returns the error for the pattern. */
REAL compute_external_dE_dy(pat)
     pattern pat;
{
  unit u, *pu;
  REAL *rp, total_e = 0;
  REAL tot_act = 0;		/* Total activation for normed dist */
  REAL tot_act1 = 0;		/* Normalization constant for normed dist */

  if (error_type == 3)		/* normalized distribution */
    {
      FOROUTS(pu,rp,pat)
	{
	  REAL r = *rp;
	  unit u = *pu;
	  REAL y = u_y(u);

	  tot_act += y;

	  /* Comment out for speed, as it's constant wrt learning: */
	  if (r)
	    total_e += r*LOG(r);
	}
      tot_act1 = 1/tot_act;
      total_e += LOG(tot_act);
    }

  DOUNITS(u) u_external_dE_dy(u) = 0;

  FOROUTS(pu,rp,pat)
    {
      REAL r = *rp;		/* target value */
      if (r != DONTCARE)
	{
	  unit u = *pu;
	  REAL y = u_y(u);	/* current value */
	  REAL e;

	  switch (error_type)
	    {
	    case 0:		/* simple square */
	      u_external_dE_dy(u) = e = y-r;
	      total_e += e*e/2;
	      break;

	    case 1:		/* marginated square (for binary tasks) */
	      u_external_dE_dy(u) =
		e = 
		  (r > 0.5 ?
		   (y-(1-square_margin) > 0 ? 0 : y-(1-square_margin)) :
		   (y-square_margin < 0 ? 0 : y-square_margin));
	      total_e += e*e/2;
	      break;

	    case 2:		/* cross entrophy */

	      /*
	       * We use base e digits rather than bits as units of
	       * information so we can take natural logs.  This simplifies
	       * the derivatives also.  All we lose is a constant factor
	       * near 1, which comes out in the wash anyway.
	       
	       * Unused trick: compute dC/dx directly and skip dC/dy.
	       
	       * C = - d log y - (1-d) log (1-y)
	       * dC/dx = (dC/dy)(dy/dx) = -d/y+(1-d)/(1-y) y(1-y) = y-d
	       */

	      /* special case 0/0 == 0*log(0) == 0; otherwise signal
	       * math error as usual -- STEVER
	       */
	      if (y==0.0 && r==0.0) {
	         u_external_dE_dy(u) = e = 1.0;
	         total_e -= 0;
	      } else if (y==1.0 && r==1.0) {
	         u_external_dE_dy(u) = e = -1.0;
	         total_e -= 0;
	      } else {
		 /******* UGLY PATCH *********/
		 if (y==0.0) {
		     y = 0.00001;
		     printf(stderr, 
		      "bumping zero output %d for pattern %d at epoch %d", 
		      u, NUMBER_PAT(pat), Epoch);
		     printf(stderr, 
		      "\t(u_sig_x(%d) == %f, u_I(%d) == %f)\n",
		      u, u_sig_x(u), u_I(u));
		 } else if (y==1.0) {
		     y = 0.99999;
		     printf(stderr, 
		      "bumping 1.0 output %d for pattern %d at epoch %d", 
		      u, NUMBER_PAT(pat), Epoch);
		     printf(stderr, 
		      "\t(u_sig_x(%d) == %f, u_I(%d) == %f)\n",
		      u, u_sig_x(u), u_I(u));
		 }
		 /******* end UGLY PATCH *********/
	         u_external_dE_dy(u) = e = - r/y + (1 - r)/(1-y);
	         /* u_dE_dx(u) = u_y(u) - r; */
	         total_e -= r*LOG(y) + (1-r)*LOG(1-y);
	      }
	      break;

	    case 3:		/* normalized distribution */

	      /*
	       * As above, we use base e digits.  The idea here is that
	       * we normalize the outputs to sum to 1 before measuring
	       * the surprise of the actual distribution given the output
	       * distribution.
	       *
	       */
	      u_external_dE_dy(u) = tot_act1 - r/y;
	      total_e -= r*LOG(y);
	      break;

	    case 4:		/* gaussian w/ error bar */
	      {
		/* This is tricky because the net has two outputs: an
		   expected value and a standard deviation.  The
		   expected value is in the usual output unit, and the
		   stdev is in the NEXT unit.  We have to tease this
		   out. */

		unit stdu = u_next_unit(u);
		REAL std = u_y(stdu);
		REAL variance = std*std;
		e = y-r;

		u_external_dE_dy(u) = 2*e/variance;
		{ register REAL tempr = SQRT2*e;
		  u_external_dE_dy(stdu) =
		    (std - tempr)*(std + tempr)/(variance*std);
		}

		total_e += e*e/variance + log((double) std) + SQRT_PI_O2;
	      }
	      break;

	    default:
	      printf("Internal error -- error type %d unimplemented.\n",
		     error_type);
	      _del_ += 1;
	      break;
	    }
	  /* u_dE_dx(u) = y * (1 - y) * e; */
	}
    }
  return total_e;
}






REAL trajectory_backward_timestep(tick, pat)
     int tick;
     pattern pat;
{
  unit u;
  REAL total_e = 0;

  DOUNITS(u)
    {
      u_y(u)	 = u_y_movie(u)[tick];
      u_sig_x(u) = u_sig_x_movie(u)[tick];
      u_dy_dt(u) = u_dy_dt_movie(u)[tick];
    }

  total_e += compute_external_dE_dy(pat);

  compute_dz_dt();

  DOUNITS(u) u_z(u) -= delta_t * u_dz_dt(u);

  trajectory_accumulate_dw();

  return total_e;
}





REAL trajectory_backward_sweep(pat0)
     pattern pat0;
{
  REAL t = end_of_time;
  int tick = total_ticks - 1;
  unit u;
  pattern pat;
  REAL *patt_t = env_times  + env_steps-1;
  pattern *env = envs_array + env_steps-1;
  REAL total_e = 0;

  DOUNITS(u) u_z(u) = 0;

  while (t>=0)
    {
      /* time for previous environment step? */
      while (t < *patt_t)
	{
	  pat = *(--env) + (pat0 - *envs_array);
	  patt_t -= 1;
	}

      total_e += trajectory_backward_timestep(tick, pat);

      t -= delta_t;
      tick -= 1;
    }

  return delta_t*total_e;
}






				/* SUPPRESS 590 *//* fd unused */
void compute_B(p_total_B, fd)
     REAL *p_total_B;
     FILE *fd;
{
  /* Compute B and its derivatives' squared norm. */
  /* nb. B2 = 2*B. */
  REAL B2 = 0, B2_norm2 = 0, B;
  unit u, u2;

  DOUNITS(u) B2 += u_z(u) * u_z(u);
  DOUNITS(u) u_dB_dz0(u) = 0;
  DOUNITS(u) DOUNITS(u2) u_dB_dz0(u) += u_z(u2) * zeta(u2,u);

  DOUNITS(u) B2_norm2 += u_dB_dz0(u) * u_dB_dz0(u);
  B2_norm2 = B2/B2_norm2;

  B = B2/2;

  if (display_B)
    printf("B %g |B| %g ", (double) B, (double) sqrt((double) (B2/B2_norm2)));

  *p_total_B += B;

  /* Update the z0's. */
  DOUNITS(u) u_z0(u) -= dB_hack * B2_norm2*u_dB_dz0(u);
}








REAL trajectory_forward_sweep (pat0,save_dw,p_total_B,fd)
     pattern pat0;
     bool save_dw;
     REAL *p_total_B;
     FILE *fd;
{
  REAL total_error = 0;
  REAL t = 0;
  REAL time_until_perturb;
  int tick = 0;
  pattern pat = pat0;
  pattern *env = envs_array;
  REAL *patt_t = env_times;

  if (net_type == NET_TRAJ_GUESS_Z)
    {
      unit u, u2;
      DOUNITS(u) u_z0(u) = u_z(u) = pat0->z0_array[NUMBER_UNIT(u)];
      DOUNITS(u) DOUNITS(u2) zeta(u,u2) = (u == u2);
    }

  clear_states();
  setup_inputs(pat);
  I_ys();

  if (fd) dump_current_state(fd,t,true);

  if (perturb_interval)
    time_until_perturb = perturb_interval + perturb_interval*RANDOM()/2;

  while (t<=end_of_time)
    {
      /* time for next environment step? */
      while (t >= *(patt_t+1))
	{
	  pat = *++env + (pat0 - *envs_array);
	  patt_t += 1;
	  setup_inputs(pat);
	}

      if (net_type == NET_TRAJ_GUESS_Z)
	{
	  total_error += compute_external_dE_dy(pat);
	  (void)forward_timestep(save_dw);
	}
      else
	{
	  /* if (tick >= total_ticks)
	     fprintf(stderr,"Tick %d over bounds %d.\n",tick,total_ticks); */
	  (void)forward_timestep(tick++);
	}

      if (perturb_interval)
	{
	  if ((time_until_perturb -= delta_t) <= 0)
	    {
	      unit u;
	      DOUNITS(u)
		u_y(u) += perturb_mag * RANDOM();
	      time_until_perturb
		= perturb_interval + perturb_interval*RANDOM()/2;
	    }
	}
		/* SUPPRESS 544 *//* return void instead of double */
      if (_del_>1) return;
      if (fd) dump_current_state(fd,t,true);

      t += delta_t;
    }

  if (net_type == NET_TRAJ_GUESS_Z)
    {
      compute_B(p_total_B, fd);
      return total_error * delta_t / 2;
    }
}





REAL pineda_backward_timestep()
{
  unit u;
  wet w;
  int i;
  REAL total_change = 0;

  DOUNITS(u)
    {
      REAL sum = u_external_dE_dy(u);
      DOOUTGOING(u,w,i)
	sum += w_w(w) * u_dE_dx(w_to(w));
      u_z(u) = sum;
    }

  /* Bad old unvectorized code:
  DOUNITS(u) u_z(u) = u_external_dE_dy(u);
  DOWETS(w) u_z(w_from(w)) += w_w(w) * u_dE_dx(w_to(w));
  */

  DOUNITS(u)
    {
      REAL new = u_y(u) * (1 - u_y(u)) * u_z(u)
	, delta = u_dE_dx(u) - new;

      total_change += ABS(delta);
      u_dE_dx(u) = new;
    }

  return total_change / unit_count;
}




void normalize_outputs(pat)
     pattern pat;
{
  unit *pu;
  REAL *rp, tot_act_inv, tot_act = 0.0;
  FOROUTS(pu,rp,pat) tot_act += u_y(*pu);
  tot_act_inv = 1.0/tot_act;
  FOROUTS(pu,rp,pat) u_y(*pu) *= tot_act_inv;
}




REAL setup_outputs (pat)
     pattern pat;
{
  unit u;
  REAL total_e;

  total_e = compute_external_dE_dy(pat);

  DOUNITS(u)
    {
      u_z(u) = u_external_dE_dy(u);
      u_dE_dx(u) = u_y(u) * (1 - u_y(u)) * u_z(u);
    }

  if (norm_outputs) normalize_outputs(pat);

  return total_e;
}
   




/* This computes the derivatives, increments the per weight derivative
   sums, and returns the total error for this case. */

void pineda_backward_sweep ()
{
  REAL delta, t;
  wet w;

  t = 0;
  do {
    delta = pineda_backward_timestep();
    t += delta_t;
  } while (delta > back_criterion);

  DOWETS(w) w_dE_dw(w) += u_y(w_from(w)) * u_dE_dx(w_to(w));

  if (display_time) printf("%f ", (double) t);
}


/* This routine does enough work to get the final states for state dumping. */
REAL forward_part (e, save_dw, fd, p_total_B)
     pattern e;
     bool save_dw;
     FILE *fd;
     REAL *p_total_B;
{
  REAL total_e = 0;

  switch (net_type)
    {
    case NET_PINEDA:
      clear_states();
      setup_inputs(e);
      clear_ys();
      fixpoint_forward_sweep(fd);
      break;

    case NET_STANDARD:
      clear_states();
      setup_inputs(e);
      I_ys();
      forward_propagate();
      break;

    case NET_MEAN_FIELD:
      /* here: clamp inputs and outputs */
      fixpoint_forward_sweep(fd);
      if (save_dw)
	{
	  wet w;
	  
	  DOWETS(w) w_dE_dw(w) += u_y(w_to(w)) * u_y(w_from(w));
	}
      break;

    case NET_TRAJ_GUESS_Z:
      {
	unit u;
	int i = 0;
	REAL B = Huge;
	REAL *rp;

	/* Make some passes just to reduce B. */
	if (save_dw)
	  {
	    while (B_subepoch_count == -1 ? B>max_B : i<B_subepoch_count)
	      {
		unit u;

		B = 0;
		(void)trajectory_forward_sweep(e,false,&B,(FILE *)NULL);
					/* SUPPRESS 544 */
		if (_del_>1) return;
		rp = e->z0_array;
		FOREUNITS(u)
		  *rp++ = u_z0(u);
		i++;
	      }
	    if (B_subepoch_count == -1)
	      printf("%d,", i);
	  }

	B = 0;
	total_e += trajectory_forward_sweep(e,true,&B,fd);
	*p_total_B += B;
	if (save_dw)
	  {				/* SUPPRESS 544 */
	    if (_del_>1) return;
	    if (stop_high_B && B>max_B)
	      {
		printf("maximum B exceeded...\n");
		_del_ += 1;
	      }
	    /* Update the z0's stored for this pattern. */
	    rp = e->z0_array;
	    FOREUNITS(u)
	      *rp++ = u_z0(u);
	  }
      }
      break;

    case NET_TRAJ_BACKWARDS:
      {
	REAL temp;
	(void)trajectory_forward_sweep(e,save_dw,&temp,fd);
      }
      break;
    }
  return total_e;
}



				/* SUPPRESS 590 *//* fd unused */
REAL backward_part (e, save_dw, fd)
     pattern e;
     bool save_dw;
     FILE *fd;
{
  REAL total_e = 0;

  switch (net_type)
    {

    case NET_PINEDA:
      total_e += setup_outputs(e);
      pineda_backward_sweep();
      break;

    case NET_STANDARD:
      total_e += setup_outputs(e);
      backward_propagate();
      break;

    case NET_MEAN_FIELD:
      /* here: unclamp outputs */
      fixpoint_forward_sweep((FILE *)NULL);
      if (save_dw)
	{
	  wet w;
	  
	  DOWETS(w) w_dE_dw(w) -= u_y(w_to(w)) * u_y(w_from(w));
	}
      break;

    case NET_TRAJ_GUESS_Z:
      break;

    case NET_TRAJ_BACKWARDS:
      total_e += trajectory_backward_sweep(e);
      break;

    }
  return total_e;
}



REAL process_pattern(e, wrong_cases, total_B)
     pattern e;
     int *wrong_cases;
     REAL *total_B;
{
  REAL total_e = 0;

  if (net_type == NET_MEAN_FIELD)
    fprintf(stderr, " Mean field theory doesn't work.\n");

  /* total_e += forward_part(e, true, (FILE *)NULL, &total_B); */
  total_e += forward_part(e, true, (FILE *)NULL, total_B); /*SUPPRESS 544 */
  if (_del_>1) return;
  total_e += backward_part(e, true, (FILE *)NULL);	/* SUPPRESS 544 */
  if (_del_>1) return;

  if (display_B && net_type==NET_TRAJ_GUESS_Z) printf("\n");

  if (display_cases && !NET_TRAJ(net_type))
    {
      /* Count the number of wrong cases.  Just one wrong element
	 is enough; none of this best guess stuff. */
      REAL *rp;
      unit *pu;

      FOROUTS(pu,rp,e)
	{
	  REAL temp = u_y(*pu) - *rp;

	  if ((temp > correct_margin) || (temp < -correct_margin))
	    {
	      *wrong_cases += 1;
	      break;
	    }
	}
    }
  return total_e;
}

void one_go()
{
  int wrong_cases = 0, opn=online_pat_num;
  pattern e;
  wet w;
  unit u;
  REAL total_e = 0, total_B = 0;

  printf("%s %d", run_name, epoch+1);

  DOWETS(w) w_dE_dw(w) = 0;
  DOUNITS(u) u_dE_dT(u) = 0;

  if (batchsize == 0)
    {
      DOENV(e)
	{
	  total_e += process_pattern(e, &wrong_cases, &total_B);
	  if (_del_>1) return;
	}
      /* total_e /= patt_count; */
    }
  else if (batchsize > 0)
    {
      int i;
      for (i=0; i<batchsize; i++)
	{
	  if (opn == patt_count) opn = 0;
	  total_e += process_pattern(PAT_NUMBER(opn), &wrong_cases, &total_B);
	  opn += 1;
	  if (_del_>1) return;
	}
      /* total_e /= batchsize; */
    }
  else if (batchsize < 0)
    {
      int i;
      for (i=0; i < -batchsize; i++)
	{
	  total_e += process_pattern(PAT_NUMBER((int)RANDRANGE(0,patt_count)),
				     &wrong_cases, &total_B);
	  if (_del_>1) return;
	}
      /* total_e /= -batchsize; */
    }
  else
    fprintf(stderr, "Error: trichotomy axiom violated.\n");

  /* This is a noop except for trajectory networks: */
  correct_dw_constant_factors();

  total_e += decay_error();

  write_real(stdout, " ", total_e);

  if (display_cases && !NET_TRAJ(net_type)) {
    printf("(%d)", wrong_cases);
    if (stop_zero_errors && wrong_cases==0) {
      printf("\nZero cases wrong, halting simulation.\n");
      _del_ += 1;
      return;
    }
  }
  if (net_type == NET_TRAJ_GUESS_Z)
    printf(" B:%g", (double) total_B/patt_count);
  update_weights();
  if (batchsize) online_pat_num = opn;
  fflush(stdout);
}



void ensure_file(filebase, filebasebase, place, ext)
     char *filebase, *filebasebase, *place, *ext;
{
  if (*filebase == '\0')
    {
      if (*filebasebase == '\0')
	{
	  printf("Checkpoint file (base): ");
	  read_filename(filebase);
	}
      else
	{
	  strcpy(filebase, filebasebase);
	  strcat(filebase, ext);
	}
    }
  strcpy(place,filebase);
  sprintf(&place[strlen(place)], ".%d", epoch);
  printf(" writing %s.\n", place);
}


				/* SUPPRESS 590 *//* ignored unused */
void checkpoint_weights(ignored)
     char *ignored;
{
  char temp_buff[MAXPATHLEN];
  printf("Checkpointing weights...");
  ensure_file(check_file, run_name, temp_buff, ".wet");
  write_wet(temp_buff);
}


			/* SUPPRESS 590 *//* ignored unused */
void checkpoint_states(ignored)
     char *ignored;
{
  char temp_buff[MAXPATHLEN];
  printf("Checkpointing states...");
  ensure_file(scheck_file, run_name, temp_buff, ".state");
  write_states(temp_buff);
}

				/* SUPPRESS 590 *//* ignored unused */
void checkpoint_trajectory(ignored)
     char *ignored;
{
  char temp_buff[MAXPATHLEN];
  printf("Checkpointing trajectory...");
  ensure_file(tcheck_file, run_name, temp_buff, ".traj");
  write_trajectory(temp_buff);
}



int compute_total_ticks()
{
  int tick=0;
  REAL t=0;
  while (t<end_of_time)
    {
      t+=delta_t;
      tick+=1;
    }
  return tick+1;
}





void allocate_movie_space()
{
  unit u;

  total_ticks = compute_total_ticks();

  if (total_ticks <= allocated_ticks) return;

  fprintf(stderr, "Allocating %d ticks, was %d.\n", total_ticks, allocated_ticks);
  allocated_ticks = total_ticks;
  DOUNITS(u)
    {
      REALLOC(u_y_movie(u),     REAL, total_ticks);
      REALLOC(u_sig_x_movie(u), REAL, total_ticks);
      REALLOC(u_dy_dt_movie(u), REAL, total_ticks);
    }
}







void go(arglist)
     char *arglist;
{
  int epochs, i;

  epochs = intarg(&arglist,0,"epochs:",0,MAX_COOL_INT,1);

  if (net_type == NET_TRAJ_BACKWARDS) allocate_movie_space();

  for (i=0; i<epochs; i++)
    {
      Epoch = i;	/* debugging */
      one_go();

      if (check_interval && epoch % check_interval == 0)
	checkpoint_weights("");
      if (scheck_interval && epoch % scheck_interval == 0)
	checkpoint_states("");
      if (tcheck_interval && epoch % tcheck_interval == 0)
	checkpoint_trajectory("");

      DELBREAK;
    }
}




void state_writer(file,traj)
     char *file;
     bool traj;
{
  FILE *fd = prompt_fopen_with_ext(file, "", "w", "State file to write");
  int i;
  pattern e;
  REAL temp;

  if (fd == NULL) return;

  if (net_type == NET_TRAJ_BACKWARDS) allocate_movie_space();

  fprintf(fd, "%d units\n%d patterns\n", unit_count, patt_count);

  i=0;
  FOREENV(e)
    {
      if (traj) fprintf(fd, "pattern %d\n", i);
      (void)forward_part(e, false, traj ? fd : (FILE *)NULL, &temp);
      if (norm_outputs) normalize_outputs(e);
      if (!traj) dump_current_state(fd, DONTCARE, false);
      i += 1;
    }

  fclose(fd);
}




void write_states(file)
     char *file;
{
  state_writer(file,false);
}

void write_trajectory(file)
     char *file;
{
  state_writer(file,true);
}





/* Network description and environment files are in bpla format. */

void read_net(file)
     char *file;
{
  int i;
  FILE *fd = prompt_fopen_with_ext(file, ".net", "r",
				   "Network description file to read");
  unit u;
  wet w;

  if (fd==NULL) exit(1);

  fscanf_1(fd, "%d units\n", &unit_count);
  unit_array = MALLOC(struct unit_str,unit_count);

  low_printing_unit = 0;
  high_printing_unit = unit_count-1;

  allocated_ticks = 1;

  DOUNITS(u)
    {
      u_in_count(u) = 0;
      u_out_count(u) = 0;
      u_transfer(u) = STANDARD_SIGMOID;
      u_zetarray(u) = MALLOC(REAL,unit_count);

      u_y_movie(u)     = MALLOC(REAL, allocated_ticks);
      u_sig_x_movie(u) = MALLOC(REAL, allocated_ticks);
      u_dy_dt_movie(u) = MALLOC(REAL, allocated_ticks);
    }

  fscanf_1(fd, "%d inputs\n", &input_count);
  io_array = MALLOC(unit, input_count);
  for (i=0; i<input_count; i++)
    io_array[i] = UNIT_NUMBER(read_int(fd));

  fscanf_1(fd, "%d outputs\n", &output_count);
  io_count = input_count + output_count;
  io_array = REALLOC(io_array, unit, io_count);
  input_array = io_array;
  output_array = io_array + input_count;
  for (i=0; i<output_count; i++)
    output_array[i] = UNIT_NUMBER(read_int(fd));

  fscanf_1(fd, "%d conns\n", &conn_count);
  conn_array = MALLOC(struct wet_str, conn_count);

  FOREWETS(w)
    {
      u_out_count( w_from(w) = UNIT_NUMBER(read_int(fd)) )  += 1;
      u_in_count(  w_to(w)   = UNIT_NUMBER(read_int(fd)) )  += 1;
    }

  DOUNITS(u)
    {
      u_ins(u) = MALLOC(wet, u_in_count(u));
      u_outs(u) = MALLOC(wet, u_out_count(u));
    }
  DOWETS(w)
    {
      *(u_outs(w_from(w)))++ = w;
      *(u_ins(w_to(w)))++ = w;
    }
  DOUNITS(u)
    {
      u_ins(u) -= u_in_count(u);
      u_outs(u) -= u_out_count(u);
    }

  /* Check for link equivalences.  They are specified by a ':' after
     the link specs, followed by stuff like this:
     
       3 equivalences
       3 47 48 49
       2 19 20
       2 52 53

     where the first number in each row specifies the number of
     weights in each equivalence, and the rest are the connection
     numbers, zero based. */

  if (char_p(fd, ':'))
    {
      fscanf_1(fd, "%d equivalences\n", &wet_equiv_count);
      wet_equivs = MALLOC(struct wet_equiv_str, wet_equiv_count);
      for (i=0; i<wet_equiv_count; i++)
	{
	  int j, n = read_int(fd);
	  wet_equivs[i].count = n;
	  wet_equivs[i].guys = MALLOC(wet,n);
	  for (j=0; j<n; j++)
	    wet_equivs[i].guys[j] = WET_NUMBER(read_int(fd));
	}
    }

  if (char_p(fd, 't')) {
    FOREUNITS(u)
      fscanf_1(fd, "%d", &u_transfer(u));
  }

  fclose(fd);

  printf("%d units, %d inputs, %d outputs, %d connections, %d equivalences.\n",
	 unit_count, input_count, output_count, conn_count, wet_equiv_count);
}





void initialize()
{

#ifdef USE_SIGMOID_TABLE
  make_sigmoid_table();
#endif

  {
    extern long time();
    char a[100];

    sprintf(a,"%f %ld", (double) 1.0, time((long *)NULL));
    randomize_weights(a);
  }

  clear_momentum();
  one_Ts();

  env_times = MALLOC(REAL, 1);
  env_times[0] = 0;
  envs_array = MALLOC(pattern, 1);

  /* This is where defaults for the parameters go. */

  batchsize = 0;

  for_criterion = 1e-3;
  back_criterion = 1e-6;
  delta_t = .1;
  error_type = 0;		/* continuous square */
  square_margin = 0.2;
  weight_decay = 0;
  T_decay = 0;
  correct_margin = 0.4;		/* liberal, for symbolic tasks */
  max_B = .1;
  dB_hack = 1;
  stop_slosh = TRUE;
  stop_zero_errors = FALSE;
  display_cases = FALSE;
  display_Ev = TRUE;
  display_len_w = TRUE;
  perturb_interval = 0;

  epsilon = 0;
  momentum = 0;
  bmomentum = 0;

  autopilot = false;
  autolimit = 0.85;
  autoraise = 1.1;
  autolower = 0.9;
  automuchlower = 0.5;

  norm_outputs = false;
  weight_seed = -1;
  weight_excursion = -1.0;
}




#ifdef USE_DOUBLES
#define PROMPT "dcbp>"
#else
#define PROMPT "cbp>"
#endif




main(argc, argv)
     int argc;
     char **argv;
{
  if (argc != 2)
    {
      fprintf(stderr, "Usage: %s net.\n", argv[0]);
      exit(1);
    }

  read_net(argv[1]);
  initialize();

  /* The "warning: illegal pointer combination" here is due to "int del()"
     instead of "void del()" like the handler structure wants.  Nothing can
     be done. */
					/* SUPPRESS 544 */
  ENABLEDEL;

  ci(PROMPT, 0, 0, ci_list, 0, 0);
  return 0;
}
