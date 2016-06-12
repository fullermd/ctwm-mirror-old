/*
 *  [ ctwm ]
 *
 *  Copyright 2004 Richard Levitte
 *
 * Permission to use, copy, modify  and distribute this software  [ctwm] and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above  copyright notice appear  in all copies and that both that
 * copyright notice and this permission notice appear in supporting documen-
 * tation, and that the name of  Claude Lecommandeur not be used in adverti-
 * sing or  publicity  pertaining to  distribution of  the software  without
 * specific, written prior permission. Claude Lecommandeur make no represen-
 * tations  about the suitability  of this software  for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 * Claude Lecommandeur DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL  IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS.  IN NO
 * EVENT SHALL  Claude Lecommandeur  BE LIABLE FOR ANY SPECIAL,  INDIRECT OR
 * CONSEQUENTIAL  DAMAGES OR ANY  DAMAGES WHATSOEVER  RESULTING FROM LOSS OF
 * USE, DATA  OR PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR
 * OTHER  TORTIOUS ACTION,  ARISING OUT OF OR IN  CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Richard Levitte [ richard@levitte.org ][ June 2004 ]
 */

#ifndef _CTWM_SESSION_H
#define _CTWM_SESSION_H

#include <X11/SM/SMlib.h>

/* XXX Only used in one place, should convert to a func? */
extern SmcConn smcConn;

char *GetClientID(Window window);
char *GetWindowRole(Window window);
int WriteWinConfigEntry(FILE *configFile, TwmWindow *theWindow,
                        char *clientId, char *windowRole);
int ReadWinConfigEntry(FILE *configFile, unsigned short version,
                       TWMWinConfigEntry **pentry);
void ReadWinConfigFile(char *filename);
int GetWindowConfig(TwmWindow *theWindow,
                    short *x, short *y,
                    unsigned short *width, unsigned short *height,
                    bool *iconified,
                    bool *icon_info_present,
                    short *icon_x, short *icon_y,
                    bool *width_ever_changed_by_user,
                    bool *height_ever_changed_by_user,
                    int *occupation /* <== [ Matthew McNeill Feb 1997 ] == */
                   );
void SaveYourselfPhase2CB(SmcConn smcCon, SmPointer clientData);
void DieCB(SmcConn smcCon, SmPointer clientData);
void SaveCompleteCB(SmcConn smcCon, SmPointer clientData);
void ShutdownCancelledCB(SmcConn smcCon, SmPointer clientData);
void ProcessIceMsgProc(XtPointer client_data, int *source, XtInputId *id);
void ConnectToSessionManager(char *previous_id);

#endif /* _CTWM_SESSION_H */

