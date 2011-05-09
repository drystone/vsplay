/****************************************************************************
** MSelect meta object code from reading C++ file 'multipleselect.h'
**
** Created: Tue Feb 20 18:35:09 2001
**      by: The Qt MOC ($Id: moc_multi.cc,v 1.2 2001/02/20 19:04:07 micce Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "multipleselect.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *MSelect::className() const
{
    return "MSelect";
}

QMetaObject *MSelect::metaObj = 0;

void MSelect::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QTableView::className(), "QTableView") != 0 )
	badSuperclassWarning("MSelect","QTableView");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString MSelect::tr(const char* s)
{
    return qApp->translate( "MSelect", s, 0 );
}

QString MSelect::tr(const char* s, const char * c)
{
    return qApp->translate( "MSelect", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* MSelect::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QTableView::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    typedef void (MSelect::*m2_t0)();
    typedef void (QObject::*om2_t0)();
    typedef void (MSelect::*m2_t1)();
    typedef void (QObject::*om2_t1)();
    m2_t0 v2_0 = &MSelect::clicked;
    om2_t0 ov2_0 = (om2_t0)v2_0;
    m2_t1 v2_1 = &MSelect::doubleclicked;
    om2_t1 ov2_1 = (om2_t1)v2_1;
    QMetaData *signal_tbl = QMetaObject::new_metadata(2);
    signal_tbl[0].name = "clicked()";
    signal_tbl[0].ptr = (QMember)ov2_0;
    signal_tbl[1].name = "doubleclicked()";
    signal_tbl[1].ptr = (QMember)ov2_1;
    metaObj = QMetaObject::new_metaobject(
	"MSelect", "QTableView",
	0, 0,
	signal_tbl, 2,
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

// SIGNAL clicked
void MSelect::clicked()
{
    activate_signal( "clicked()" );
}

// SIGNAL doubleclicked
void MSelect::doubleclicked()
{
    activate_signal( "doubleclicked()" );
}
