#include <stdio.h>
#include <epicsExport.h>
#include <registryFunction.h>

#include <bc635.h>
#include <genSubRecord.h>

#define DEBUG_MODE	*((int *) (pgsub->a))

#define CARD_FOUND	*((int *) (pgsub->valb))

#define CARD_CARDSTAT	*((double *) (pgsub->vala) + 0)
#define CARD_TIMESTAT	*((double *) (pgsub->vala) + 1)
#define CARD_TIME	*((double *) (pgsub->vala) + 2)
#define CARD_REGS	*((double *) (pgsub->vala) + 3)

/********************************************************************
 *+
 * FUNCTION NAME:
 * initBancomTime
 *
 * INVOCATION:
 * status = initBancomTime();
 *
 * PURPOSE:
 * Checks whether the time card is present
 *
 * DESCRIPTION:
 * Call bcTestCard() to determine whether the Bancom card is available.
 * Use VALB to save this status for future invocations of the record.
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)  
 * (>) pgsub  (struct genSubRecord *)    Pointer to gensub record structure
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
long initBancomTime (struct genSubRecord *pgsub)
{
    if (bcTestCard() == 0) { /* card found */
	printf ("initBancomTime: time card found\n");
	CARD_FOUND = 1;
    } else
	CARD_FOUND = 0;

    return 0;
}

/********************************************************************
 *+
 * FUNCTION NAME:
 * getBancomTime
 *
 * INVOCATION:
 * status = getBancomTime();
 *
 * PURPOSE:
 * Return the time from the Bancom card
 *
 * DESCRIPTION:
 * Call bc635_read() to get the time from the bancom card and the status bits.
 * (bit 0) the time is not locked to the synchronisation source 
 * (bit 1) time offset > 2 or 5 microsecs
 * (bit 2) Frequency offset is too large
 *
 * The output array follow the same conventions of the RPC routines.
 * CAR
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)  
 * (>) pgsub  (struct genSubRecord *)    Pointer to gensub record structure
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
long getBancomTime (struct genSubRecord *pgsub)
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
	    CARD_TIMESTAT = 0;
	    CARD_REGS = (double) status;
	    CARD_TIME = rawt;	/* time ok */
	} else
	    CARD_TIMESTAT = 1;  /* cannot convert time */
    } else if (DEBUG_MODE) {
	    printf("getBancomTime, fail: found=%d\n", CARD_FOUND);
    }

    return 0;
}

epicsRegisterFunction(initBancomTime);
epicsRegisterFunction(getBancomTime);
