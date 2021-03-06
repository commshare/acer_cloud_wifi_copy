// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: vplex_community_service_types.proto

#ifndef PROTOBUF_vplex_5fcommunity_5fservice_5ftypes_2eproto__INCLUDED
#define PROTOBUF_vplex_5fcommunity_5fservice_5ftypes_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace vplex {
namespace community {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_vplex_5fcommunity_5fservice_5ftypes_2eproto();
void protobuf_AssignDesc_vplex_5fcommunity_5fservice_5ftypes_2eproto();
void protobuf_ShutdownFile_vplex_5fcommunity_5fservice_5ftypes_2eproto();

class SessionInfo;

// ===================================================================

class SessionInfo : public ::google::protobuf::Message {
 public:
  SessionInfo();
  virtual ~SessionInfo();
  
  SessionInfo(const SessionInfo& from);
  
  inline SessionInfo& operator=(const SessionInfo& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const SessionInfo& default_instance();
  
  void Swap(SessionInfo* other);
  
  // implements Message ----------------------------------------------
  
  SessionInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SessionInfo& from);
  void MergeFrom(const SessionInfo& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required fixed64 sessionHandle = 1;
  inline bool has_sessionhandle() const;
  inline void clear_sessionhandle();
  static const int kSessionHandleFieldNumber = 1;
  inline ::google::protobuf::uint64 sessionhandle() const;
  inline void set_sessionhandle(::google::protobuf::uint64 value);
  
  // required bytes serviceTicket = 2;
  inline bool has_serviceticket() const;
  inline void clear_serviceticket();
  static const int kServiceTicketFieldNumber = 2;
  inline const ::std::string& serviceticket() const;
  inline void set_serviceticket(const ::std::string& value);
  inline void set_serviceticket(const char* value);
  inline void set_serviceticket(const void* value, size_t size);
  inline ::std::string* mutable_serviceticket();
  inline ::std::string* release_serviceticket();
  
  // @@protoc_insertion_point(class_scope:vplex.community.SessionInfo)
 private:
  inline void set_has_sessionhandle();
  inline void clear_has_sessionhandle();
  inline void set_has_serviceticket();
  inline void clear_has_serviceticket();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::uint64 sessionhandle_;
  ::std::string* serviceticket_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_vplex_5fcommunity_5fservice_5ftypes_2eproto();
  friend void protobuf_AssignDesc_vplex_5fcommunity_5fservice_5ftypes_2eproto();
  friend void protobuf_ShutdownFile_vplex_5fcommunity_5fservice_5ftypes_2eproto();
  
  void InitAsDefaultInstance();
  static SessionInfo* default_instance_;
};
// ===================================================================


// ===================================================================

// SessionInfo

// required fixed64 sessionHandle = 1;
inline bool SessionInfo::has_sessionhandle() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SessionInfo::set_has_sessionhandle() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SessionInfo::clear_has_sessionhandle() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SessionInfo::clear_sessionhandle() {
  sessionhandle_ = GOOGLE_ULONGLONG(0);
  clear_has_sessionhandle();
}
inline ::google::protobuf::uint64 SessionInfo::sessionhandle() const {
  return sessionhandle_;
}
inline void SessionInfo::set_sessionhandle(::google::protobuf::uint64 value) {
  set_has_sessionhandle();
  sessionhandle_ = value;
}

// required bytes serviceTicket = 2;
inline bool SessionInfo::has_serviceticket() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SessionInfo::set_has_serviceticket() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SessionInfo::clear_has_serviceticket() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SessionInfo::clear_serviceticket() {
  if (serviceticket_ != &::google::protobuf::internal::kEmptyString) {
    serviceticket_->clear();
  }
  clear_has_serviceticket();
}
inline const ::std::string& SessionInfo::serviceticket() const {
  return *serviceticket_;
}
inline void SessionInfo::set_serviceticket(const ::std::string& value) {
  set_has_serviceticket();
  if (serviceticket_ == &::google::protobuf::internal::kEmptyString) {
    serviceticket_ = new ::std::string;
  }
  serviceticket_->assign(value);
}
inline void SessionInfo::set_serviceticket(const char* value) {
  set_has_serviceticket();
  if (serviceticket_ == &::google::protobuf::internal::kEmptyString) {
    serviceticket_ = new ::std::string;
  }
  serviceticket_->assign(value);
}
inline void SessionInfo::set_serviceticket(const void* value, size_t size) {
  set_has_serviceticket();
  if (serviceticket_ == &::google::protobuf::internal::kEmptyString) {
    serviceticket_ = new ::std::string;
  }
  serviceticket_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* SessionInfo::mutable_serviceticket() {
  set_has_serviceticket();
  if (serviceticket_ == &::google::protobuf::internal::kEmptyString) {
    serviceticket_ = new ::std::string;
  }
  return serviceticket_;
}
inline ::std::string* SessionInfo::release_serviceticket() {
  clear_has_serviceticket();
  if (serviceticket_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = serviceticket_;
    serviceticket_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace community
}  // namespace vplex

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_vplex_5fcommunity_5fservice_5ftypes_2eproto__INCLUDED
