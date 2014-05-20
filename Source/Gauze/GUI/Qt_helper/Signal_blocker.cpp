#include "Signal_blocker.hpp"
#include <QObject>

Signal_blocker::Signal_blocker(QObject* object) : object_(object)
{
	if (object_)
	{
		previous_value_ = object_->blockSignals(true);
	}
}

Signal_blocker::~Signal_blocker()
{
	if (object_)
	{
		object_->blockSignals(previous_value_);
	}

}
