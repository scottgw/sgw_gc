#ifndef _POINTER_TABLE_HPP
#define _POINTER_TABLE_HPP
#include <map>
#include <memory>
#include <vector>

#include "chunk.hpp"

struct pointer_table
{
  std::shared_ptr<chunk>&
  operator [] (void *ptr)
  {
    std::shared_ptr<inner_map> inner;
    auto it = map.find (TOP_BITS(ptr));
    if (it == map.end())
      {
	inner = std::make_shared<inner_map>();
	map [TOP_BITS(ptr)] = inner;
      }
    else
      {
	inner = it->second;
      }

    return (*inner)[MID_BITS(ptr)];
  }

  bool
  has (void *ptr)
  {
    auto it = map.find (TOP_BITS(ptr));
    if (it == map.end())
      {
	return false;
      }
    else
      {
	shared_chunk chk = (*it->second) [MID_BITS(ptr)];
	return !!chk;
      }
  }
  
private:
  typedef std::shared_ptr<chunk> shared_chunk;
  
  struct inner_map : public std::vector<shared_chunk>
  {
    inner_map ():
      std::vector<shared_chunk>(PTR_MID_SIZE, shared_chunk())
      {
      }
  };

private:
  std::map<std::size_t, std::shared_ptr<inner_map>> map;  

};

#endif
