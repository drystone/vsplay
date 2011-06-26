/****************************************************************************
** MSelect meta object code from reading C++ file 'multipleselect.h'
**
** Created: Wed Dec 1 12:03:07 2004
**      by: The Qt MOC ($Id: moc_multi.cc,v 1.2 2004/12/10 15:06:06 jayache Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "multipleselect.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MSelect::className() const
{
    return "MSelect";
}

QMetaObject *MSelect::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MSelect( "MSelect", &MSelect::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MSelect::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MSelect", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MSelect::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MSelect", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MSelect::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGridView::staticMetaObject();
    static const QUMethod signal_0 = {"clicked", 0, 0 };
    static const QUMethod signal_1 = {"doubleclicked", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "clicked()", &signal_0, QMetaData::Public },
	{ "doubleclicked()", &signal_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"MSelect", parentObject,
	0, 0,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MSelect.setMetaObject( metaObj );
    return metaObj;
}

void* MSelect::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MSelect" ) )
	return this;
    return QGridView::qt_cast( clname );
}

// SIGNAL clicked
void MSelect::clicked()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

// SIGNAL doubleclicked
void MSelect::doubleclicked()
{
    activate_signal( staticMetaObject()->signalOffset() + 1 );
}

bool MSelect::qt_invoke( int _id, QUObject* _o )
{
    return QGridView::qt_invoke(_id,_o);
}

bool MSelect::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: clicked(); break;
    case 1: doubleclicked(); break;
    default:
	return QGridView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool MSelect::qt_property( int id, int f, QVariant* v)
{
    return QGridView::qt_property( id, f, v);
}

bool MSelect::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
