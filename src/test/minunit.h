#ifndef __MIN_UNIT_H__
#define __MIN_UNIT_H__

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                               if (message) return message; } while (0)
extern int tests_run;

#endif // __MIN_UNIT_H__
