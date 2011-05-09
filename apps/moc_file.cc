/****************************************************************************
** FileSelect meta object code from reading C++ file 'fileselect.h'
**
** Created: Tue Feb 20 18:35:14 2001
**      by: The Qt MOC ($Id: moc_file.cc,v 1.2 2001/02/20 19:04:07 micce Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "fileselect.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *FileSelect::className() const
{
    return "FileSelect";
}

QMetaObject *FileSelect::metaObj = 0;

void FileSelect::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("FileSelect","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString FileSelect::tr(const char* s)
{
    return qApp->translate( "FileSelect", s, 0 );
}

QString FileSelect::tr(const char* s, const char * c)
{
    return qApp->translate( "FileSelect", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* FileSelect::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (FileSelect::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (FileSelect::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (FileSelect::*m1_t2)(int);
    typedef void (QObject::*om1_t2)(int);
    typedef void (FileSelect::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    typedef void (FileSelect::*m1_t4)();
    typedef void (QObject::*om1_t4)();
    m1_t0 v1_0 = &FileSelect::SelectOk;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &FileSelect::SelectAll;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &FileSelect::dirSelected;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &FileSelect::MSelectclicked;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    m1_t4 v1_4 = &FileSelect::filterentered;
    om1_t4 ov1_4 = (om1_t4)v1_4;
    QMetaData *slot_tbl = QMetaObject::new_metadata(5);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(5);
    slot_tbl[0].name = "SelectOk()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "SelectAll()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "dirSelected(int)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "MSelectclicked()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Public;
    slot_tbl[4].name = "filterentered()";
    slot_tbl[4].ptr = (QMember)ov1_4;
    slot_tbl_access[4] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"FileSelect", "QDialog",
	slot_tbl, 5,
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
