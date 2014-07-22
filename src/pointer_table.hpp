#ifndef _POINTER_TABLE_HPP
#define _POINTER_TABLE_HPP
#include <map>
#include <memory>
#include <vector>

#include "chunk_header.hpp"

#define PTR_TOP_BITS 40UL
#define PTR_TOP_SIZE (1UL << PTR_TOP_BITS)
#define TOP_BIT_MASK (~(PTR_TOP_BITS - 1UL))
#define TOP_BITS(x) (((std::size_t) x) >> (PTR_MID_BITS + CHUNK_BITS))

#define PTR_MID_BITS (64UL - PTR_TOP_BITS - CHUNK_BITS)
#define PTR_MID_SIZE (1 << PTR_MID_BITS)
#define MID_BITS(x)							\
  ((((std::size_t) x) << PTR_TOP_BITS) >> (PTR_TOP_BITS + CHUNK_BITS))

struct pointer_table
{
  std::shared_ptr<chunk_header>&
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
  typedef std::shared_ptr<chunk_header> shared_chunk;

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
