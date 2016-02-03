#include "ads/ContainerWidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QSplitter>

ADS_NAMESPACE_BEGIN

// Static Helper //////////////////////////////////////////////////////

static QSplitter* newSplitter(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0)
{
	QSplitter* s = new QSplitter(orientation, parent);
	s->setChildrenCollapsible(false);
	s->setOpaqueResize(false);
	return s;
}

static void dropContentOuterHelper(ContainerWidget* cw, QLayout* l, const InternalContentData& data, Qt::Orientation orientation, bool append)
{
	auto sw = new SectionWidget(cw);
	sw->addContent(data, true);

	QSplitter* oldsp = findImmediateSplitter(cw);
	if (oldsp->orientation() == orientation
		|| oldsp->count() == 1)
	{
		oldsp->setOrientation(orientation);
		if (append)
			oldsp->addWidget(sw);
		else
			oldsp->insertWidget(0, sw);
	}
	else
	{
		auto sp = newSplitter(orientation);
		if (append)
		{
			auto li = l->replaceWidget(oldsp, sp);
			sp->addWidget(oldsp);
			sp->addWidget(sw);
			delete li;
		}
		else
		{
			sp->addWidget(sw);
			auto li = l->replaceWidget(oldsp, sp);
			sp->addWidget(oldsp);
			delete li;
		}
	}
}

///////////////////////////////////////////////////////////////////////

ContainerWidget::ContainerWidget(QWidget *parent) :
	QFrame(parent),
	_mainLayout(NULL),
	_orientation(Qt::Horizontal),
	_splitter(NULL)
{
	_mainLayout = new QGridLayout();
	_mainLayout->setContentsMargins(9, 9, 9, 9);
	_mainLayout->setSpacing(0);
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
	// Drop on outer area
	if (!targetSection)
	{
		switch (area)
		{
		case TopDropArea:
			dropContentOuterHelper(this, _mainLayout, data, Qt::Vertical, false);
			break;
		case RightDropArea:
			dropContentOuterHelper(this, _mainLayout, data, Qt::Horizontal, true);
			break;
		case BottomDropArea:
			dropContentOuterHelper(this, _mainLayout, data, Qt::Vertical, true);
			break;
		case LeftDropArea:
			dropContentOuterHelper(this, _mainLayout, data, Qt::Horizontal, false);
			break;
		default:
			return;
		}
		return;
	}

	QSplitter* targetSectionSplitter = findParentSplitter(targetSection);

	// Drop logic based on area.
	switch (area)
	{
	case TopDropArea:
	{
		SectionWidget* sw = new SectionWidget(this);
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Vertical)
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index, sw);
		}
		else
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			QSplitter* s = newSplitter(Qt::Vertical);
			s->addWidget(sw);
			s->addWidget(targetSection);
			targetSectionSplitter->insertWidget(index, s);
		}
		break;
	}
	case RightDropArea:
	{
		SectionWidget* sw = new SectionWidget(this);
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Horizontal)
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index + 1, sw);
		}
		else
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			QSplitter* s = newSplitter(Qt::Horizontal);
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
	default:
		break;
	}
}

void ContainerWidget::addSection(SectionWidget* section)
{
	// Create default splitter.
	if (!_splitter)
	{
		_splitter = newSplitter(_orientation);
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

	QSplitter* currentSplitter = findParentSplitter(s1);
	if (currentSplitter)
	{
		const int index = currentSplitter->indexOf(s1);
		QSplitter* splitter = newSplitter(orientation, this);
		splitter->addWidget(s1);
		splitter->addWidget(s2);
		currentSplitter->insertWidget(index, splitter);
	}
}

SectionWidget* ContainerWidget::sectionAt(const QPoint& pos) const
{
	const QPoint gpos = mapToGlobal(pos);
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

QByteArray ContainerWidget::saveGeometry() const
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << (int) 0x00001337;
	out << _sections.size();
	return ba;
}

bool ContainerWidget::restoreGeometry(const QByteArray& data)
{
	QDataStream in(data);
	in.setVersion(QDataStream::Qt_4_5);

	int magic = 0;
	in >> magic;

	return false;
}

QMenu* ContainerWidget::createContextMenu() const
{
	QMenu* m = new QMenu(const_cast<ContainerWidget*>(this));

	// Contents of SectionWidgets
	for (int i = 0; i < _sections.size(); ++i)
	{
		SectionWidget* sw = _sections.at(i);
		QList<SectionContent::RefPtr> contents = sw->contents();
		foreach (const SectionContent::RefPtr& c, contents)
		{
			m->addAction(QIcon(), QString("Content %1").arg(c->uid()));
		}
	}

	// Contents of FloatingWidgets
	if (_floatingWidgets.size())
	{
		if (m->actions().size())
			m->addSeparator();
		for (int i = 0; i < _floatingWidgets.size(); ++i)
		{
			FloatingWidget* fw = _floatingWidgets.at(i);
			SectionContent::RefPtr c = fw->content();
			QAction* a = m->addAction(QIcon(), QString("Floating %1").arg(c->uid()));
			a->setCheckable(true);
			a->setChecked(fw->isVisible());
			QObject::connect(a, &QAction::toggled, fw, &FloatingWidget::setVisible);
		}
	}

	return m;
}

ADS_NAMESPACE_END
