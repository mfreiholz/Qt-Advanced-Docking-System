#include "TestCore.h"

#include "ads/API.h"
#include "ads/Serialization.h"

void TestCore::serialization()
{
	QList<QByteArray> datas;
	datas.append(QByteArray("Custom Data Here!!!"));
	datas.append(QByteArray("Even More..."));
	datas.append(QByteArray("lalalaalalalalalalal").toBase64());

	// WRITE some data.
	ADS_NS_SER::InMemoryWriter writer;
	for (int i = 0; i < datas.count(); ++i)
	{
		QVERIFY(writer.write(ADS_NS_SER::ET_Custom + i, datas.at(i)));
	}

	// Type: SectionIndexData
	ADS_NS_SER::SectionIndexData sid;
	for (int i = 0; i < 1; ++i)
	{
		ADS_NS_SER::SectionEntity se;
		se.x = i;
		se.y = i;
		se.width = 100 + i;
		se.height = 100 + i;
		se.currentIndex = i;

		for (int j = 0; j < 1; ++j)
		{
			ADS_NS_SER::SectionContentEntity sce;
			sce.uniqueName = QString("uname-%1-%2").arg(i).arg(j);
			sce.preferredIndex = 8;
			sce.visible = true;
			se.sectionContents.append(sce);
			se.sectionContentsCount += 1;
		}

		sid.sections.append(se);
		sid.sectionsCount += 1;
	}
	QVERIFY(writer.write(sid));

	QVERIFY(writer.offsetsCount() == datas.count() + 1);
	const QByteArray writtenData = writer.toByteArray();
	QVERIFY(writtenData.size() > 0);

	// READ and validate written data.
	ADS_NS_SER::InMemoryReader reader(writtenData);
	QVERIFY(reader.initReadHeader());
	QVERIFY(reader.offsetsCount() == datas.count() + 1);
	for (int i = 0; i < datas.count(); ++i)
	{
		QByteArray readData;
		QVERIFY(reader.read(ADS_NS_SER::ET_Custom + i, readData));
		QVERIFY(readData == datas.at(i));
	}

	// Type: SectionIndexData
	ADS_NS_SER::SectionIndexData sidRead;
	QVERIFY(reader.read(sidRead));

	// TODO compare sidRead with sid
}

QTEST_MAIN(TestCore)