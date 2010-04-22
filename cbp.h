/* Include file for the cbp simulator. */

#ifndef USE_SINGLES

typedef double REAL;
#define SCAN_REAL "%F"

#else

typedef float REAL;
#define SCAN_REAL "%f"

#endif


#define MAXINT (0x7fffffff)  /* 2147483647 */
#define MININT (-MAXINT-1)

#define MAX_COOL_INT 1000000000

/* This is used as a distinguished "don't care" or "no change" token for
   inputs and outputs in the environment. */

#define DONTCARE	((REAL)123456789)

/* This is a large floating point number.  Would just use HUGE, but on
   some machines it has nasty properties like (HUGE == -HUGE) and
   (HUGE<0 && !(HUGE>0)). */
#define Huge	(1e20)




/* typedef struct unit_str *unit; */
typedef int unit;

/* typedef struct wet_str *wet; */
typedef int wet;




struct wet_str {
  unit from, to;
  REAL w, dE_dw, velocity, acceleration;
};

#define w_from(x)		(conn_array[x].from)
#define w_to(x)			(conn_array[x].to)
#define w_w(x)			(conn_array[x].w)
#define w_dE_dw(x)		(conn_array[x].dE_dw)
#define w_velocity(x)		(conn_array[x].velocity)
#define w_acceleration(x)	(conn_array[x].acceleration)



struct unit_str {
  int in_count, out_count;
  int transfer;			/* transfer function used */
  wet *ins, *outs;		/* vectors of incoming and outgoing weights */
  REAL x;			/* total input */
  REAL sig_x;			/* sigmoid(x) */
  REAL y;			/* activation level */
  REAL T;			/* time constant */
  REAL I;			/* external input */
  REAL external_dE_dy;		/* external error signal */
  /* This is for regular backprop mode. */
  bool y_valid;
  /* These are for trajectory forward running. */
  REAL dy_dt;
  REAL dE_dx;
  REAL z;			/* dE/dy */
  /* These are for trajectory learning. */
  REAL dz_dt, dE_dT, velocity, acceleration, z0, dB_dz0;
  REAL *y_movie, *sig_x_movie, *dy_dt_movie;
  REAL *zetarray;
  REAL temp;			/* for scratch use. */
};

#define u_in_count(u)		(unit_array[u].in_count)
#define u_out_count(u)		(unit_array[u].out_count)

#define u_transfer(u)		(unit_array[u].transfer)

#define u_ins(u)		(unit_array[u].ins)
#define u_outs(u)		(unit_array[u].outs)

#define u_x(u)			(unit_array[u].x)
#define u_sig_x(u)		(unit_array[u].sig_x)
#define u_y(u)			(unit_array[u].y)
#define u_T(u)			(unit_array[u].T)
#define u_I(u)			(unit_array[u].I)
#define u_external_dE_dy(u)	(unit_array[u].external_dE_dy)
#define u_dy_dt(u)		(unit_array[u].dy_dt)
#define u_dE_dx(u)		(unit_array[u].dE_dx)
#define u_z(u)			(unit_array[u].z)
#define u_y_valid(u)		(unit_array[u].y_valid)
#define u_dz_dt(u)		(unit_array[u].dz_dt)
#define u_dE_dT(u)		(unit_array[u].dE_dT)
#define u_velocity(u)		(unit_array[u].velocity)
#define u_acceleration(u)	(unit_array[u].acceleration)
#define u_z0(u)			(unit_array[u].z0)
#define u_dB_dz0(u)		(unit_array[u].dB_dz0)
#define u_y_movie(u)		(unit_array[u].y_movie)
#define u_sig_x_movie(u)	(unit_array[u].sig_x_movie)
#define u_dy_dt_movie(u)	(unit_array[u].dy_dt_movie)
#define u_zetarray(u)		(unit_array[u].zetarray)
#define u_temp(u)		(unit_array[u].temp)

#define u_next_unit(u)		((u)+1)







typedef struct patt_str {
  REAL *input, *output;
  int input_count, output_count;
  unit *input_array, *output_array;
  REAL *z0_array;
} *pattern;

struct wet_equiv_str {
  wet *guys;
  int count;
};


extern void go(), read_env(), read_wet(), write_wet(),
  clear_momentum(), one_Ts(), randomize_weights(), weight_stats(),
  write_states(), write_trajectory(), read_z0s(), write_z0s(),
  checkpoint_weights(), checkpoint_states(), checkpoint_trajectory(),
  cd_cmd(), pwd_cmd(), multiply_epsilon(), divide_epsilon();


/* Shouldn't this be in ci.h? */
extern FILE *ciinput;
