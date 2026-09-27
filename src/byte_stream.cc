#include "byte_stream.hh"
#include <stdexcept>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), available_capacity_( capacity ), buf_( "" ) {}

void Writer::push( string data )
{
  if ( this->is_closed_ ) {
    return; // Ignore any push after the stream has been closed.
  }

  if ( data.length() > available_capacity_ ) {
    data = data.substr( 0, available_capacity_ );
  }

  if ( this->buf_.length() > MAX_LEN ) {
    this->buf_.erase( 0, read_pos_ );
    read_pos_ = 0;
  }
  if ( buf_.length() > MAX_LEN ) {
    set_error();
    throw runtime_error( "Buffer overflow: cannot push more data than MAX_LEN" );
  }
  this->buf_ += data;
  this->bytes_pushed_ += data.length();
  available_capacity_ -= data.length();
}

void Writer::close()
{
  this->is_closed_ = true;
}

bool Writer::is_closed() const
{
  return { this->is_closed_ }; // Your code here.
}

uint64_t Writer::available_capacity() const
{
  return { available_capacity_ }; // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return { bytes_pushed_ }; // Your code here.
}

string_view Reader::peek() const
{
  return { string_view { buf_ }.substr( read_pos_ ) }; // Your code here.
}

void Reader::pop( uint64_t len )
{
  if ( len > buf_.length() ) {
    set_error();
    throw runtime_error( "pop len bigger than buffered" );
  }

  read_pos_ += len;
  bytes_popped_ += len;
  available_capacity_ += len;
}

bool Reader::is_finished() const
{
  if ( is_closed_ && read_pos_ == buf_.length() )
    return true;
  return false;
}

uint64_t Reader::bytes_buffered() const
{
  return { buf_.length() - read_pos_ }; // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return { bytes_popped_ }; // Your code here.
}
