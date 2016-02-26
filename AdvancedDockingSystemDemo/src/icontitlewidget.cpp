#include "icontitlewidget.h"

#include <QIcon>
#include <QString>
#include <QBoxLayout>
#include <QLabel>
#include <QStyle>

IconTitleWidget::IconTitleWidget(const QIcon& icon, const QString& title, QWidget *parent) :
	QFrame(parent)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::LeftToRight);
	l->setContentsMargins(0, 0, 0, 0);
	setLayout(l);

	_iconLabel = new QLabel();
	if (!icon.isNull())
		_iconLabel->setPixmap(icon.pixmap(16, 16));
	l->addWidget(_iconLabel);

	_titleLabel = new QLabel();
	_titleLabel->setText(title);
	l->addWidget(_titleLabel, 1);
}

void IconTitleWidget::polishUpdate()
{
	QList<QWidget*> widgets;
	widgets.append(_iconLabel);
	widgets.append(_titleLabel);
	foreach (QWidget* w, widgets)
	{
		w->style()->unpolish(w);
		w->style()->polish(w);
		w->update();
	}
}
