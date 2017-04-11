#ifndef DockSplitterH
#define DockSplitterH
//============================================================================
/// \file   DockSplitter.h
/// \author Uwe Kindler
/// \date   24.03.2017
/// \brief  Declaration of CDockSplitter
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QSplitter>

namespace ads
{
struct DockSplitterPrivate;

/**
 * Splitter used internally instead of QSplitter
 */
class CDockSplitter : public QSplitter
{
	Q_OBJECT
private:
	DockSplitterPrivate* d;
	friend class DockSplitterPrivate;

public:
	CDockSplitter(QWidget *parent = Q_NULLPTR);
	CDockSplitter(Qt::Orientation orientation, QWidget *parent = Q_NULLPTR);

	/**
	 * Prints debug info
	 */
	virtual ~CDockSplitter();

	/**
	 * Returns true, if any of the internal widgets is visible
	 */
	bool hasVisibleContent() const;
}; // class CDockSplitter

} // namespace ads

//---------------------------------------------------------------------------
#endif // DockSplitterH
