#include <stdio.h>
#include <epicsExport.h>
#include <registryFunction.h>
#include <time.h>

//#include <bc635.h>
#include "timeLib.h"
#include "bc635Aliases.h"
#include <genSubRecord.h>

/* Record inputs. CARD_ENABLE is set to zero (disable) in initTime. It can
 * be reenabled in the startup.
 */
#define DEBUG_FLAG      *((long *) (pgsub->a))  /* 1=debug, 0=no debug */
#define CARD_ENABLE     *((long *) (pgsub->b))  /* enable time card */

/* Bancom time card present. This value is set (once) in initTime
 * when the record is initialized.
 */
#define CARD_FOUND      *((long *) (pgsub->valb))

/* These values are set in the getTime every time the record runs.
 * The output is an array of four bytes to allow the Linux timeProbe to
 * read these values in a single get operation.
 */
#define CARD_CARDSTAT   *((double *) (pgsub->vala) + 0)
#define CARD_TIMESTA    *((double *) (pgsub->vala) + 1)
#define CARD_TIME       *((double *) (pgsub->vala) + 2)
#define CARD_REGS       *((double *) (pgsub->vala) + 3)
#define RT_CLOCK        *((double *) (pgsub->vala) + 4)
#define MONO_CLOCK      *((double *) (pgsub->vala) + 5)
#define MONO_DIFF       *((double *) (pgsub->vala) + 6)

/********************************************************************
 *+
 * FUNCTION NAME:
 * initTime
 *
 * INVOCATION:
 * status = initTime();
 *
 * PURPOSE:
 * Checks whether the bancom time card is present if compiled with Bancomm 
 * support if not it will always claim no card is found.
 *
 * DESCRIPTION:
 * Call bcTestCard() to determine whether the Bancom card is available.
 * Use VALB to save this status for future invocations of the record.
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)  
 * (>) pgsub  (struct genSubRecord *)    Pointer to gensub record structure
 *
 * EPICS OUTPUT FIELDS:
 * (int) VALB   bancom card present (1=present, 0=not present).
 *
 * FUNCTION VALUE:
 * Always zero
 * 
 * DEFICIENCIES:
 * None.
 *-
 */
long initTime (struct genSubRecord *pgsub)
{
    /* The card enable flag is reset by default since normally IOCs should
     * not use the time card even if it's present. The card can still be enabled
     * in the startup script or at run time for special cases.
     */
    CARD_ENABLE = 0;

    if (bcTestCard() == 0) { /* card found */
        printf ("initTime:Bancom time card found (enable=%ld)\n", CARD_ENABLE); 
        CARD_FOUND = 1;
    } else
        CARD_FOUND = 0;

    return 0;
}

/********************************************************************
 *+
 * FUNCTION NAME:
 * getDoubleTime
 *
 * INVOCATION:
 * time = getDoubleTime(ts);
 *
 * PURPOSE:
 * Helper function to not repeat that big operation everywhere
 *
 * DESCRIPTION:
 * Convert the time to seconds (including the nanosecond contribution)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)  
 * (>) ts   (struct timespec *)    Pointer to timespec structure
 *
 * FUNCTION VALUE:
 * Time in seconds
 */
double getDoubleTime(struct timespec *ts)
{
    return(((double)ts->tv_sec) + ((double)ts->tv_nsec)/1e9);
}

/********************************************************************
 *+
 * FUNCTION NAME:
 * getTime
 *
 * INVOCATION:
 * status = getTime();
 *
 * PURPOSE:
 * Return the time from the Bancom card if not available use other provider
 *
 * DESCRIPTION:
 * Call bc635_read() to get the time from the bancom card and the status bits.
 * (bit 0) the time is not locked to the synchronisation source 
 * (bit 1) time offset > 2 or 5 microsecs
 * (bit 2) Frequency offset is too large
 *
 * The output array follow the same conventions of the RPC routines.
 * 0: CARD_STAT     : 0 if card was found; 1 otherwise
 * 1: CARD_TIMESTAT : 0 if time read successfuly; 1 otherwise.
 * 2: CARD_TIME     : time value, this is the EPICS time
 * 3: CARD_REGS     : time card registers (three bits).
 * 4: RT_CLOCK      : real time clock value
 * 5: MONO_CLOCK    : monotonic clock value
 * 6: MONO_DIFF     : difference between 2 reads to check if there has been context
 *                    switching, we don't know how to get exclusive access on RTEMS
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)  
 * (>) pgsub  (struct genSubRecord *)    Pointer to gensub record structure
 *
 * EPICS INPUT FIELDS:
 * (int) A          debug mode (0=no debug, 1=debug)
 * (int) VALB       bancom card present (1=present, 0=not present)
 *
 * EPICS OUTPUT FIELDS:
 * (int) VALA       bancom card time and status (array)
 *
 * FUNCTION VALUE:
 * Always zero
 * 
 * DEFICIENCIES:
 * None.
 *-
 */
long getTime (struct genSubRecord *pgsub)
{
    double  rawt;
    int     status;

    /* Set default values for the output array.
     */
    CARD_CARDSTAT = (double) (!CARD_FOUND); /* card found */
    CARD_TIMESTAT = (double) 1;             /* failed; no time yet */
    CARD_TIME = (double) 0;                 /* no time yet */
    CARD_REGS = (double) 0;                 /* no register info yet */
    RT_CLOCK = (double) 0;                  /* no time yet */
    MONO_CLOCK = (double) 0;                /* no time yet */
    MONO_DIFF = (double) 0;                 /* no time yet */

    /* Read the time from the card if the hardware was found.
     * bc635_read will return return the Bancom card status bits
     * if it succeeds in converting the time or -1 otherwise.
     */
    if (CARD_FOUND && CARD_ENABLE) {
        status = bc635_read (&rawt);
        if (status != -1) {
            CARD_TIMESTAT = 0;  /* time ok */
            CARD_REGS = (double) status;
            CARD_TIME = rawt;
            if (DEBUG_FLAG)
                printf("getTime, ok rawt=%lg, regs=%d\n", rawt, status);
        } else {
            CARD_TIMESTAT = 1;  /* cannot convert time */
            if (DEBUG_FLAG)
                printf("getTime, failed to read time\n");
        }
    } else {
        /* We can still obtain the time if there's no card or if it's not enabled */
        timeNow(&rawt);  
        CARD_TIMESTAT = 0;  /* time ok */
        CARD_TIME = rawt;
        if (DEBUG_FLAG)
            printf("getTime, Bancom card not present, rawt=%lg\n", rawt);
    }

    struct timespec nowRT, nowMono1, nowMono2;
    clock_gettime(CLOCK_MONOTONIC, &nowMono1);
    /* If a Hi-Res clock is available and works, use it */
    #ifdef CLOCK_REALTIME_HR
        clock_gettime(CLOCK_REALTIME_HR, &nowRT) &&
        /* Note: Uses the lo-res clock below if the above call fails */
    #endif
    clock_gettime(CLOCK_REALTIME, &nowRT); // We cannot assure that a preemption is not
    // happening before this clock is read, as we are below ms precision, getting a 2nd
    // read of the MONOTONIC clock will reveal if there have been some internal delays.
    // We are forwarding the difference between the 2 MONO reads, that way we will know.
    clock_gettime(CLOCK_MONOTONIC, &nowMono2);

    //RT_CLOCK = ((double)nowRT.tv_sec) + ((double)nowRT.tv_nsec)/1e9;
    RT_CLOCK = getDoubleTime(&nowRT);
    MONO_CLOCK = getDoubleTime(&nowMono1);
    MONO_DIFF = getDoubleTime(&nowMono2)-MONO_CLOCK;

    return 0;
}

epicsRegisterFunction(initTime);
epicsRegisterFunction(getTime);
