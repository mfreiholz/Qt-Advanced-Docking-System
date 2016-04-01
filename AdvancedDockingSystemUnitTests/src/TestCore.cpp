#include "TestCore.h"

#include "ads/API.h"
#include "ads/Serialization.h"

void TestCore::serialization()
{
	QList<QByteArray> datas;
	datas.append(QByteArray("Custom Data Here!!!"));
	datas.append(QByteArray("Even More..."));
	datas.append(QByteArray("lalalaalalalalalalal").toBase64());

	// Write some data.
	ADS_NS_SER::InMemoryWriter writer;
	for (int i = 0; i < datas.count(); ++i)
	{
		QVERIFY(writer.write(i + 1, datas.at(i)));
	}
	QVERIFY(writer.offsetsCount() == datas.count());
	const QByteArray writtenData = writer.toByteArray();
	QVERIFY(writtenData.size() > 0);

	// Read and validate written data.
	ADS_NS_SER::InMemoryReader reader(writtenData);
	QVERIFY(reader.initReadHeader());
	QVERIFY(reader.offsetsCount() == datas.count());
	for (int i = 0; i < datas.count(); ++i)
	{
		QByteArray readData;
		QVERIFY(reader.read(i + 1, readData));
		QVERIFY(readData == datas.at(i));
	}
}

QTEST_MAIN(TestCore)