/****************************************************************************
** OptionPanel meta object code from reading C++ file 'optionpanel.h'
**
** Created: Tue Feb 20 18:35:27 2001
**      by: The Qt MOC ($Id: moc_optionp.cc,v 1.2 2001/02/20 19:04:07 micce Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "optionpanel.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *OptionPanel::className() const
{
    return "OptionPanel";
}

QMetaObject *OptionPanel::metaObj = 0;

void OptionPanel::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("OptionPanel","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString OptionPanel::tr(const char* s)
{
    return qApp->translate( "OptionPanel", s, 0 );
}

QString OptionPanel::tr(const char* s, const char * c)
{
    return qApp->translate( "OptionPanel", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* OptionPanel::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (OptionPanel::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (OptionPanel::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (OptionPanel::*m1_t2)(int);
    typedef void (QObject::*om1_t2)(int);
    m1_t0 v1_0 = &OptionPanel::SelectOk;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &OptionPanel::SelectCancel;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &OptionPanel::setthread;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "SelectOk()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "SelectCancel()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "setthread(int)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"OptionPanel", "QDialog",
	slot_tbl, 3,
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
