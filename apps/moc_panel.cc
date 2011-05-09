/****************************************************************************
** Panel meta object code from reading C++ file 'panel.h'
**
** Created: Tue Feb 20 18:35:21 2001
**      by: The Qt MOC ($Id: moc_panel.cc,v 1.2 2001/02/20 19:04:07 micce Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "panel.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *Panel::className() const
{
    return "Panel";
}

QMetaObject *Panel::metaObj = 0;

void Panel::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("Panel","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString Panel::tr(const char* s)
{
    return qApp->translate( "Panel", s, 0 );
}

QString Panel::tr(const char* s, const char * c)
{
    return qApp->translate( "Panel", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* Panel::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (Panel::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (Panel::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (Panel::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    typedef void (Panel::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    typedef void (Panel::*m1_t4)();
    typedef void (QObject::*om1_t4)();
    typedef void (Panel::*m1_t5)();
    typedef void (QObject::*om1_t5)();
    typedef void (Panel::*m1_t6)();
    typedef void (QObject::*om1_t6)();
    typedef void (Panel::*m1_t7)();
    typedef void (QObject::*om1_t7)();
    typedef void (Panel::*m1_t8)();
    typedef void (QObject::*om1_t8)();
    typedef void (Panel::*m1_t9)();
    typedef void (QObject::*om1_t9)();
    typedef void (Panel::*m1_t10)(int);
    typedef void (QObject::*om1_t10)(int);
    typedef void (Panel::*m1_t11)(int);
    typedef void (QObject::*om1_t11)(int);
    m1_t0 v1_0 = &Panel::load;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &Panel::loadlist;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &Panel::option;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &Panel::restart;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    m1_t4 v1_4 = &Panel::quit;
    om1_t4 ov1_4 = (om1_t4)v1_4;
    m1_t5 v1_5 = &Panel::stop;
    om1_t5 ov1_5 = (om1_t5)v1_5;
    m1_t6 v1_6 = &Panel::play;
    om1_t6 ov1_6 = (om1_t6)v1_6;
    m1_t7 v1_7 = &Panel::pause;
    om1_t7 ov1_7 = (om1_t7)v1_7;
    m1_t8 v1_8 = &Panel::previous;
    om1_t8 ov1_8 = (om1_t8)v1_8;
    m1_t9 v1_9 = &Panel::next;
    om1_t9 ov1_9 = (om1_t9)v1_9;
    m1_t10 v1_10 = &Panel::Volumechanged;
    om1_t10 ov1_10 = (om1_t10)v1_10;
    m1_t11 v1_11 = &Panel::Framechanged;
    om1_t11 ov1_11 = (om1_t11)v1_11;
    QMetaData *slot_tbl = QMetaObject::new_metadata(12);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(12);
    slot_tbl[0].name = "load()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "loadlist()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "option()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "restart()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Public;
    slot_tbl[4].name = "quit()";
    slot_tbl[4].ptr = (QMember)ov1_4;
    slot_tbl_access[4] = QMetaData::Public;
    slot_tbl[5].name = "stop()";
    slot_tbl[5].ptr = (QMember)ov1_5;
    slot_tbl_access[5] = QMetaData::Public;
    slot_tbl[6].name = "play()";
    slot_tbl[6].ptr = (QMember)ov1_6;
    slot_tbl_access[6] = QMetaData::Public;
    slot_tbl[7].name = "pause()";
    slot_tbl[7].ptr = (QMember)ov1_7;
    slot_tbl_access[7] = QMetaData::Public;
    slot_tbl[8].name = "previous()";
    slot_tbl[8].ptr = (QMember)ov1_8;
    slot_tbl_access[8] = QMetaData::Public;
    slot_tbl[9].name = "next()";
    slot_tbl[9].ptr = (QMember)ov1_9;
    slot_tbl_access[9] = QMetaData::Public;
    slot_tbl[10].name = "Volumechanged(int)";
    slot_tbl[10].ptr = (QMember)ov1_10;
    slot_tbl_access[10] = QMetaData::Public;
    slot_tbl[11].name = "Framechanged(int)";
    slot_tbl[11].ptr = (QMember)ov1_11;
    slot_tbl_access[11] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"Panel", "QWidget",
	slot_tbl, 12,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
