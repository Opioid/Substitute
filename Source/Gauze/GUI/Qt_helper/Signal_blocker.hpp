#pragma once

class QObject;

class Signal_blocker
{

public:

	Signal_blocker(QObject* object);
	~Signal_blocker();

private:

	QObject* object_;
	bool previous_value_;
};
