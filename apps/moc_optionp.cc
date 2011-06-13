/****************************************************************************
** OptionPanel meta object code from reading C++ file 'optionpanel.h'
**
** Created: Wed Dec 1 12:03:12 2004
**      by: The Qt MOC ($Id: moc_optionp.cc,v 1.2 2004/12/10 15:06:06 jayache Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "optionpanel.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *OptionPanel::className() const
{
    return "OptionPanel";
}

QMetaObject *OptionPanel::metaObj = 0;
static QMetaObjectCleanUp cleanUp_OptionPanel( "OptionPanel", &OptionPanel::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString OptionPanel::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "OptionPanel", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString OptionPanel::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "OptionPanel", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* OptionPanel::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"SelectOk", 0, 0 };
    static const QUMethod slot_1 = {"SelectCancel", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"setthread", 1, param_slot_2 };
    static const QMetaData slot_tbl[] = {
	{ "SelectOk()", &slot_0, QMetaData::Public },
	{ "SelectCancel()", &slot_1, QMetaData::Public },
	{ "setthread(int)", &slot_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"OptionPanel", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_OptionPanel.setMetaObject( metaObj );
    return metaObj;
}

void* OptionPanel::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "OptionPanel" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool OptionPanel::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: SelectOk(); break;
    case 1: SelectCancel(); break;
    case 2: setthread((int)static_QUType_int.get(_o+1)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool OptionPanel::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool OptionPanel::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool OptionPanel::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
