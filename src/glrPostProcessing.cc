#include "glrPostProcessing.hh"

#include <algorithm>

namespace GLRender
{
	void PostStack::add(std::shared_ptr<PostPass> pass)
	{
		this->p_postOrder.push_back(std::move(pass));
	}
	
	void PostStack::remove(std::shared_ptr<PostPass> const &pass)
	{
		this->p_postOrder.erase(std::find(this->p_postOrder.begin(), this->p_postOrder.end(), pass));
	}
	
	std::vector<std::shared_ptr<PostPass>> PostStack::getPasses()
	{
		return this->p_postOrder;
	}
	
	bool PostStack::empty() const
	{
		return this->p_postOrder.empty();
	}
}
