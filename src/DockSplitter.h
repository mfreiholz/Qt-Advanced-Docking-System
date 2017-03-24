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

/**
 * Splitter used internally instead of QSplitter
 */
class CDockSplitter : public QSplitter
{
	Q_OBJECT
public:
	using QSplitter::QSplitter;

	/**
	 * Prints debug info
	 */
	virtual ~CDockSplitter();
}; // class CDockSplitter

} // namespace ads

//---------------------------------------------------------------------------
#endif // DockSplitterH
