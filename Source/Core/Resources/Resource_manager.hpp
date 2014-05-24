#pragma once

#include "Resource_provider.hpp"
#include "Resources/Handle.hpp"
#include "String/String.hpp"
#include "File/Stream.hpp"
#include "File/Virtual_file_system.hpp"
#include "File/File_stream.hpp"
#include "../Logging/Logging.hpp"

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <istream>

class Resource_manager 
{

	class Resource_store_base
	{

	public:

		Resource_store_base() : id_counter_(0) {}
		virtual ~Resource_store_base() {}

		uint32_t query_new_id() { return id_counter_; }
		uint32_t get_new_id() { return id_counter_++; }

		virtual bool release_resources() = 0;
		
		virtual void reassign_ids() = 0;

		virtual size_t get_resource_info(std::vector<std::string>& info) const = 0;

	protected:

		uint32_t id_counter_;
	};


	template<typename Type>
	class Resource_store : public Resource_store_base
	{

	public:

		Resource_store(const Resource_provider<Type>* provider) : provider_(provider)
		{}

		~Resource_store()
		{}

		virtual bool release_resources()
		{
			bool something_was_released = false;

			auto& resources = resource_handles_;

			auto r = resources.begin();

			while (r != resources.end())
			{
				if (r->second.reference_count() <= 1)
				{
					r = resources.erase(r);

					something_was_released = true;
				}
				else
				{
					++r;
				}
			}

			return something_was_released;
		}

		Handle<Type> find_resource(const std::string& name) const
		{
			auto r = resource_handles_.find(name);

			if (resource_handles_.end() == r)
			{
				return nullptr;
			}

			return r->second;	
		}

		void add_resource(Handle<Type>& resource, const std::string& lookup_name)
		{
			resource->set_manageable_id(get_new_id());

			// store the handle for future usage
			resource_handles_[lookup_name] = resource;

		}

		virtual void reassign_ids()
		{
			id_counter_ = 0;
  
			for (auto r : resource_handles_)
			{
				r.second->set_manageable_id(get_new_id());
			}
		}

		virtual size_t get_resource_info(std::vector<std::string>& info) const
		{
			info.push_back("");

			const size_t start = info.size() - 1;

			size_t num_bytes_total = 0;

			size_t c = 1;
			for (auto r = resource_handles_.begin(); r != resource_handles_.end(); ++r, ++c)
			{
				const size_t num_bytes = r->second->num_bytes();
				num_bytes_total += num_bytes;

				std::ostringstream stream;
				stream << "[" << c - 1 << "]" << (c <= 10 ? "  " : " ") << "\"" << r->first
					   << "\", " << string::format_byte_size(num_bytes) << " (" << r->second.reference_count() - 1 << ")";

				info.push_back(stream.str());
			}

			info[start] = provider_->name() + ", " + string::format_byte_size(num_bytes_total) + ":";

			return num_bytes_total;
		}

		const Resource_provider<Type>* provider() const
		{
			return provider_;
		}

	private:

		const Resource_provider<Type>* provider_;

		std::map<std::string, Handle<Type>> resource_handles_;
	};

public:

	void release();
	void cleanup();
	void reassign_resource_ids();

	template<typename Type>
	void cleanup()
	{
		auto store = stores_.find(Resource_provider<Type>::id());

		if (stores_.end() != store)
		{
			store->second.release_resources();
		}
	}

	template<typename Type>
	void register_resource_provider(const Resource_provider<Type>* provider)
	{
		if (provider)
		{
			stores_[Resource_provider<Type>::id()] = new Resource_store<Type>(provider);
		}
	}

	template<typename Type>
	const Resource_provider<Type>* provider() const
	{
		auto store = get_resource_store<Type>();

		if (!store)
		{
			return nullptr;
		}

		return store->provider();
	}

	template<typename Type>
	uint32_t get_unique_ID()
	{
		return stores_[Resource_provider<Type>::id()]->get_unique_resource_ID();
	}

	template<typename Type>
	Handle<Type> load(const std::string& name, Flags flags = Flags())
	{
		if (!name.length())
		{
			return nullptr;
		}

		Resource_store<Type>* store = get_resource_store<Type>();

		// a store/loader for this resource type was never registered
		if (!store) 
		{
			return nullptr;
		}

		// has this resource already been loaded?
		Handle<Type> resource = store->find_resource(name);

		// return it
		if (resource)
		{
			return resource;
		}

		std::string resolved_path;

		if (!virtual_file_system_.query_resolved_complete_path(resolved_path, name))
		{
			logging::error("Resource_manager::load<" + store->provider()->name() + ">(): could not open stream for \"" + name + "\".");
			return nullptr;
		}

		virtual_file_system_.push_mount(resolved_path);

		// create a new resource
		file::Input_stream stream(name);

		if (!stream)
		{
			virtual_file_system_.pop_mount();
			logging::error("Resource_manager::load<" + store->provider()->name() + ">(): could not open stream for \"" + name + "\".");
			return nullptr;
		}

		resource = store->provider()->load(stream, *this, flags);

		if (!resource)
		{
			virtual_file_system_.pop_mount();
			return nullptr;
		}

		store->add_resource(resource, name);

		virtual_file_system_.pop_mount();

		return resource;
	}

	template<typename Type>
	Handle<Type> clone(const Handle<Type>& original)
	{
		Resource_store<Type>* store = get_resource_store<Type>();

		// a store/loader for this resource type was never registered
		if (!store) 
		{
			return nullptr;
		}

		Handle<Type> resource = store->provider()->clone(original, *this);

		store->add_resource(resource, original->name() + string::to_string(store->query_new_id()));

		return resource;
	}

	template<typename Type>
	void update() const
	{
		auto* store = get_resource_store<Type>();

		if (!store)
		{
			return;
		}

		auto loader = store->provider_;

		const auto& resources = store->resource_handles_;

		for (auto r : resources)
		{
			loader->update(r->second);
		}
	}

private:

	template<typename Type>
	const Resource_store<Type>* get_resource_store() const
	{
		auto store_base = stores_.find(Resource_provider<Type>::id());

		if (stores_.end() == store_base)
		{
			return nullptr;
		}

		return static_cast<const Resource_store<Type>*>(store_base->second);
	}

	template<typename Type>
	Resource_store<Type>* get_resource_store()
	{
		auto store_base = stores_.find(Resource_provider<Type>::id());

		if (stores_.end() == store_base)
		{
			return nullptr;
		}

		return static_cast<Resource_store<Type>*>(store_base->second);
	}

public:

	file::Virtual_file_system& virtual_file_system();

	void get_resource_info(std::vector<std::string>& info, const std::string& type = "") const;

private:

	std::map<size_t, Resource_store_base*> stores_;

	file::Virtual_file_system virtual_file_system_;
};
