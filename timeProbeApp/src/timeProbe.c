#include <stdio.h>
#include <epicsExport.h>
#include <registryFunction.h>

//#include <bc635.h>
#include "timeLib.h"
#include "bc635Aliases.h"
#include <aSubRecord.h>

/* Debug mode. Set this value to 1 to enable debug messages.
 */
#define DEBUG_FLAG	*((long *) (pgsub->a))

/* Bancom time card present. This value is set (once) in initTime
 * when the record is initialized.
 */
#define CARD_FOUND	*((long *) (pgsub->valb))

/* These values are set in the getTime every time the record runs.
 * The output is an array of four bytes to allow the Linux timeProbe to
 * read these values in a single get operation.
 */
#define CARD_CARDSTAT	*((double *) (pgsub->vala) + 0)
#define CARD_TIMESTAT	*((double *) (pgsub->vala) + 1)
#define CARD_TIME	*((double *) (pgsub->vala) + 2)
#define CARD_REGS	*((double *) (pgsub->vala) + 3)

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
 * (>) pgsub  (struct aSubRecord *)    Pointer to gensub record structure
 *
 * EPICS OUTPUT FIELDS:
 * (int) VALB	bancom card present (1=present, 0=not present).
 *
 * FUNCTION VALUE:
 * Always zero
 * 
 * DEFICIENCIES:
 * None.
 *-
 */
long initTime (struct aSubRecord *pgsub)
{
    if (bcTestCard() == 0) { /* card found */
	 printf ("initTime:Bancom time card found\n"); 
	CARD_FOUND = 1;
    } else
	CARD_FOUND = 0;

    return 0;
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
 * 2: CARD_TIME     : time value
 * 3: CARD_REGS     : time card registers (three bits).
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)  
 * (>) pgsub  (struct aSubRecord *)    Pointer to gensub record structure
 *
 * EPICS INPUT FIELDS:
 * (int) A	debug mode (0=no debug, 1=debug)
 * (int) VALB	bancom card present (1=present, 0=not present)
 *
 * EPICS OUTPUT FIELDS:
 * (int) VALA	bancom card time and status (array)
 *
 * FUNCTION VALUE:
 * Always zero
 * 
 * DEFICIENCIES:
 * None.
 *-
 */
long getTime (struct aSubRecord *pgsub)
{
    double	rawt;
    int		status;

    /* Set default values for the output array.
     */
    CARD_CARDSTAT = (double) (!CARD_FOUND);	/* card found */
    CARD_TIMESTAT = (double) 1;			/* failed; no time yet */
    CARD_TIME = (double) 0;			/* no time yet */
    CARD_REGS = (double) 0;			/* no register info yet */

    /* Read the time from the card if the hardware was found.
     * bc635_read will return return the Bancom card status bits
     * if it succeeds in converting the time or -1 otherwise.
     */
    if (CARD_FOUND) {
	status = bc635_read (&rawt);
	if (status != -1) {
	    CARD_TIMESTAT = 0;	/* time ok */
	    CARD_REGS = (double) status;
	    CARD_TIME = rawt;
	    if (DEBUG_FLAG)
		printf("getTime, ok rawt=%f, regs=%d\n", rawt, status);
	} else {
	    CARD_TIMESTAT = 1;  /* cannot convert time */
	    if (DEBUG_FLAG)
		printf("getTime, failed to read time\n");
	}
    } else {
	if (DEBUG_FLAG)
	    printf("getTime, Bancom card not present\n");
	/* if there is no card we can still obtain the time from other providers */
	timeNow(&rawt);  
	CARD_TIMESTAT = 0;  /* time ok */
	CARD_TIME = rawt;
    }

    return 0;
}

epicsRegisterFunction(initTime);
epicsRegisterFunction(getTime);
