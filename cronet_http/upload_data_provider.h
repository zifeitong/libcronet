#ifndef CRONET_HTTP_UPLOAD_DATA_PROVIDER_H_
#define CRONET_HTTP_UPLOAD_DATA_PROVIDER_H_

#include <string>

#include "absl/base/thread_annotations.h"
#include "absl/synchronization/mutex.h"
#include "cronet_c.h"
#include "cronet_http/internal/memory.h"

namespace cronet_http {

class UploadDataProvider {
 public:
  UploadDataProvider();

  virtual ~UploadDataProvider() = default;

  Cronet_UploadDataProviderPtr GetPtr() const;

 private:
  // Implementation of Cronet_UploadDataProvider interface.
  virtual int64_t GetLength() = 0;
  static int64_t GetLength(Cronet_UploadDataProviderPtr self);

  virtual void Read(Cronet_UploadDataSinkPtr upload_data_sink,
                    Cronet_BufferPtr buffer) = 0;
  static void Read(Cronet_UploadDataProviderPtr self,
                   Cronet_UploadDataSinkPtr upload_data_sink,
                   Cronet_BufferPtr buffer);

  virtual void Rewind(Cronet_UploadDataSinkPtr upload_data_sink) = 0;
  static void Rewind(Cronet_UploadDataProviderPtr self,
                     Cronet_UploadDataSinkPtr upload_data_sink);

  virtual void Close() = 0;
  static void Close(Cronet_UploadDataProviderPtr self);

  MAKE_CRONET_C_UNIQUE_PTR_WITH(Cronet_UploadDataProvider, provider_,
                                UploadDataProvider::GetLength,
                                UploadDataProvider::Read,
                                UploadDataProvider::Rewind,
                                UploadDataProvider::Close);
};

class InMemoryUploadDataProvider : public UploadDataProvider {
 public:
  InMemoryUploadDataProvider(std::string data);

 private:
  int64_t GetLength() override;

  void Read(Cronet_UploadDataSinkPtr upload_data_sink,
            Cronet_BufferPtr buffer) override;

  void Rewind(Cronet_UploadDataSinkPtr upload_data_sink) override;

  void Close() override;

  std::string data_;
  size_t position_ = 0;
};

}  // namespace cronet_http

#endif  // CRONET_HTTP_UPLOAD_DATA_PROVIDER_H_
