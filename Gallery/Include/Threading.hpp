#pragma once

#include "Database.hpp"

enum struct Query_Type {
	FILTER_CATEGORIES,
	ASSIGNED_TAGS,
	UNASSIGN_TAG,
	FILTER_TAGS,
	DIRECTORIES,
	CATEGORIES,
	ASSIGN_TAG,
	FILES,
	TAGS,
	FILE_VIEW,
	ANY_QUERY,
	DATA_UPDATE,
	DATA_DELETION,
	DATA_INSERTION
};

class Database_Thread : public QThread {
	Q_OBJECT
public:
	QQueue<DB::Query> work_queue;
	QWaitCondition condition;
	QMutex semaphore;

	DB::Database* database;

	Database_Thread() :
		QThread(),
		database(new DB::Database())
	{};
	~Database_Thread() {
		delete database;
	};

	void run() override;

	void processWork(const DB::Query& work);
	void queueWork  (const DB::Query& work);
	void cancelWork (const DB::Query_Type& type);
signals:
	void tagResult      (const DB::Query_Id& request_id, const vector<DB::Tag>& data);
	void fileResult     (const DB::Query_Id& request_id, const vector<DB::File>& data);
	void categoryResult (const DB::Query_Id& request_id, const vector<DB::Category>& data);

	void fileViewResult (const DB::Query_Id& request_id, QListWidgetItem* data);

	void logMsg(const QString& message);
};

class Image_Thread : public QRunnable {
public:
	const QString file_path;
	unordered_map<string, CORE::Prop> args;
	function<QPixmap(const QPixmap&)> processImage;
	Image_Thread(const QString& file_path);

	void run() override;

signals:
	void result(const unordered_map<string, CORE::Prop>& args, const QIcon& icon);
};