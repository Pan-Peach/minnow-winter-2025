#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{ return Wrap32 { (uint32_t)( ( n + zero_point.raw_value_ ) % ( 1ul << 32 ) ) }; }

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  const uint64_t MOD = UINT32_MAX + 1ull;
  uint64_t offset = ( raw_value_ - zero_point.raw_value_ ) % MOD;
  uint64_t base = ( checkpoint / MOD ) * MOD;
  uint64_t candidate = base + offset;
  if ( candidate > checkpoint && candidate - checkpoint > MOD / 2 && candidate >= MOD ) {
    candidate -= MOD;
  } else if ( candidate < checkpoint && checkpoint - candidate > MOD / 2 && candidate + MOD < UINT64_MAX ) {
    candidate += MOD;
  }
  return candidate;
}
