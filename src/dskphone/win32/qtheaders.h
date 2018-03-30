#include <vector>

#include <QtGui>
#include <QScrollBar>
#include <QtCore/qobject.h>
#include <QtGui/qbrush.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsettings.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/QtCore>
#include <qfile.h>
#include <qtextstream.h>
#include <QMouseEvent>
#include <QFile>
#include <QtGui/QApplication>
#include <qapplication.h>
#include <qsocketnotifier.h>
#include <QObject>
#include <QKeyEvent>
#include <QDialog>
#include <QPixmap>
#include <QtGui/QLabel>
#include <QTimer>
#include <QStylePainter>
#include <QStyleOption>
#include <QFrame>
#include <QtCore/qnamespace.h>

#ifdef _MSC_VER
#include "precompile.h"

#include <qlistwidget>
#include <qpainter>
#include <QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QRect>
#include <QtCore/QPair>
#include <QtGui/QComboBox>
#include <QFontMetrics>
#include <QRadioButton>
#include <QColor>
#include <QVariant>
#include <QtGui/QAction>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>

/*#include <QRegExp>
#include <QItemDelegate>
#include <QtCore/QSharedPointer>
#include <QtCore/QWeakPointer>
#include <QDateTime>
#include <QAbstractItemView>
#include <QStyleOptionSlider>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QResizeEvent>
#include <QIODevice>
#include <QByteArray>
#include <QImageReader>
#include <QBuffer>*/
#else
#include <QtGui/qmouse_qws.h>
#include <qwindowsystem_qws.h>
#include <QWSServer>
#include <qscreen_qws.h>

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#include <ETLLib.hpp>
#endif


