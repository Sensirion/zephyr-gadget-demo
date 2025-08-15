#ifndef THREADS_H
#define THREADS_H

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL

/* size of stack area used by each thread */
#define STACKSIZE_DEFAULT 1024
#define STACKSIZE_DISPLAY 4096

/* scheduling priority used by each thread */
#define PRIORITY 7

void display(void *, void *, void *);
void scd(void *, void *, void *);
void sht(void *, void *, void *);
void display_l(void);
void scd_l(void);
void sht_l(void);

#endif /* THREADS_H */
