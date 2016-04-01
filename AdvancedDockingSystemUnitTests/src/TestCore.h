#ifndef TEST_CORE_H
#define TEST_CORE_H

#include <QtTest/QtTest>

class TestCore : public QObject
{
	Q_OBJECT

private slots:
	void serialization();
};

#endif