/****************************************************************************
** Panel meta object code from reading C++ file 'panel.h'
**
** Created: Wed Dec 1 12:03:10 2004
**      by: The Qt MOC ($Id: moc_panel.cc,v 1.2 2004/12/10 15:06:06 jayache Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "panel.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Panel::className() const
{
    return "Panel";
}

QMetaObject *Panel::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Panel( "Panel", &Panel::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Panel::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Panel", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Panel::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Panel", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Panel::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"load", 0, 0 };
    static const QUMethod slot_1 = {"loadlist", 0, 0 };
    static const QUMethod slot_2 = {"option", 0, 0 };
    static const QUMethod slot_3 = {"restart", 0, 0 };
    static const QUMethod slot_4 = {"quit", 0, 0 };
    static const QUMethod slot_5 = {"stop", 0, 0 };
    static const QUMethod slot_6 = {"play", 0, 0 };
    static const QUMethod slot_7 = {"pause", 0, 0 };
    static const QUMethod slot_8 = {"previous", 0, 0 };
    static const QUMethod slot_9 = {"next", 0, 0 };
    static const QUParameter param_slot_10[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_10 = {"Volumechanged", 1, param_slot_10 };
    static const QUParameter param_slot_11[] = {
	{ "frame", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_11 = {"Framechanged", 1, param_slot_11 };
    static const QMetaData slot_tbl[] = {
	{ "load()", &slot_0, QMetaData::Public },
	{ "loadlist()", &slot_1, QMetaData::Public },
	{ "option()", &slot_2, QMetaData::Public },
	{ "restart()", &slot_3, QMetaData::Public },
	{ "quit()", &slot_4, QMetaData::Public },
	{ "stop()", &slot_5, QMetaData::Public },
	{ "play()", &slot_6, QMetaData::Public },
	{ "pause()", &slot_7, QMetaData::Public },
	{ "previous()", &slot_8, QMetaData::Public },
	{ "next()", &slot_9, QMetaData::Public },
	{ "Volumechanged(int)", &slot_10, QMetaData::Public },
	{ "Framechanged(int)", &slot_11, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"Panel", parentObject,
	slot_tbl, 12,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Panel.setMetaObject( metaObj );
    return metaObj;
}

void* Panel::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Panel" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool Panel::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: load(); break;
    case 1: loadlist(); break;
    case 2: option(); break;
    case 3: restart(); break;
    case 4: quit(); break;
    case 5: stop(); break;
    case 6: play(); break;
    case 7: pause(); break;
    case 8: previous(); break;
    case 9: next(); break;
    case 10: Volumechanged((int)static_QUType_int.get(_o+1)); break;
    case 11: Framechanged((int)static_QUType_int.get(_o+1)); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Panel::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Panel::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool Panel::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
