/**
 * Gouverneur Thomas - 2011
 *
 * Quickly find last login's date of a user
 *
 * ripped off finger.c / openindiana
 *
 * Compile:
 *   # gcc -o lastlog lastlog.c
 */

#include <sys/time.h>
#include <sys/ddi.h>
#include <utmpx.h>
#include <stdio.h>
#include <pwd.h>
#include <lastlog.h>

#define NMAX    sizeof (((struct utmpx *)0)->ut_name)
#define LMAX    sizeof (((struct utmpx *)0)->ut_line)
#define HMAX    sizeof (((struct utmpx *)0)->ut_host)
#define MAX_LOGIN_LENGTH 200

struct person {                         /* one for each person fingered */
        char *name;                     /* name */
        char tty[LMAX+1];               /* null terminated tty line */
        char host[HMAX+1];              /* null terminated remote host name */
        char *ttyloc;                   /* location of tty line, if any */
        time_t loginat;                 /* time of (last) login */
        time_t idletime;                /* how long idle (if logged in) */
        char *realname;                 /* pointer to full name */
        struct passwd *pwd;             /* structure of /etc/passwd stuff */
        char loggedin;                  /* person is logged in */
        char writable;                  /* tty is writable */
        char original;                  /* this is not a duplicate entry */
        struct person *link;            /* link to next person */
};

void findwhen(struct person *);
void shortprint(struct person *);
void stimeprint(time_t *);

FILE *lf = NULL;
char LASTLOG[] = "/var/adm/lastlog";    /* last login info */
time_t tloc;

int main(int ac, char**av) {

  struct passwd *pw;
  struct person *pers;

  if (ac != 2) {
    fprintf(stderr, "[!] Please specify username\n");
    return -1;
  }
  
  if (strlen(av[1]) > MAX_LOGIN_LENGTH) {
    fprintf(stderr, "[!] Invalid username\n");
    return -1;
  }

  tloc = time(NULL);

  pers = (struct person *) malloc(sizeof(struct person));
  pers->name = strdup(av[1]);

  if ((lf = fopen(LASTLOG, "r")) == NULL) {
    fprintf(stderr, "[!] Cannot open %s\n", LASTLOG);
    return -1;
  }

  if ((pw = getpwnam(pers->name)) == NULL) {
    fprintf(stderr, "[!] Unable to gather pw entry for %s\n", pers->name);
    return -1;
  }
 
  pers->pwd = pw;
  findwhen(pers);

  if (lf != NULL)
    (void) fclose(lf);

  shortprint(pers);

  return 0;
}


void
findwhen(struct person *pers)
{
        struct lastlog ll;

        if (lf != NULL) {
                if (fseeko(lf, (off_t)pers->pwd->pw_uid * (off_t)sizeof (ll),
                    SEEK_SET) == 0) {
                        if (fread((char *)&ll, sizeof (ll), 1, lf) == 1) {
                                int l_max, h_max;

                                l_max = min(LMAX, sizeof (ll.ll_line));
                                h_max = min(HMAX, sizeof (ll.ll_host));

                                bcopy(ll.ll_line, pers->tty, l_max);
                                pers->tty[l_max] = '\0';
                                bcopy(ll.ll_host, pers->host, h_max);
                                pers->host[h_max] = '\0';
                                pers->loginat = ll.ll_time;
                        } else {
                                if (ferror(lf))
                                        (void) fprintf(stderr,
                                            "finger: %s read error\n", LASTLOG);
                                pers->tty[0] = 0;
                                pers->host[0] = 0;
                                pers->loginat = 0L;
                        }
                } else {
                        (void) fprintf(stderr, "finger: %s fseeko error\n",
                            LASTLOG);
                }
        } else {
                pers->tty[0] = 0;
                pers->host[0] = 0;
                pers->loginat = 0L;
        }
}


void
shortprint(struct person *pers)
{
        char *p;
        int tloc =0;

        (void) printf("%s", pers->pwd->pw_name);
        p = ctime(&pers->loginat);
        if (pers->loggedin) {
                stimeprint(&pers->idletime);
                (void) printf(" %3.3s %-5.5s ", p, p + 11);
        } else if (pers->loginat == 0) {
                (void) printf(" < .  .  .  . >");
        } else if (tloc - pers->loginat >= 180 * 24 * 60 * 60) {
                (void) printf(" <%-6.6s, %-4.4s>", p + 4, p + 20);
        } else {
                (void) printf(" <%-12.12s>", p + 4);
        }
        (void) putchar('\n');
}

void
stimeprint(time_t *dt)
{
        struct tm *delta;

        delta = gmtime(dt);
        if (delta->tm_yday == 0)
                if (delta->tm_hour == 0)
                        if (delta->tm_min == 0)
                                (void) printf("    ");
                        else
                                (void) printf("  %2d", delta->tm_min);
                else
                        if (delta->tm_hour >= 10)
                                (void) printf("%3d:", delta->tm_hour);
                        else
                                (void) printf("%1d:%02d",
                                    delta->tm_hour, delta->tm_min);
        else
                (void) printf("%3dd", delta->tm_yday);
}

