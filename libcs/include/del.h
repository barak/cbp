/*
 * Copyright (c) 1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND CARNEGIE MELLON UNIVERSITY
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT
 * SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Users of this software agree to return to Carnegie Mellon any
 * improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 *
 * Export of this software is permitted only after complying with the
 * regulations of the U.S. Deptartment of Commerce relating to the
 * Export of Technical Data.
 */
/*  del  --  interrupt handling macros
 *
 * Revision 1.2  90/12/12  20:56:03  mja
 */

#include <signal.h>

extern int del();
extern int _del_;
extern struct sigvec _del_vec;

#define ENABLEDEL {_del_=0; _del_vec.sv_handler=del; sigvec(SIGINT,&_del_vec,0);}
#define DISABLEDEL {_del_=0; _del_vec.sv_handler=SIG_DFL; sigvec(SIGINT,&_del_vec,0);}
#define IGNOREDEL {_del_=0; _del_vec.sv_handler=SIG_IGN; sigvec(SIGINT,&_del_vec,0);}

#define _DELNOTE_	_del_=0; fprintf (stderr,"  Break!\n"); fflush (stderr);
#define DELBREAK	if (_del_) {_DELNOTE_ break;}
#define DELRETURN	if (_del_) {_DELNOTE_ return;}
#define DELRETN(x)	if (_del_) {_DELNOTE_ return (x);}
#define DELCLEAR	if (_del_) {_del_=0; fprintf (stderr,"Break ignored.\n"); fflush (stderr);}
