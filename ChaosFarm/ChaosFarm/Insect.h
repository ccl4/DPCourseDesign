#pragma once
#include<map>
#include "Animal.h"
#include "Collection.h"
#include "List.h"
#include "Atmosphere.h"
#include<vector>
#include<math.h>


/*Insect*/
class Insect:public Animal
{
public:
	virtual ~Insect(){}
	static Insect* find_and_clone(vector<Abstract*>* abs_list, string* species)
	{
		map<string*, Insect*>::iterator it;
		
		for (it = prototype_.begin(); ; it++)
		{
			if (*(*it).first == *species)break;
			if (it == prototype_.end())
			{
				if (*(*it).first != *species)
				{
					return NULL;
				}
			}
		}
		Insect*temp = (*it).second->clone(abs_list, 1);
		return temp;
	}
	virtual void time_pass_by();
	virtual float get_reproduction_rate() = 0;
	virtual void when_atmosphere_changed();
	virtual string* get_species() = 0;
	virtual Living* mate_with(vector<Abstract*>* abs_list,Living* another) = 0;
	virtual const char* get_class_name(){ return "Insect"; }
protected:
	float reproduction_rate_;
	Insect(vector<Abstract*>* abs_list, int size, int max_age) :Animal(abs_list, size, max_age),reproduction_rate_(1) {}
	virtual Insect* clone(vector<Abstract*>* abs_list, int size) = 0;
	static void addPrototype(Insect* insect);
private:
	static map<string*, Insect*> prototype_;
};

inline void Insect::addPrototype(Insect * insect)
{
	prototype_.insert(map<string*, Insect*>::value_type(insect->get_species(), insect));
}
map<string*, Insect*> Insect::prototype_;

void Insect::time_pass_by()
{
	grow();
}

void Insect::when_atmosphere_changed()
{
	Atmosphere* atmosphere = Atmosphere::getInstance();
	if (atmosphere->get_weather_type() == WINDY)
	{
		reproduction_rate_ = 0.5;
		format_output("Insect::when_atmosphere_changed()", "low reproduction rate!");
	}
	if (atmosphere->get_weather_type() == RAINY)
	{
		reproduction_rate_ = 0.25;
		format_output("Insect::when_atmosphere_changed()", "low reproduction rate!");
	}
}


/*InsectGroup*/
class InsectGroup :public Collection
{
public:
	class Iterator :public FarmIterator
	{
	public:
		Iterator();
		virtual ~Iterator(){}
		friend class InsectGroup;
		Iterator(InsectGroup* group);
		Iterator(Iterator* another);
		virtual Object* value();
		virtual void turn_next();
		virtual void turn_previous();
		virtual void turn_first();
		virtual void turn_last();
		virtual bool has_next();
		virtual bool has_previous();
		virtual const char* get_class_name() { return "Iterator"; }
	};
	InsectGroup(string* species) :species_(species)
	{
		list_ = new List();
	}
	~InsectGroup()
	{
		delete species_;
		delete list_;
	}
	virtual int size();
	virtual bool is_empty();
	virtual void begin(FarmIterator& iterator);
	virtual void end(FarmIterator& iterator);
	void hatch(vector<Abstract*>* abs_list);
	virtual void add(Object* new_element);
	virtual void remove(FarmIterator& iterator);
	virtual const char* get_class_name(){ return "InsectGroup"; }
private:
	string* species_;
	List*list_;
};

int InsectGroup::size()
{
	return list_->size();
}
bool InsectGroup::is_empty()
{
	return list_->is_empty();
}
void InsectGroup::begin(FarmIterator& iterator)
{
	if (size() > 0)
	{
		Iterator begin(this);
		iterator = begin;
	}
	else
	{
		Iterator begin;
		iterator = begin;
	}
}
void InsectGroup::end(FarmIterator& iterator)
{
	if (size() > 0)
	{
		Iterator end(this);
		iterator = end;
	}
	else
	{
		Iterator end;
		iterator = end;
	}
}
void InsectGroup::hatch(vector<Abstract*>* abs_list)
{
	int number;
	if (list_->size() > 0)
	{
		Iterator* it = new Iterator();
		begin(*it);
		float group_reproduction_rate = 0;
		for (; it->has_next(); it->turn_next())
		{
			group_reproduction_rate = group_reproduction_rate + ((Insect*)it->value())->get_reproduction_rate();
		}
		group_reproduction_rate = group_reproduction_rate + ((Insect*)it->value())->get_reproduction_rate();
		float average_rate = group_reproduction_rate / (list_->size());
		number = (int)(sqrt(list_->size())*average_rate);
		delete it;
	}
	else
	{
		number = 100;
	}
	for (int i = 0; i < number; i++)
	{
		Insect*temp = Insect::find_and_clone(abs_list, species_);
		Node* new_element = new Node(temp);
		list_->add(new_element);
	}
}
void InsectGroup::add(Object* new_element)
{
	if (new_element != NULL)
	{
		Insect* temp = (Insect*)new_element;
		if (*species_ == *(temp->get_species()))
		{
			Node* new_node = new Node(temp);
			list_->add(new_node);
		}
	}
}
void InsectGroup::remove(FarmIterator& iterator)
{
	Iterator* temp = (Iterator*)&iterator;
	list_->erase((Node*)temp->current_node_);
}


/*InsectGroup::Iterator*/
InsectGroup::Iterator::Iterator()
{
	current_node_ = NULL;
}

InsectGroup::Iterator::Iterator(InsectGroup* group)
{
	Node* node = group->list_->head();
	if (node != NULL)
	{
		current_node_ = node;
	}
	else
	{
		current_node_ = NULL;
	}
}

InsectGroup::Iterator::Iterator(Iterator* another)
{
	if (another != NULL)
	{
		current_node_ = another->current_node_;
	}
	else
	{
		current_node_ = NULL;
	}
}

Object* InsectGroup::Iterator::value()
{
	if (current_node_ != NULL)
	{
		return ((Node*)current_node_)->get_value();
	}
	else
	{
		return NULL;
	}
}

void InsectGroup::Iterator::turn_next()
{
	if (has_next())
	{
		current_node_ = ((Node*)current_node_)->get_next();
	}
}
void InsectGroup::Iterator::turn_previous()
{
	if (has_previous())
	{
		current_node_ = ((Node*)current_node_)->get_previous();
	}
}
void InsectGroup::Iterator::turn_first()
{
	if (current_node_ != NULL)
	{
		Node* temp = (Node*)current_node_;
		for (; temp->get_previous() != NULL; temp = temp->get_previous());
		current_node_ = temp;
	}
}
void InsectGroup::Iterator::turn_last()
{
	if (current_node_ != NULL)
	{
		Node* temp = (Node*)current_node_;
		for (; temp->get_next() != NULL; temp = temp->get_next());
		current_node_ = temp;
	}
}
bool InsectGroup::Iterator::has_next()
{
	if (current_node_ != NULL)
	{
		if (((Node*)current_node_)->get_next() == NULL)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}
bool InsectGroup::Iterator::has_previous()
{
	if (current_node_ != NULL)
	{
		if (((Node*)current_node_)->get_previous() == NULL)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}