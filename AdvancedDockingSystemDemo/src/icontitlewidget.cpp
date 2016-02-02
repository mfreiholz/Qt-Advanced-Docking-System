#include "icontitlewidget.h"

#include <QIcon>
#include <QString>
#include <QBoxLayout>
#include <QLabel>
#include <QStyle>

IconTitleWidget::IconTitleWidget(const QIcon& icon, const QString& title, QWidget *parent) :
	QFrame(parent)
{
	auto l = new QBoxLayout(QBoxLayout::LeftToRight);
	l->setContentsMargins(0, 0, 0, 0);
	setLayout(l);

	// Icon label
	if (icon.isNull())
	{
//		auto titleIcon = new QLabel();
//		titleIcon->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(16, 16));
//		l->addWidget(titleIcon);
	}
	else
	{
		auto titleIcon = new QLabel();
		titleIcon->setPixmap(icon.pixmap(16, 16));
		l->addWidget(titleIcon);
	}

	// Title label
	auto titleText = new QLabel(title);
	auto titleFont = titleText->font();
	titleFont.setBold(true);
	titleText->setFont(titleFont);
	l->addWidget(titleText, 1);
}