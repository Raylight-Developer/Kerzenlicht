#pragma once


#include "Shared.hpp"
#include "QT.hpp"

#undef FILE
#include "libpq-fe.h"

namespace DB {
	struct Directory {
		QString name;
		QString description;

		Directory() {}
		Directory(char* name, char* description);
	};

	struct File {
		QString name;
		QString directory;
		QString display_name;
		QString date;
		bool    modified;

		File() {}
		File(char* name, char* directory, char* display_name, char* date, char* modified);
	};

	struct Category {
		QString name;
		QString color;
		QString description;
		uint32  tag_count;

		Category() {}
		Category(char* name, char* color, char* description, char* tag_count);
		bool operator==(const Category& other) const {
			return (name == other.name && tag_count == other.tag_count);
		}
	};

	struct Tag {
		QString name;
		QString category_name;
		QString description;
		QString category_color;
		uint32  file_count;
		Qt::CheckState filter_state;

		Tag() {}
		Tag(char* name, char* category, char* description, char* filter_state, char* category_color, char* file_count);
		bool operator==(const Tag& other) const {
			return (name == other.name && category_name == other.category_name);
		}
	};

	enum struct Query_Type {
		TAG_ASSIGN,
		TAG_UNASSIGN,
		DATA_UPDATE,
		DATA_DELETE,
		DATA_INSERT,
		VIEW_FILES
	};

	struct Query_Id {
		Query_Type type;
		uint64 request_id;
		Query_Id(const Query_Type& type, const uint64& request_id) :
			type(type),
			request_id(request_id)
		{}
	};

	struct Query {
		Query_Id request_id;

		Query(const Query_Type& type, const uint64& request_id);
	};

	namespace QUERY {
		struct View_Files : Query {
			vector<Tag> matching;
			bool ascending;
			string sort;
			View_Files(const uint64& request_id);
			View_Files(const uint64& request_id, const vector<Tag>& matching);
		};
	}

	struct Database {
		PGconn* database;

		Database();
		~Database();

		void init();
		bool createDatabase();
		void createDatabaseDefaults();
		void createFolders();

		vector<Tag> queryDatabaseTags(const string& query);
		vector<File> queryDatabaseFiles(const string& query);
		vector<Category> queryDatabaseCategories(const string& query);
		vector<Directory> queryDatabaseDirectories(const string& query);

		vector<File> query(const QUERY::View_Files& query);
	};
}