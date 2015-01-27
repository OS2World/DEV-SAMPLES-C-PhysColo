/* PHYSCOLO.C -- Program to display the physical screen colors */
/* Version 2.0 */

/* This program is Copyright (c) 1992, 1993 by Raja Thiagarajan and
   Peter Nielsen. However, you may freely use it for any non-commercial
   purpose. You can contact us at sthiagar@nickel.ucs.indiana.edu
   or pnielsen@finabo.abo.fi respectively.*/

/* Many thanks to Peter for significantly speeding this program up for
   OS/2 2.1. My apologies to him for destroying his pretty indentation
   style and his variable-naming conventions.*/

#define INCL_GPI
#define INCL_WIN
#include <os2.h>

#define MAX_PAL_SIZE 256

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);

static HAB hab;

#define clientClass "PhysColor"

VOID main (VOID)
{
   HMQ   hmq;
   QMSG  qmsg;
   HWND  hwndFrame, hwndClient;

   ULONG createFlags =  FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER
                        | FCF_MINMAX | FCF_SHELLPOSITION |  FCF_TASKLIST;

   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);

   WinRegisterClass (hab, clientClass, (PFNWP)ClientWndProc,
                     CS_SIZEREDRAW, 0);

   hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE, &createFlags,
                                   clientClass, "", 0, 0, 0,
                                   &hwndClient);


   while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
      WinDispatchMsg (hab, &qmsg);

   WinDestroyWindow (hwndFrame);
   WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
}

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static ULONG cWidth, cHeight;
   static LONG 	colorsToShow, colorsAvail;
   static HPAL	hpal = NULLHANDLE;
   register	i;

   switch (msg) {
      case WM_CREATE:
         {
         HDC   hdc;
         LONG  addlGraphics;
         PSZ   title;

         hdc = WinOpenWindowDC (hwnd);

         DevQueryCaps (hdc, CAPS_COLORS, 1L, &colorsAvail);
         DevQueryCaps (hdc, CAPS_ADDITIONAL_GRAPHICS, 1,
                       &addlGraphics);


         if (addlGraphics & CAPS_PALETTE_MANAGER) {
            ULONG table [MAX_PAL_SIZE];
            colorsToShow = colorsAvail;
            if (colorsToShow > MAX_PAL_SIZE)
               colorsToShow = MAX_PAL_SIZE;

            for (i = 0; i < colorsToShow; i++)
               table [i] = (PC_EXPLICIT << 24) + i;

            hpal = GpiCreatePalette (hab, 0L, LCOLF_CONSECRGB,
                                     colorsToShow, table);

            title = "Physical Color Viewer";
         } else
            title = "Color Viewer";

         WinSetWindowText (WinQueryWindow (hwnd, QW_PARENT), title);
         }
         break;
      case WM_SIZE:
         cWidth  = (ULONG)SHORT1FROMMP (mp2);
         cHeight = (ULONG)SHORT2FROMMP (mp2);
         break;
      case WM_ERASEBACKGROUND:
         return (MRESULT)TRUE;
      case WM_PAINT:
         {
         POINTL   ptlA, ptlB;
	 ULONG    temp = colorsToShow;
         HPS 	  hps;

         hps = WinBeginPaint (hwnd, NULLHANDLE, NULL);

         GpiSelectPalette (hps, hpal);
         WinRealizePalette (hwnd, hps, &temp);

         ptlA.y = 0;
         ptlB.y = cHeight;
         i = 0;
         while (i < cWidth) {
            ptlA.x = ptlB.x = i;
            GpiSetColor (hps, (++i * colorsToShow) / cWidth);
            GpiMove (hps, &ptlA);
            GpiLine (hps, &ptlB);
         }

         WinEndPaint (hps);
         }
         break;
      case WM_DESTROY:
         if (hpal)
            GpiDeletePalette (hpal);
         break;
      default:
         return (WinDefWindowProc (hwnd, msg, mp1, mp2));
   }
   return ((MRESULT)FALSE);
}
