/****************************************************************************
** FileSelect meta object code from reading C++ file 'fileselect.h'
**
** Created: Wed Dec 1 12:03:08 2004
**      by: The Qt MOC ($Id: moc_file.cc,v 1.2 2004/12/10 15:06:06 jayache Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "fileselect.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *FileSelect::className() const
{
    return "FileSelect";
}

QMetaObject *FileSelect::metaObj = 0;
static QMetaObjectCleanUp cleanUp_FileSelect( "FileSelect", &FileSelect::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString FileSelect::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FileSelect", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString FileSelect::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FileSelect", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* FileSelect::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"SelectOk", 0, 0 };
    static const QUMethod slot_1 = {"SelectAll", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ "index", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"dirSelected", 1, param_slot_2 };
    static const QUMethod slot_3 = {"MSelectclicked", 0, 0 };
    static const QUMethod slot_4 = {"filterentered", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "SelectOk()", &slot_0, QMetaData::Public },
	{ "SelectAll()", &slot_1, QMetaData::Public },
	{ "dirSelected(int)", &slot_2, QMetaData::Public },
	{ "MSelectclicked()", &slot_3, QMetaData::Public },
	{ "filterentered()", &slot_4, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"FileSelect", parentObject,
	slot_tbl, 5,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_FileSelect.setMetaObject( metaObj );
    return metaObj;
}

void* FileSelect::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "FileSelect" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool FileSelect::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: SelectOk(); break;
    case 1: SelectAll(); break;
    case 2: dirSelected((int)static_QUType_int.get(_o+1)); break;
    case 3: MSelectclicked(); break;
    case 4: filterentered(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool FileSelect::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool FileSelect::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool FileSelect::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
