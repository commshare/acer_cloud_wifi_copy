// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "vplex_shared_credential.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace vplex {
namespace sharedCredential {

namespace {

const ::google::protobuf::Descriptor* UserCredential_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  UserCredential_reflection_ = NULL;
const ::google::protobuf::Descriptor* DeviceCredential_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  DeviceCredential_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_vplex_5fshared_5fcredential_2eproto() {
  protobuf_AddDesc_vplex_5fshared_5fcredential_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "vplex_shared_credential.proto");
  GOOGLE_CHECK(file != NULL);
  UserCredential_descriptor_ = file->message_type(0);
  static const int UserCredential_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserCredential, user_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserCredential, ias_output_),
  };
  UserCredential_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      UserCredential_descriptor_,
      UserCredential::default_instance_,
      UserCredential_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserCredential, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserCredential, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(UserCredential));
  DeviceCredential_descriptor_ = file->message_type(1);
  static const int DeviceCredential_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DeviceCredential, device_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DeviceCredential, creds_clear_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DeviceCredential, creds_secret_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DeviceCredential, renewal_token_),
  };
  DeviceCredential_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      DeviceCredential_descriptor_,
      DeviceCredential::default_instance_,
      DeviceCredential_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DeviceCredential, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DeviceCredential, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(DeviceCredential));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_vplex_5fshared_5fcredential_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    UserCredential_descriptor_, &UserCredential::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    DeviceCredential_descriptor_, &DeviceCredential::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_vplex_5fshared_5fcredential_2eproto() {
  delete UserCredential::default_instance_;
  delete UserCredential_reflection_;
  delete DeviceCredential::default_instance_;
  delete DeviceCredential_reflection_;
}

void protobuf_AddDesc_vplex_5fshared_5fcredential_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\035vplex_shared_credential.proto\022\026vplex.s"
    "haredCredential\"7\n\016UserCredential\022\021\n\tuse"
    "r_name\030\001 \001(\t\022\022\n\nias_output\030\002 \001(\014\"g\n\020Devi"
    "ceCredential\022\021\n\tdevice_id\030\001 \001(\006\022\023\n\013creds"
    "_clear\030\002 \001(\014\022\024\n\014creds_secret\030\003 \001(\014\022\025\n\rre"
    "newal_token\030\004 \001(\014B(\n\017igware.vplex.pbB\025Sh"
    "aredCredentialTypes", 259);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "vplex_shared_credential.proto", &protobuf_RegisterTypes);
  UserCredential::default_instance_ = new UserCredential();
  DeviceCredential::default_instance_ = new DeviceCredential();
  UserCredential::default_instance_->InitAsDefaultInstance();
  DeviceCredential::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_vplex_5fshared_5fcredential_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_vplex_5fshared_5fcredential_2eproto {
  StaticDescriptorInitializer_vplex_5fshared_5fcredential_2eproto() {
    protobuf_AddDesc_vplex_5fshared_5fcredential_2eproto();
  }
} static_descriptor_initializer_vplex_5fshared_5fcredential_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int UserCredential::kUserNameFieldNumber;
const int UserCredential::kIasOutputFieldNumber;
#endif  // !_MSC_VER

UserCredential::UserCredential()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void UserCredential::InitAsDefaultInstance() {
}

UserCredential::UserCredential(const UserCredential& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void UserCredential::SharedCtor() {
  _cached_size_ = 0;
  user_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ias_output_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

UserCredential::~UserCredential() {
  SharedDtor();
}

void UserCredential::SharedDtor() {
  if (user_name_ != &::google::protobuf::internal::kEmptyString) {
    delete user_name_;
  }
  if (ias_output_ != &::google::protobuf::internal::kEmptyString) {
    delete ias_output_;
  }
  if (this != default_instance_) {
  }
}

void UserCredential::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* UserCredential::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return UserCredential_descriptor_;
}

const UserCredential& UserCredential::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_vplex_5fshared_5fcredential_2eproto();  return *default_instance_;
}

UserCredential* UserCredential::default_instance_ = NULL;

UserCredential* UserCredential::New() const {
  return new UserCredential;
}

void UserCredential::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_user_name()) {
      if (user_name_ != &::google::protobuf::internal::kEmptyString) {
        user_name_->clear();
      }
    }
    if (has_ias_output()) {
      if (ias_output_ != &::google::protobuf::internal::kEmptyString) {
        ias_output_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool UserCredential::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string user_name = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_user_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->user_name().data(), this->user_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_ias_output;
        break;
      }
      
      // optional bytes ias_output = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_ias_output:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_ias_output()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void UserCredential::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string user_name = 1;
  if (has_user_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->user_name().data(), this->user_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->user_name(), output);
  }
  
  // optional bytes ias_output = 2;
  if (has_ias_output()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      2, this->ias_output(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* UserCredential::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional string user_name = 1;
  if (has_user_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->user_name().data(), this->user_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->user_name(), target);
  }
  
  // optional bytes ias_output = 2;
  if (has_ias_output()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->ias_output(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int UserCredential::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string user_name = 1;
    if (has_user_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->user_name());
    }
    
    // optional bytes ias_output = 2;
    if (has_ias_output()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->ias_output());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void UserCredential::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const UserCredential* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const UserCredential*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void UserCredential::MergeFrom(const UserCredential& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_user_name()) {
      set_user_name(from.user_name());
    }
    if (from.has_ias_output()) {
      set_ias_output(from.ias_output());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void UserCredential::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void UserCredential::CopyFrom(const UserCredential& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool UserCredential::IsInitialized() const {
  
  return true;
}

void UserCredential::Swap(UserCredential* other) {
  if (other != this) {
    std::swap(user_name_, other->user_name_);
    std::swap(ias_output_, other->ias_output_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata UserCredential::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = UserCredential_descriptor_;
  metadata.reflection = UserCredential_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int DeviceCredential::kDeviceIdFieldNumber;
const int DeviceCredential::kCredsClearFieldNumber;
const int DeviceCredential::kCredsSecretFieldNumber;
const int DeviceCredential::kRenewalTokenFieldNumber;
#endif  // !_MSC_VER

DeviceCredential::DeviceCredential()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void DeviceCredential::InitAsDefaultInstance() {
}

DeviceCredential::DeviceCredential(const DeviceCredential& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void DeviceCredential::SharedCtor() {
  _cached_size_ = 0;
  device_id_ = GOOGLE_ULONGLONG(0);
  creds_clear_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  creds_secret_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  renewal_token_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

DeviceCredential::~DeviceCredential() {
  SharedDtor();
}

void DeviceCredential::SharedDtor() {
  if (creds_clear_ != &::google::protobuf::internal::kEmptyString) {
    delete creds_clear_;
  }
  if (creds_secret_ != &::google::protobuf::internal::kEmptyString) {
    delete creds_secret_;
  }
  if (renewal_token_ != &::google::protobuf::internal::kEmptyString) {
    delete renewal_token_;
  }
  if (this != default_instance_) {
  }
}

void DeviceCredential::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* DeviceCredential::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return DeviceCredential_descriptor_;
}

const DeviceCredential& DeviceCredential::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_vplex_5fshared_5fcredential_2eproto();  return *default_instance_;
}

DeviceCredential* DeviceCredential::default_instance_ = NULL;

DeviceCredential* DeviceCredential::New() const {
  return new DeviceCredential;
}

void DeviceCredential::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    device_id_ = GOOGLE_ULONGLONG(0);
    if (has_creds_clear()) {
      if (creds_clear_ != &::google::protobuf::internal::kEmptyString) {
        creds_clear_->clear();
      }
    }
    if (has_creds_secret()) {
      if (creds_secret_ != &::google::protobuf::internal::kEmptyString) {
        creds_secret_->clear();
      }
    }
    if (has_renewal_token()) {
      if (renewal_token_ != &::google::protobuf::internal::kEmptyString) {
        renewal_token_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool DeviceCredential::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional fixed64 device_id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, &device_id_)));
          set_has_device_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_creds_clear;
        break;
      }
      
      // optional bytes creds_clear = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_creds_clear:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_creds_clear()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_creds_secret;
        break;
      }
      
      // optional bytes creds_secret = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_creds_secret:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_creds_secret()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_renewal_token;
        break;
      }
      
      // optional bytes renewal_token = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_renewal_token:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_renewal_token()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void DeviceCredential::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional fixed64 device_id = 1;
  if (has_device_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(1, this->device_id(), output);
  }
  
  // optional bytes creds_clear = 2;
  if (has_creds_clear()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      2, this->creds_clear(), output);
  }
  
  // optional bytes creds_secret = 3;
  if (has_creds_secret()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      3, this->creds_secret(), output);
  }
  
  // optional bytes renewal_token = 4;
  if (has_renewal_token()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      4, this->renewal_token(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* DeviceCredential::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional fixed64 device_id = 1;
  if (has_device_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed64ToArray(1, this->device_id(), target);
  }
  
  // optional bytes creds_clear = 2;
  if (has_creds_clear()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->creds_clear(), target);
  }
  
  // optional bytes creds_secret = 3;
  if (has_creds_secret()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        3, this->creds_secret(), target);
  }
  
  // optional bytes renewal_token = 4;
  if (has_renewal_token()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        4, this->renewal_token(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int DeviceCredential::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional fixed64 device_id = 1;
    if (has_device_id()) {
      total_size += 1 + 8;
    }
    
    // optional bytes creds_clear = 2;
    if (has_creds_clear()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->creds_clear());
    }
    
    // optional bytes creds_secret = 3;
    if (has_creds_secret()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->creds_secret());
    }
    
    // optional bytes renewal_token = 4;
    if (has_renewal_token()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->renewal_token());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void DeviceCredential::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const DeviceCredential* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const DeviceCredential*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void DeviceCredential::MergeFrom(const DeviceCredential& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_device_id()) {
      set_device_id(from.device_id());
    }
    if (from.has_creds_clear()) {
      set_creds_clear(from.creds_clear());
    }
    if (from.has_creds_secret()) {
      set_creds_secret(from.creds_secret());
    }
    if (from.has_renewal_token()) {
      set_renewal_token(from.renewal_token());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void DeviceCredential::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DeviceCredential::CopyFrom(const DeviceCredential& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DeviceCredential::IsInitialized() const {
  
  return true;
}

void DeviceCredential::Swap(DeviceCredential* other) {
  if (other != this) {
    std::swap(device_id_, other->device_id_);
    std::swap(creds_clear_, other->creds_clear_);
    std::swap(creds_secret_, other->creds_secret_);
    std::swap(renewal_token_, other->renewal_token_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata DeviceCredential::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = DeviceCredential_descriptor_;
  metadata.reflection = DeviceCredential_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace sharedCredential
}  // namespace vplex

// @@protoc_insertion_point(global_scope)
