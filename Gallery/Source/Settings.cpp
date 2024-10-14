#include "Settings.hpp"

Settings::Settings(QWidget* parent) :
	GUI::Form(parent),
	parent(parent)
{
	GUI::Value_Input* database_path = new GUI::Value_Input();
	database_path->setPlaceholderText("Database Path");
	database_path->setText(qstr(SESSION("database_path").getStr()));

	GUI::Value_Input* danbooru_key = new GUI::Value_Input();
	danbooru_key->setPlaceholderText("Danbooru Key");
	danbooru_key->setText(qstr(SESSION("danbooru_key").getStr()));

	GUI::Value_Input* danbooru_user = new GUI::Value_Input();
	danbooru_user->setPlaceholderText("Danbooru User");
	danbooru_user->setText(qstr(SESSION("danbooru_user").getStr()));

	GUI::Text_Edit* danbooru_blacklist = new GUI::Text_Edit();
	danbooru_blacklist->setPlaceholderText("Danbooru Blacklist");
	danbooru_blacklist->setPlainText(qstr(SESSION("danbooru_blacklist").getStr()));

	auto safebooru_blacklist = new GUI::Text_Edit();
	safebooru_blacklist->setPlaceholderText("Safebooru Blacklist");
	safebooru_blacklist->setPlainText(qstr(SESSION("safebooru_blacklist").getStr()));

	auto gelbooru_blacklist = new GUI::Text_Edit();
	gelbooru_blacklist->setPlaceholderText("Gelbooru Blacklist");
	gelbooru_blacklist->setPlainText(qstr(SESSION("gelbooru_blacklist").getStr()));

	layout->addRow("Database:", database_path);
	layout->addRow("Danbooru API Key:", danbooru_key);
	layout->addRow("Danbooru Username:", danbooru_user);
	layout->addRow("Danbooru  Blacklist:", danbooru_blacklist);
	layout->addRow("Safebooru Blacklist:", safebooru_blacklist);
	layout->addRow("Gelbooru  Blacklist:", gelbooru_blacklist);

	connect(database_path, &GUI::Value_Input::textChanged, [this, database_path]() {
		SESSION("database_path") = str(database_path->text());
		LOG ENDL << "Change Database Path To: " << str(database_path->text()); FLUSH;
	});
	connect(danbooru_key, &GUI::Value_Input::textChanged, [this, danbooru_key]() {
		SESSION("danbooru_key") = str(danbooru_key->text());
		LOG ENDL << "Change Danbooru Key To: " << str(danbooru_key->text()); FLUSH;
	});
	connect(danbooru_user, &GUI::Value_Input::textChanged, [this, danbooru_user]() {
		SESSION("danbooru_user") = str(danbooru_user->text());
		LOG ENDL << "Change Danbooru User To: " << str(danbooru_user->text()); FLUSH;
	});
	connect(danbooru_blacklist, &GUI::Text_Edit::textChanged, [this, danbooru_blacklist]() {
		SESSION("danbooru_blacklist") = str(danbooru_blacklist->toPlainText());
		LOG ENDL << "Change Danbooru Blacklist To: " << str(danbooru_blacklist->toPlainText()); FLUSH;
	});
	connect(safebooru_blacklist, &GUI::Text_Edit::textChanged, [this, safebooru_blacklist]() {
		SESSION("safebooru_blacklist") = str(safebooru_blacklist->toPlainText());
		LOG ENDL << "Change Safebooru Blacklist To: " << str(safebooru_blacklist->toPlainText()); FLUSH;
	});
	connect(gelbooru_blacklist, &GUI::Text_Edit::textChanged, [this, gelbooru_blacklist]() {
		SESSION("gelbooru_blacklist") = str(gelbooru_blacklist->toPlainText());
		LOG ENDL << "Change Gelbooru Blacklist To: " << str(gelbooru_blacklist->toPlainText()); FLUSH;
	});
}