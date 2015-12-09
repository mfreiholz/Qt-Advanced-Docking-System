#include "container_widget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QSplitter>

ADS_NAMESPACE_BEGIN

// Static Helper //////////////////////////////////////////////////////

static QSplitter* newSplitter(Qt::Orientation orientation = Qt::Horizontal)
{
	QSplitter* s = new QSplitter();
	s->setChildrenCollapsible(false);
	s->setOpaqueResize(false);
	s->setOrientation(orientation);
	return s;
}

//static void deleteEmptySplitter(ContainerWidget* container)
//{
//	auto splitters = container->findChildren<QSplitter*>();
//	for (auto i = 0; i < splitters.count(); ++i)
//	{
//		if (splitters[i]->count() == 0 && container->_splitter != splitters[i])
//		{
//			qDebug() << "Delete empty QSplitter";
//			splitters[i]->deleteLater();
//		}
//	}
//}

///////////////////////////////////////////////////////////////////////

ContainerWidget::ContainerWidget(QWidget *parent) :
	QFrame(parent),
	_mainLayout(NULL),
	_orientation(Qt::Horizontal),
	_splitter(NULL)
{
	_mainLayout = new QGridLayout();
	_mainLayout->setContentsMargins(9, 9, 9, 9);
	_mainLayout->setSpacing(9);
	setLayout(_mainLayout);
}

Qt::Orientation ContainerWidget::orientation() const
{
	return _orientation;
}

void ContainerWidget::setOrientation(Qt::Orientation orientation)
{
	if (_orientation != orientation)
	{
		_orientation = orientation;
		emit orientationChanged();
	}
}

void ContainerWidget::dropContent(const InternalContentData& data, SectionWidget* targetSection, DropArea area)
{
	if (!targetSection)
	{
		qWarning() << "Drop on invalid targetSection";
		return;
	}

	auto targetSectionSplitter = findParentSplitter(targetSection);

	// Drop logic based on area.
	switch (area)
	{
	case TopDropArea:
	{
		auto sw = new SectionWidget(this);
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Vertical)
		{
			auto index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index, sw);
		}
		else
		{
			auto index = targetSectionSplitter->indexOf(targetSection);
			auto s = newSplitter(Qt::Vertical);
			s->addWidget(sw);
			s->addWidget(targetSection);
			targetSectionSplitter->insertWidget(index, s);
		}
		break;
	}
	case RightDropArea:
	{
		auto sw = new SectionWidget(this);
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Horizontal)
		{
			auto index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index + 1, sw);
		}
		else
		{
			auto index = targetSectionSplitter->indexOf(targetSection);
			auto s = newSplitter(Qt::Horizontal);
			s->addWidget(targetSection);
			s->addWidget(sw);
			targetSectionSplitter->insertWidget(index, s);
		}
		break;
	}
	case BottomDropArea:
	{
		auto sw = new SectionWidget(this);
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Vertical)
		{
			auto index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index + 1, sw);
		}
		else
		{
			auto index = targetSectionSplitter->indexOf(targetSection);
			auto s = newSplitter(Qt::Vertical);
			s->addWidget(targetSection);
			s->addWidget(sw);
			targetSectionSplitter->insertWidget(index, s);
		}
		break;
	}
	case LeftDropArea:
	{
		auto sw = new SectionWidget(this);
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Horizontal)
		{
			auto index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index, sw);
		}
		else
		{
			auto s = newSplitter(Qt::Horizontal);
			s->addWidget(sw);
			auto index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index, s);
			s->addWidget(targetSection);
		}
		break;
	}
	case CenterDropArea:
	{
		targetSection->addContent(data, true);
		break;
	}
	}
}

void ContainerWidget::addSection(SectionWidget* section)
{
	// Create default splitter.
	if (!_splitter)
	{
		_splitter = new QSplitter(_orientation);
		_splitter->setChildrenCollapsible(false);
		_splitter->setOpaqueResize(false);
		_mainLayout->addWidget(_splitter, 0, 0);
	}
	if (_splitter->indexOf(section) != -1)
	{
		qWarning() << Q_FUNC_INFO << QString("Section has already been added");
		return;
	}
	_splitter->addWidget(section);
}

void ContainerWidget::splitSections(SectionWidget* s1, SectionWidget* s2, Qt::Orientation orientation)
{
	addSection(s1);
	addSection(s2);
	//_sections.append(s2);

	auto currentSplitter = findParentSplitter(s1);
	if (currentSplitter)
	{
		const int index = currentSplitter->indexOf(s1);
		auto splitter = new QSplitter(orientation, this);
		splitter->setChildrenCollapsible(false);
		splitter->setOpaqueResize(false);
		splitter->addWidget(s1);
		splitter->addWidget(s2);
		currentSplitter->insertWidget(index, splitter);
	}
//	_mainLayout->replaceWidget(section1, splitter);
//	splitter->addWidget(section1);
//	splitter->addWidget(section2);
}

SectionWidget* ContainerWidget::sectionAt(const QPoint& pos) const
{
	auto gpos = mapToGlobal(pos);
	for (int i = 0; i < _sections.size(); ++i)
	{
		auto sw = _sections[i];
		if (sw->rect().contains(sw->mapFromGlobal(gpos)))
		{
			return sw;
		}
	}
	return 0;
}

QRect ContainerWidget::outerTopDropRect() const
{
	auto r = rect();
	auto h = r.height() / 100 * 5;
	return QRect(r.left(), r.top(), r.width(), h);
}

QRect ContainerWidget::outerRightDropRect() const
{
	auto r = rect();
	auto w = r.width() / 100 * 5;
	return QRect(r.right() - w, r.top(), w, r.height());
}

QRect ContainerWidget::outerBottomDropRect() const
{
	auto r = rect();
	auto h = r.height() / 100 * 5;
	return QRect(r.left(), r.bottom() - h, r.width(), h);
}

QRect ContainerWidget::outerLeftDropRect() const
{
	auto r = rect();
	auto w = r.width() / 100 * 5;
	return QRect(r.left(), r.top(), w, r.height());
}

void ContainerWidget::paintEvent(QPaintEvent* e)
{
	QFrame::paintEvent(e);

//	QPainter p(this);
//	p.fillRect(outerTopDropRect(), QBrush(QColor(Qt::red)));
//	p.fillRect(outerRightDropRect(), QBrush(QColor(Qt::green)));
//	p.fillRect(outerBottomDropRect(), QBrush(QColor(Qt::blue)));
//	p.fillRect(outerLeftDropRect(), QBrush(QColor(Qt::yellow)));
}

void ContainerWidget::contextMenuEvent(QContextMenuEvent*)
{
	qDebug() << Q_FUNC_INFO;
//	if (_contents.size() <= 0)
//	{
//		QFrame::contextMenuEvent(e);
//		return;
//	}

//	// Menu with all available contents.
//	QMenu m;
//	for (int i = 0; i < _contents.size(); ++i)
//	{
//		auto c = _contents[i];
//		m.addAction(QIcon(), QString("Content %1").arg(c->uid()));
//	}
//	m.exec();
}

ADS_NAMESPACE_END