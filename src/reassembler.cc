#include "reassembler.hh"
#include "debug.hh"

#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  uint64_t bytes_pushed = output_.writer().bytes_pushed();
  uint64_t left = max( first_index, bytes_pushed );
  uint64_t right = min( first_index + data.size(), bytes_pushed + output_.writer().available_capacity() );

  // 检查left是否已经落在一个已有的map片段中
  auto it = pending_substrings_.upper_bound( left );
  if ( it != pending_substrings_.begin() ) {
    auto prev_it = prev( it );
    uint64_t covered_until = prev_it->first + prev_it->second.size();
    if ( covered_until > left ) {
      left = min( right, covered_until );
    }
  }
  uint64_t pre_index = left;

  for ( uint64_t i = left; i < right; i++ ) {
    if ( pending_substrings_.find( i ) != pending_substrings_.end() ) {
      pending_substrings_[pre_index] += data.substr( pre_index - first_index, i - pre_index );
      count_bytes_pending_ += i - pre_index;
      i += pending_substrings_[i].size() - 1;
      pre_index = i + 1;
    }
  }
  if ( pre_index < right ) {
    pending_substrings_[pre_index] += data.substr( pre_index - first_index, right - pre_index );
    count_bytes_pending_ += right - pre_index;
  }
  if ( is_last_substring ) {
    last_index_ = first_index + data.size();
  }
  while ( pending_substrings_.find( bytes_pushed ) != pending_substrings_.end() ) {
    output_.writer().push( pending_substrings_[bytes_pushed] );
    count_bytes_pending_ -= pending_substrings_[bytes_pushed].size();
    pending_substrings_.erase( bytes_pushed );
    bytes_pushed = output_.writer().bytes_pushed();
  }
  bytes_pushed = output_.writer().bytes_pushed();
  if ( bytes_pushed == last_index_ ) {
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{ return count_bytes_pending_; }
