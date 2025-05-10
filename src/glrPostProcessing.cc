#include "glrender/glrPostProcessing.hh"

#include <algorithm>

namespace glr
{
  void PostStack::add(PostPass pass)
  {
    this->postOrder.push_back(std::move(pass));
  }
  
  std::vector<PostPass> PostStack::getPasses()
  {
    return this->postOrder;
  }
  
  bool PostStack::isEmpty() const
  {
    return this->postOrder.empty();
  }
  
  void PostStack::clear()
  {
    this->postOrder.clear();
  }
}
