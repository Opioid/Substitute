#pragma once

#include "Platform/Input_signal.hpp"
#include <map>
#include <list>
#include <string>

class Control_mappings_interface
{

public:

	virtual ~Control_mappings_interface()
	{}

	virtual void register_action(const std::string& name, void (*action)(bool just_activated, int data, float speed), void (*release)() = nullptr) = 0;
	virtual bool map_action(const std::string& map, const std::string& action, unsigned int signal) = 0;

	virtual void set_current_map(const std::string& map) = 0;
};

class Control_mappings : public Control_mappings_interface
{

public:

	Control_mappings();
	virtual ~Control_mappings();

	void register_action(const std::string& name, void (*action)(bool just_activated, int data, float speed), void (*release)() = nullptr);
	bool map_action(const std::string& map, const std::string& action, unsigned int signal);
	void set_current_map(const std::string& map);

	void insert(const platform::Input_signal& signal);
	void process(float speed);

	bool map_action(size_t map, const std::string& action, unsigned int signal_id);

	void set_current_action_map(size_t map);

private:

	class Action
	{

	public:	

		platform::Input_signal signal_;

		void (*action_)(bool just_activated, int data, float speed);
		void (*release_)();

	public:

		Action();
		Action(void (*action)(bool just_activated, int data, float speed), void (*release)() = nullptr);

		bool start(const platform::Input_signal& signal);
		void stop(const platform::Input_signal& signal);
		void stop();

		bool process(float speed);

	private:

		bool active_;
		bool just_activated_;
	};

	typedef std::map<unsigned int, Action*> Action_map;

	Action_map* current_action_map_;

	std::map<std::string, Action> registered_actions_;
	
	std::map<std::string, Action_map*> action_maps_;

	std::list<Action*> active_actions_;
};
