/* Session control */
#include <stdio.h>
#include "global.h"
#include "config.h"
#include "mbuf.h"
#include "netuser.h"
#include "timer.h"
#include "tcp.h"
#include "ax25.h"
#include "lapb.h"
#include "iface.h"
#include "ftp.h"
#include "telnet.h"
#ifdef _FINGER
#include "finger.h"
#endif
#ifdef NETROM
#include "netrom.h"
#include "nr4.h"
#endif
#include "session.h"
#include "cmdparse.h"
#ifdef	UNIX
#include <string.h>
#endif

struct session *sessions;
struct session *current;
char notval[] = "Not a valid control block\n";
char badsess[] = "Invalid session\n";

/* Convert a character string containing a decimal session index number 
 * into a pointer. If the arg is NULLCHAR, use the current default session.
 * If the index is out of range or unused, return NULLSESSION.
 */
static struct session *
sessptr(cp)
char *cp;
{
	register struct session *s;
	unsigned int i;

	if(cp == NULLCHAR){
		s = current;
	} else {
		if((i = atoi(cp)) >= nsessions)
			return NULLSESSION;
		s = &sessions[i];
	}
	if(s == NULLSESSION || s->type == FREE)
		return NULLSESSION;

	return s;
}

/* Select and display sessions */
dosession(argc,argv)
int argc;
char *argv[];
{
	struct session *s;
	extern char *tcpstates[];
	char *psocket();
	extern char *ax25states[];
	char *tcp_port();

	if(argc > 1){
		if((current = sessptr(argv[1])) != NULLSESSION)
			go_mode();
		return 0;
	}
	printf(" #       &CB Type   Rcv-Q  State        Remote socket\n");
	for(s=sessions; s < &sessions[nsessions];s++){
		switch(s->type){
		case TELNET:
			printf("%c%-3d%8lx Telnet  %4d  %-13s%-s:%s",
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.telnet->tcb,
			 s->cb.telnet->tcb->rcvcnt,
			 tcpstates[s->cb.telnet->tcb->state],
			 s->name,
			 tcp_port(s->cb.telnet->tcb->conn.remote.port));
			break;
		case FTP:
			printf("%c%-3d%8lx FTP     %4d  %-13s%-s:%s",
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.ftp->control,
			 s->cb.ftp->control->rcvcnt,
			 tcpstates[s->cb.ftp->control->state],
			 s->name,
			 tcp_port(s->cb.ftp->control->conn.remote.port));
			break;
#ifdef	AX25
		case AX25TNC:
			printf("%c%-3d%8lx AX25    %4d  %-13s%-s",
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.ax25_cb,
			len_mbuf(s->cb.ax25_cb->rxq),
			 ax25states[s->cb.ax25_cb->state],
			 s->name);
			break;
#endif
#ifdef	_FINGER
		case FINGER:
			printf("%c%-3d%8lx Finger  %4d  %-13s%-s",
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.finger->tcb,
			 s->cb.finger->tcb->rcvcnt,
			 tcpstates[s->cb.finger->tcb->state],
			 s->name, s->cb.finger->tcb->conn.remote.port);
			break;
#endif
#ifdef NETROM
		case NRSESSION:
			printf("%c%-3d%8lx NET/ROM %4d  %-13s%-s",
			 (current == s)? '*':' ',
			 (int)(s - sessions),
			 (long)s->cb.nr4_cb,
			len_mbuf(s->cb.nr4_cb->rxq),
			 Nr4states[s->cb.nr4_cb->state],
			 s->name);
			break;
#endif
		default:
			continue;
		}
		if(s->rfile != NULLCHAR || s->ufile != NULLCHAR)
			printf("\t");
		if(s->rfile != NULLCHAR)
			printf("Record: %s ",s->rfile);
		if(s->ufile != NULLCHAR)
			printf("Upload: %s",s->ufile);
		printf("\n");
	}
	return 0;
}

/* Enter conversational mode with current session */
int
go_mode()
{
	if(current == NULLSESSION || current->type == FREE)
		return 0;
	if (current->type == TELNET)
		tel_setterm(current->cb.telnet);
	go();
}


int
go()
{
	void rcv_char(),ftpccr(),ax_rx(),fingcli_rcv() ;

	if(current == NULLSESSION || current->type == FREE)
		return 0;
	mode = CONV_MODE;
	switch(current->type){
	case TELNET:
		rcv_char(current->cb.telnet->tcb,0); /* Get any pending input */
		break;
	case FTP:
		ftpccr(current->cb.ftp->control,0);
		break;
#ifdef	AX25
	case AX25TNC:
		ax_rx(current->cb.ax25_cb,0);
		break;
#endif
#ifdef _FINGER
	case FINGER:
		fingcli_rcv(current->cb.finger->tcb,0) ;
		break ;
#endif
#ifdef	NETROM
	case NRSESSION:
		nr4_rx(current->cb.nr4_cb,0) ;
		break ;
#endif
	}
	return 0;
}
doclose(argc,argv)
int argc;
char *argv[];
{
	struct session *s;

	if((s = sessptr(argc > 1 ? argv[1] : NULLCHAR)) == NULLSESSION){
		printf(badsess);
		return -1;
	}
	switch(s->type){
	case TELNET:
		close_tcp(s->cb.telnet->tcb);
		break;
	case FTP:
		close_tcp(s->cb.ftp->control);
		break;
#ifdef	AX25
	case AX25TNC:
		disc_ax25(s->cb.ax25_cb);
		break;
#endif
#ifdef _FINGER
	case FINGER:
		close_tcp(s->cb.finger->tcb);
		break;
#endif
#ifdef NETROM
	case NRSESSION:
		disc_nr4(s->cb.nr4_cb) ;
		break ;
#endif
	}
	return 0;
}
doreset(argc,argv)
int argc;
char *argv[];
{
	long htol();
	struct session *s;

	if((s = sessptr(argc > 1 ? argv[1] : NULLCHAR)) == NULLSESSION){
		printf(badsess);
		return -1;
	}
	switch(s->type){
	case TELNET:
		reset_tcp(s->cb.telnet->tcb);
		break;
	case FTP:
		if(s->cb.ftp->data != NULLTCB){
			reset_tcp(s->cb.ftp->data);
			s->cb.ftp->data = NULLTCB;
		}
		reset_tcp(s->cb.ftp->control);
		break;
#ifdef	AX25
	case AX25TNC:
		reset_ax25(s->cb.ax25_cb);
		break;
#endif
#ifdef _FINGER
	case FINGER:
		reset_tcp(s->cb.finger->tcb);
		break;
#endif
#ifdef NETROM
	case NRSESSION:
		reset_nr4(s->cb.nr4_cb) ;
		break ;
#endif
	}
	return 0;
}
int
dokick(argc,argv)
int argc;
char *argv[];
{
	long htol();
	void tcp_timeout();
	struct session *s;

	if((s = sessptr(argc > 1 ? argv[1] : NULLCHAR)) == NULLSESSION){
		printf(badsess);
		return -1;
	}
	switch(s->type){
	case TELNET:
		if(kick_tcp(s->cb.telnet->tcb) == -1){
			printf(notval);
			return 1;
		}
		break;
	case FTP:
		if(kick_tcp(s->cb.ftp->control) == -1){

			printf(notval);
			return 1;
		}
		if(s->cb.ftp->data != NULLTCB)
			kick_tcp(s->cb.ftp->data);
		break;
#ifdef	AX25
	case AX25TNC:
		if(kick_ax25(s->cb.ax25_cb) == -1){
			printf(notval);
			return 1;
		}
		return 1;
#endif
#ifdef _FINGER
	case FINGER:
		if(kick_tcp(s->cb.finger->tcb) == -1){
			printf(notval);
			return 1;
		}
		break;
#endif
#ifdef NETROM
	case NRSESSION:
		if(kick_nr4(s->cb.nr4_cb) == -1) {
			printf(notval) ;
			return 1 ;
		}
		break ;
#endif
	}
	return 0;
}
struct session *
newsession()
{
	register int i;

	for(i=0;i<nsessions;i++)
		if(sessions[i].type == FREE)
			return &sessions[i];
	return NULLSESSION;
}
freesession(s)
struct session *s;
{
	if(s == NULLSESSION)
		return;
	if(s->record != NULLFILE){
		fclose(s->record);
		s->record = NULLFILE;
	}
	if(s->rfile != NULLCHAR){
		free(s->rfile);
		s->rfile = NULLCHAR;
	}
	if(s->upload != NULLFILE){
		fclose(s->upload);
		s->upload = NULLFILE;
	}
	if(s->ufile != NULLCHAR){
		free(s->ufile);
		s->ufile = NULLCHAR;
	}
	if(s->name != NULLCHAR){
		free(s->name);
		s->name = NULLCHAR;
	}
	s->type = FREE;
}
/* Control session recording */
dorecord(argc,argv)
int argc;
char *argv[];
{
	if(current == NULLSESSION){
		printf("No current session\n");
		return 1;
	}
	if(argc > 1){
		if(current->rfile != NULLCHAR){
			fclose(current->record);
			free(current->rfile);
			current->record = NULLFILE;
			current->rfile = NULLCHAR;
		}
		/* Open new record file, unless file name is "off", which means
		 * disable recording
		 */
		if(strcmp(argv[1],"off") != 0
		 && (current->record = fopen(argv[1],"a")) != NULLFILE){
			current->rfile = malloc((unsigned)strlen(argv[1])+1);
			strcpy(current->rfile,argv[1]);
		}
	}
	if(current->rfile != NULLCHAR)
		printf("Recording into %s\n",current->rfile);
	else
		printf("Recording off\n");
	return 0;
}
/* Control file transmission */
doupload(argc,argv)
int argc;
char *argv[];
{
	struct tcb *tcb;
	struct ax25_cb *axp;
#ifdef NETROM
	struct nr4cb *cb ;
#endif

	if(current == NULLSESSION){
		printf("No current session\n");
		return 1;
	}
	if(argc > 1){
		switch(current->type){
		case TELNET:
			tcb = current->cb.telnet->tcb;
			break;
#ifdef	AX25
		case AX25TNC:
			axp = current->cb.ax25_cb;
			break;
#endif
#ifdef NETROM
		case NRSESSION:
			cb = current->cb.nr4_cb ;
			break ;
#endif
		case FTP:
			printf("Uploading on FTP control channel not supported\n");
			return 1;
		}
		if(strcmp(argv[1],"stop") == 0 && current->upload != NULLFILE){
			/* Abort upload */
			fclose(current->upload);
			current->upload = NULLFILE;
			if(current->ufile != NULLCHAR){
				free(current->ufile);
				current->ufile = NULLCHAR;
			}
		}
		/* Open upload file */
		if((current->upload = fopen(argv[1],"r")) == NULLFILE){
			printf("Can't read %s\n",argv[1]);
			return 1;
		}
		current->ufile = malloc((unsigned)strlen(argv[1])+1);
		strcpy(current->ufile,argv[1]);
		/* All set, kick transmit upcall to get things rolling */
		switch(current->type){
#ifdef	AX25
		case AX25TNC:
			(*axp->t_upcall)(axp,axp->paclen * axp->maxframe);
			break;
#endif
#ifdef NETROM
		case NRSESSION:
			(*cb->t_upcall)(cb, NR4MAXINFO) ;
			break ;
#endif
		case TELNET:
			(*tcb->t_upcall)(tcb,tcb->snd.wnd - tcb->sndcnt);
			break;
		}
	}
	if(current->ufile != NULLCHAR)
		printf("Uploading %s\n",current->ufile);
	else
		printf("Uploading off\n");
	return 0;
}
