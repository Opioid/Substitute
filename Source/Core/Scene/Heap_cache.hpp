#pragma once

#include <cstdint>
#include <vector>

template<typename Type>
class Heap_cache
{

public:

    Heap_cache() : size_(0)
    {}

    ~Heap_cache()
    {
		for (auto i : vector_)
        {
            delete i;
        }
    }

    bool empty() const
    {
        return 0 == size_;
    }

    size_t size() const
    {
        return size_;
    }

    void clear()
    {
        size_ = 0;
    }

	Type* pop()
	{
		if (0 == size_)
		{
			return nullptr;
		}
		else
		{
			--size_;

			return vector_[size_];
		}
	}

    Type* add()
    {
        Type* instance;

		if (size_ >= vector_.size())
        {
            instance = new Type;

			vector_.push_back(instance);
        }
        else
        {
			instance = vector_[size_];
        }

        ++size_;

        return instance;
    }

    Type* operator[](size_t index) const
    {
		return vector_[index];
    }

    typename std::vector<Type*>::const_iterator begin() const
    {
		return vector_.begin();
    }

    typename std::vector<Type*>::const_iterator end() const
    {
		return vector_.begin() + size_;
    }

    typename std::vector<Type*>::iterator begin()
    {
		return vector_.begin();
    }

    typename std::vector<Type*>::iterator end()
    {
		return vector_.begin() + size_;
    }

    void copy(std::vector<Type*>& vector) const
    {
        vector.resize(size_);

        for (size_t i = 0; i < size_; ++i)
        {
			vector[i] = vector_[i];
        }
    }

private:

    size_t size_;

	std::vector<Type*> vector_;
};
