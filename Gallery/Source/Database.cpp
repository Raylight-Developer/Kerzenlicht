#include "Database.hpp"

#undef FILE
#include "libpq-fe.h"

DB::Tag::Tag(char* name, char* category, char* description, char* filter_state, char* category_color, char* file_count) :
	name(*name != '\0' ? name : "DB ERROR"),
	category_name(*category != '\0' ? category : "Other"),
	description(*description != '\0' ? description : "DB ERROR"),

	category_color(*category_color != '\0' ? category_color : "255 0 0"),
	file_count(*file_count != '\0' ? stoull(file_count) : 0),
	filter_state(Qt::PartiallyChecked)
{
	if (*filter_state == '\0')
		this->filter_state = Qt::Unchecked;
	else if (filter_state[0] == 't')
		this->filter_state = Qt::Checked;
}

DB::Category::Category(char* name, char* color, char* description, char* tag_count) :
	name(*name != '\0' ? name : "DB ERROR"),
	color(*color != '\0' ? color : "255 0 0"),
	description(*description != '\0' ? description : "DB ERROR"),
	tag_count(*tag_count != '\0' ? stoull(tag_count) : 0)
{}

DB::Directory::Directory(char* name, char* description) :
	name(*name != '\0' ? name : "DB ERROR"),
	description(*description != '\0' ? description : "DB ERROR")
{}

DB::File::File(char* name, char* directory, char* display_name, char* date, char* modified) :
	name(*name != '\0' ? name : "DB ERROR"),
	directory(*directory != '\0' ? directory : "Local Storage"),
	display_name(*display_name != '\0' ? display_name : "DB ERROR"),

	date(*date ? date : "DB ERROR"),
	modified(*modified != '\0' ? (*modified == 't' ? true : false) : false)
{}

DB::Database::Database() {
	database = PQconnectdb("dbname=Gallery user=postgres password=123 host=localhost");
}

DB::Database::~Database() {
	PQfinish(database);
}

void DB::Database::init() {
	if (createDatabase()) {
		createDatabaseDefaults();
	}
	createFolders();
}

bool DB::Database::createDatabase() {
	if (PQstatus(database) == CONNECTION_BAD) {
		LOG ENDL ANSI_R << "Connection to database failed: " << PQerrorMessage(database) ANSI_RESET;
		PQfinish(database);
		return false;
	}
	else {
		PQexec(database, R"(
			CREATE TABLE IF NOT EXISTS directories (
				directory VARCHAR(64) PRIMARY KEY,
				description TEXT
			)
		)");
		PQexec(database, R"(
			CREATE TABLE IF NOT EXISTS files (
				file_name VARCHAR(128),
				directory VARCHAR(64),
				display_name VARCHAR(128),
				date_added TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
				user_info TEXT DEFAULT '',
				FOREIGN KEY (directory) REFERENCES directories(directory),
				PRIMARY KEY (file_name, directory),
				UNIQUE (file_name, directory)
			)
		)");
		PQexec(database, R"(
			CREATE TABLE IF NOT EXISTS categories (
				category_name VARCHAR(128) PRIMARY KEY,
				color VARCHAR(9) DEFAULT '255255255',
				description TEXT DEFAULT ''
			)
		)");
		PQexec(database, R"(
			CREATE TABLE IF NOT EXISTS tags (
				tag_name VARCHAR(128),
				category_name VARCHAR(128) DEFAULT 'Other',
				description TEXT DEFAULT '',
				FOREIGN KEY (category_name) REFERENCES categories(category_name),
				PRIMARY KEY (tag_name, category_name),
				UNIQUE (tag_name, category_name)
			)
		)");
		PQexec(database, R"(
			CREATE TABLE IF NOT EXISTS file_tags (
				file_name VARCHAR(128),
				directory VARCHAR(64),
				tag_name VARCHAR(128) DEFAULT 'None',
				category_name VARCHAR(128) DEFAULT 'Other',
				FOREIGN KEY (tag_name, category_name) REFERENCES tags(tag_name, category_name),
				FOREIGN KEY (file_name, directory) REFERENCES files(file_name, directory),
				FOREIGN KEY (category_name) REFERENCES categories(category_name),
				UNIQUE (file_name, directory, tag_name, category_name)
			)
		)");
		PQexec(database, R"(
			CREATE TABLE IF NOT EXISTS showroom (
				tag_name VARCHAR(128) DEFAULT 'None',
				category_name VARCHAR(128) DEFAULT 'Other',
				display_name VARCHAR(128),
				cover_file_name VARCHAR(128),
				transform VARCHAR(128) DEFAULT '0 0 0 1 1',
				format VARCHAR(1) DEFAULT '0',
				description TEXT DEFAULT '',
				FOREIGN KEY (tag_name, category_name) REFERENCES tags(tag_name, category_name),
				FOREIGN KEY (category_name) REFERENCES categories(category_name),
				UNIQUE (tag_name, category_name)
			)
		)");
	}
	return true;
}

void DB::Database::createDatabaseDefaults() {
	PQexec(database, R"(
		INSERT INTO directories
			(directory, description)
		VALUES
			('Safebooru'     , 'SFW Anime Board.'),
			('Danbooru'      , 'NSFW Anime Board.'),
			('Gelbooru'      , 'NSFW Anime Board.'),
			('Showroom'      , 'Covers for the Showroom.'),
			('Local Storage' , 'Local Drive content.')
		ON CONFLICT DO NOTHING;
	)");
	PQexec(database, R"(
		INSERT INTO categories
			(category_name, color, description)
		VALUES
			('Other'     , '240140210' , 'Default category.' ),
			('Source'    , '100240240' , 'Origin of the file.' ),
			('Rating'    , '17025080 ' , 'Content rating of the file. (15+, 18+, etc.)' ),
			('Score'     , '255255255' , 'User score.' ),
			('Artist'    , '255140140' , 'Name of the Artist(s).' ),
			('Copyright' , '200150255' , 'Copyright holder(s) of the content.' ),
			('Character' , '50 20070 ' , 'Character(s) depicted in the artwork.' ),
			('General'   , '0  155230' , 'Attributes describing the content.' ),
			('Meta'      , '255220100' , 'Attributes defining properties of the stored file.' )
		ON CONFLICT DO NOTHING;
	)");
	PQexec(database, R"(
		INSERT INTO tags
			(tag_name, category_name, description)
		VALUES
			('Safebooru'     , 'Source' , 'SFW Anime Board.'         ),
			('Danbooru'      , 'Source' , 'NSFW Anime Board.'        ),
			('Gelbooru'      , 'Source' , 'NSFW Anime Board.'        ),
			('Showroom'      , 'Source' , 'Covers for the Showroom.' ),
			('Local Storage' , 'Source' , 'Local Drive content.'     ),

			('General'       , 'Rating' , 'E'   ),
			('Sensitive'     , 'Rating' , '13+' ),
			('Questionable'  , 'Rating' , '15+' ),
			('Explicit'      , 'Rating' , '18+' ),

			('10' , 'Score'  , '10/10' ),
			('9 ' , 'Score'  , '09/10' ),
			('8 ' , 'Score'  , '08/10' ),
			('7 ' , 'Score'  , '07/10' ),
			('6 ' , 'Score'  , '06/10' ),
			('5 ' , 'Score'  , '05/10' ),
			('4 ' , 'Score'  , '04/10' ),
			('3 ' , 'Score'  , '03/10' ),
			('2 ' , 'Score'  , '02/10' ),
			('1 ' , 'Score'  , '01/10' )
		ON CONFLICT DO NOTHING;
	)");
}

void DB::Database::createFolders() {
	QString database_path = QSettings("Booru", "Database").value("database_path", "").toString();
	if (!fs::exists(database_path.toStdString())) {
		LOG ENDL << "Creating databse in: " << database_path.toStdString();
		QFileDialog* Dialog = new QFileDialog();
		Dialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::PicturesLocation));
		Dialog->setFileMode(QFileDialog::FileMode::Directory);
		Dialog->exec();
		database_path = Dialog->selectedFiles()[0];
	}
	vector<Directory> directories = queryDatabaseDirectories(R"(
		SELECT DISTINCT directory, description
		FROM directories
		ORDER BY directory ASC
	)");
	for (const Directory& directory : directories) {
		if (!fs::exists((database_path + "/" + directory.name).toStdString()))
			fs::create_directory((database_path + "/" + directory.name).toStdString());
		if (!fs::exists((database_path + "/" + directory.name + "/Files").toStdString()))
			fs::create_directory((database_path + "/" + directory.name + "/Files").toStdString());
		if (!fs::exists((database_path + "/" + directory.name + "/Previews").toStdString()))
			fs::create_directory((database_path + "/" + directory.name + "/Previews").toStdString());
		if (!fs::exists((database_path + "/" + directory.name + "/Json_Data").toStdString()))
			fs::create_directory((database_path + "/" + directory.name + "/Json_Data").toStdString());
		if (!fs::exists((database_path + "/" + directory.name + "/Modified_Files").toStdString()))
			fs::create_directory((database_path + "/" + directory.name + "/Modified_Files").toStdString());
		if (!fs::exists((database_path + "/" + directory.name + "/Modified_Previews").toStdString()))
			fs::create_directory((database_path + "/" + directory.name + "/Modified_Previews").toStdString());
		if (!fs::exists((database_path + "/" + directory.name + "/Modified_Data").toStdString()))
			fs::create_directory((database_path + "/" + directory.name + "/Modified_Data").toStdString());
	}
	QSettings("Booru", "Database").setValue("database_path", database_path);
}

vector<DB::Tag> DB::Database::queryDatabaseTags(const string& query) {
	PGresult* result = PQexec(database, query.c_str());
	vector<Tag> tags;
	if (PQresultStatus(result) != PGRES_TUPLES_OK) {
		LOG ENDL ANSI_R << "Tag Query execution failed: " << PQresultErrorMessage(result) ANSI_RESET;
		PQclear(result);
	}
	else {
		const int rows = PQntuples(result);
		for (int i = 0; i < rows; i++) {
			char* value = PQgetvalue(result, i, 0);
			if (*value != '\0') {
				tags.push_back(Tag(
					PQgetvalue(result, i, 0),
					PQgetvalue(result, i, 1),
					PQgetvalue(result, i, 2),
					PQgetvalue(result, i, 3),
					PQgetvalue(result, i, 4),
					PQgetvalue(result, i, 5))
				);
			}
		}
	}
	return tags;
}

vector<DB::File> DB::Database::queryDatabaseFiles(const string& query) {
	PGresult* result = PQexec(database, query.c_str());
	vector<File> files;
	if (PQresultStatus(result) != PGRES_TUPLES_OK) {
		LOG ENDL ANSI_R << "File Query execution failed: " << PQresultErrorMessage(result) ANSI_RESET;
		PQclear(result);
	}
	else {
		const int rows = PQntuples(result);
		for (int i = 0; i < rows; i++) {
			char* value = PQgetvalue(result, i, 0);
			if (*value != '\0') {
				files.push_back(File(
					PQgetvalue(result, i, 0),
					PQgetvalue(result, i, 1),
					PQgetvalue(result, i, 2),
					PQgetvalue(result, i, 3),
					PQgetvalue(result, i, 4))
				);
			}
		}
	}
	return files;
}

vector<DB::Category> DB::Database::queryDatabaseCategories(const string& query) {
	PGresult* result = PQexec(database, query.c_str());
	vector<Category> categories;

	if (PQresultStatus(result) != PGRES_TUPLES_OK) {
		LOG ENDL ANSI_R << "Category Query execution failed: " << PQresultErrorMessage(result) ANSI_RESET;
		PQclear(result);
	}
	else {
		int rows = PQntuples(result);
		for (int i = 0; i < rows; i++) {
			char* value = PQgetvalue(result, i, 0);
			if (*value != '\0') {
				categories.push_back(Category(
					PQgetvalue(result, i, 0),
					PQgetvalue(result, i, 1),
					PQgetvalue(result, i, 2),
					PQgetvalue(result, i, 3))
				);
			}
		}
	}
	return categories;
}

vector<DB::Directory> DB::Database::queryDatabaseDirectories(const string& query) {
	PGresult* result = PQexec(database, query.c_str());
	vector<Directory> directories;
	if (PQresultStatus(result) != PGRES_TUPLES_OK) {
		LOG ENDL ANSI_R << "Directory Query execution failed: " << PQresultErrorMessage(result) ANSI_RESET;
		PQclear(result);
	}
	else {
		int rows = PQntuples(result);
		for (int i = 0; i < rows; i++) {
			char* value = PQgetvalue(result, i, 0);
			if (*value != '\0') {
				directories.push_back(Directory(
					PQgetvalue(result, i, 0),
					PQgetvalue(result, i, 1))
				);
			}
		}
	}
	return directories;
}

vector<DB::File> DB::Database::query(const QUERY::View_Files& query) {
	if (query.matching.empty()) {
		return queryDatabaseFiles(R"(
SELECT DISTINCT
	files.file_name,
	files.directory,
	files.display_name,
	files.date,
	files.modified
FROM files ORDER BY files.date DESC)");
	}
	vector<File> files;

	return files;
}

DB::Query::Query(const Query_Type& type, const uint64& request_id) :
	request_id(Query_Id(type, request_id))
{}

DB::QUERY::View_Files::View_Files(const uint64& request_id) :
	Query(Query_Type::VIEW_FILES, request_id)
{}

DB::QUERY::View_Files::View_Files(const uint64& request_id, const vector<Tag>& matching) :
	Query(Query_Type::VIEW_FILES, request_id),
	matching(matching)
{}