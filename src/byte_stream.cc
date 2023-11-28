#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  // Push data to stream, but only as much as available capacity allows.
  if ( data.empty() || available_capacity() == 0 ) {
    return;
  }
  auto const push_len = min( data.size(), available_capacity() );
  if ( push_len < data.size() ) {
    data = data.substr( 0, push_len );
  }
  data_queue_.push_back( std::move( data ) );
  data_view_.emplace_back( data_queue_.back().c_str(), push_len );
  bytes_pushed_ += push_len;
  bytes_buffered_ += push_len;
}

void Writer::close()
{
  is_closed_ = true;
}

void Writer::set_error()
{
  has_error_ = true;
}

bool Writer::is_closed() const
{
  return is_closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - bytes_buffered_;
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  if (data_view_.empty()) {
    return {};
  }
  return data_view_.front();
}

bool Reader::is_finished() const
{
  // Is the stream finished (closed and fully popped)?
  return is_closed_ && ( bytes_buffered_ == 0 );
}

bool Reader::has_error() const
{
  return has_error_;
}

void Reader::pop( uint64_t len )
{
  auto pop_len = min( len, bytes_buffered_ );
  while ( pop_len > 0 ) {
    auto sz_data_view = data_view_.front().size();
    if ( pop_len < sz_data_view ) {
      data_view_.front().remove_prefix( pop_len );
      bytes_buffered_ -= pop_len;
      bytes_poped_ += pop_len;
      return;
    }
    data_queue_.pop_front();
    data_view_.pop_front();
    pop_len -= sz_data_view;
    bytes_buffered_ -= sz_data_view;
    bytes_poped_ += sz_data_view;
  }
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_buffered_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_poped_;
}
