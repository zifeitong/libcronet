#include "cronet_http/upload_data_provider.h"

namespace cronet_http {

namespace {

UploadDataProvider* GetThis(Cronet_UploadDataProviderPtr self) {
  return static_cast<UploadDataProvider*>(
      Cronet_UploadDataProvider_GetClientContext(self));
}

}  // namespace

UploadDataProvider::UploadDataProvider() {
  Cronet_UploadDataProvider_SetClientContext(provider_.get(), this);
}

Cronet_UploadDataProviderPtr UploadDataProvider::GetPtr() const {
  return provider_.get();
}

/* static */ int64_t UploadDataProvider::GetLength(
    Cronet_UploadDataProviderPtr self) {
  return GetThis(self)->GetLength();
}

/* static */ void UploadDataProvider::Read(
    Cronet_UploadDataProviderPtr self,
    Cronet_UploadDataSinkPtr upload_data_sink, Cronet_BufferPtr buffer) {
  GetThis(self)->Read(upload_data_sink, buffer);
}

/* static */ void UploadDataProvider::Rewind(
    Cronet_UploadDataProviderPtr self,
    Cronet_UploadDataSinkPtr upload_data_sink) {
  GetThis(self)->Rewind(upload_data_sink);
}

/* static */ void UploadDataProvider::Close(Cronet_UploadDataProviderPtr self) {
  GetThis(self)->Close();
}

InMemoryUploadDataProvider::InMemoryUploadDataProvider(std::string data)
    : data_(std::move(data)) {}

int64_t InMemoryUploadDataProvider::GetLength() { return data_.size(); }

void InMemoryUploadDataProvider::Read(Cronet_UploadDataSinkPtr upload_data_sink,
                                      Cronet_BufferPtr buffer) {
  size_t bytes_read =
      std::min(data_.size() - position_, Cronet_Buffer_GetSize(buffer));
  std::copy_n(data_.data(), bytes_read,
              static_cast<char*>(Cronet_Buffer_GetData(buffer)));
  position_ += bytes_read;
  Cronet_UploadDataSink_OnReadSucceeded(upload_data_sink, bytes_read,
                                        /* final_chunk=*/false);
}

void InMemoryUploadDataProvider::Rewind(
    Cronet_UploadDataSinkPtr upload_data_sink) {
  position_ = 0;
  Cronet_UploadDataSink_OnRewindSucceeded(upload_data_sink);
}

void InMemoryUploadDataProvider::Close() {}

}  // namespace cronet_http
