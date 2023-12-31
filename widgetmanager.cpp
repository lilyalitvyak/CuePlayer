#include "widgetmanager.h"

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define MESSAGE_SOURCE_OLD            0
#define MESSAGE_SOURCE_APPLICATION    1
#define MESSAGE_SOURCE_PAGER          2
#endif //Q_WS_X11

WidgetManager::WidgetManager(QWidget *parent) :
    QWidget(parent)
{
}

void WidgetManager::raiseWidget(QWidget *AWidget)
{
#ifdef Q_WS_X11
  static Atom         NET_ACTIVE_WINDOW = 0;
  XClientMessageEvent xev;

  if(NET_ACTIVE_WINDOW == 0)
  {
	Display *dpy      = QX11Info::display();
	NET_ACTIVE_WINDOW = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
  }

  xev.type         = ClientMessage;
  xev.window       = AWidget->winId();
  xev.message_type = NET_ACTIVE_WINDOW;
  xev.format       = 32;
  xev.data.l[0]    = MESSAGE_SOURCE_PAGER;
  xev.data.l[1]    = QX11Info::appUserTime();
  xev.data.l[2]    = xev.data.l[3] = xev.data.l[4] = 0;

  XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False, SubstructureNotifyMask | SubstructureRedirectMask, (XEvent*)&xev);
#endif//Q_WS_X11
  AWidget->raise();
}
