#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if ( message.RST ) {
    isn_ = std::nullopt;
    reassembler_.set_error();
    return;
  }
  if ( message.SYN ) {
    isn_ = message.seqno;
  }
  if ( !isn_.has_value() ) {
    return;
  }
  uint64_t first_index = message.seqno.unwrap( Wrap32 { 0 }, reassembler_.count_bytes_pending() ) + message.SYN
                         - isn_.value().unwrap( Wrap32 { 0 }, 0 ) - 1;
  std::string payload = message.payload.empty() ? "" : message.payload;
  reassembler_.insert( first_index, payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage msg;
  msg.window_size = min( reassembler_.writer().available_capacity(), static_cast<size_t>( 65535 ) );
  if ( isn_.has_value() ) {
    msg.ackno
      = Wrap32::wrap( reassembler_.writer().bytes_pushed(), isn_.value() + 1 + reassembler_.writer().is_closed() );
  }
  if ( reassembler_.writer().has_error() || reassembler_.reader().has_error() ) {
    msg.RST = true;
  }
  return msg;
}
