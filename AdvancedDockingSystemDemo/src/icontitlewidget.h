#ifndef ICONTITLEWIDGET_H
#define ICONTITLEWIDGET_H

#include <QFrame>
class QIcon;
class QString;

class IconTitleWidget : public QFrame
{
	Q_OBJECT
public:
	explicit IconTitleWidget(const QIcon& icon, const QString& title, QWidget *parent = 0);

signals:

public slots:
};

#endif // ICONTITLEWIDGET_H
