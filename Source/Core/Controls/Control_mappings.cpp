#include "Control_mappings.hpp"

Control_mappings::Action::Action() : action_(nullptr), release_(nullptr), active_(false), just_activated_(false)
{}

Control_mappings::Action::Action(void (*action)(bool, int, float), void (*release)()) : action_(action), release_(release), active_(false), just_activated_(false)
{}

bool Control_mappings::Action::start(const platform::Input_signal& signal)
{
	if (active_)
	{
		return false;
	}

	signal_ = signal;
	active_ = just_activated_ = true;

	return true;
}

void Control_mappings::Action::stop(const platform::Input_signal& signal)
{
	if (signal_.id() != signal.id())
	{
		return;
	}

	if (release_)
	{
		release_();
	}

	active_ = false;
}

void Control_mappings::Action::stop()
{
	active_ = false;
}

bool Control_mappings::Action::process(float speed)
{
	if (active_ || just_activated_)
	{
		action_(just_activated_, signal_.data(), speed);
	}

	just_activated_ = false;

	if (!signal_.wait_for_release())
	{
		active_ = false;
	}

	return active_;
}

Control_mappings::Control_mappings() : current_action_map_(nullptr)
{}

Control_mappings::~Control_mappings()
{
	for (auto m = action_maps_.begin(); m != action_maps_.end(); ++m)
	{
		delete m->second;
	}
}

void Control_mappings::register_action(const std::string& name, void (*action)(bool justActivated, int data, float speed), void (*release)())
{
	registered_actions_[name] = Action(action, release);
}

bool Control_mappings::map_action(const std::string& map, const std::string& action, unsigned int signal)
{
	auto a = registered_actions_.find(action);

	if (registered_actions_.end() == a)
	{
		return false;
	}
	
	auto m = action_maps_.find(map);

	Action_map* im;
	
	if (action_maps_.end() == m)
	{
		 im = new Action_map;
		 action_maps_[map] = im;
	}
	else
	{
		im = action_maps_[map];
	}

	(*im)[signal] = &(a->second);
	
	return true;
}

void Control_mappings::set_current_map(const std::string& map)
{
	auto m = action_maps_.find(map);

	if (action_maps_.end() == m)
	{
		 current_action_map_ = new Action_map;
		 action_maps_[map] = current_action_map_;
	}
	else
	{
		current_action_map_ = action_maps_[map];
	}
}

void Control_mappings::insert(const platform::Input_signal& signal)
{
	if (!current_action_map_)
	{
		return;
	}

	Action_map::iterator i = current_action_map_->find(signal.id());

	if (i == current_action_map_->end())
	{
		return;
	}

	if (!signal.data())
	{
		i->second->stop(signal);
	}
	else if (i->second->start(signal))
	{
		active_actions_.push_back(i->second);
	}
}

void Control_mappings::process(float speed)
{
	std::list<Action*>::iterator i = active_actions_.begin();

	Action* map_changer = nullptr;

	while (i != active_actions_.end())
	{
		Action_map* current_action_map = current_action_map_;

		if (!(*i)->process(speed)) 
		{
			i = active_actions_.erase(i);
		}
		else
		{
			if (current_action_map != current_action_map_)
			{
				map_changer = *i;
			}

			++i;
		}
	}

	if (map_changer)
	{
		for (auto a : active_actions_)
		{
			if (a != map_changer)
			{
				a->stop();
			}
		}
	}
}
