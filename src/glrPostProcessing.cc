#include "glrPostProcessing.hh"

#include <algorithm>

namespace glr
{
  void PostStack::add(PostPass pass)
  {
    this->p_postOrder.push_back(std::move(pass));
  }
  
  std::vector<PostPass> PostStack::getPasses()
  {
    return this->p_postOrder;
  }
  
  bool PostStack::isEmpty() const
  {
    return this->p_postOrder.empty();
  }
  
  void PostStack::clear()
  {
    this->p_postOrder.clear();
  }
}
