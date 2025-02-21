#pragma once

#include "selfdrive/common/queue.h"
#include "selfdrive/loggerd/encoder/encoder.h"

#define BUF_IN_COUNT 7
#define BUF_OUT_COUNT 6

class V4LEncoder : public VideoEncoder {
public:
  V4LEncoder(const char* filename, CameraType type, int in_width, int in_height, int fps,
             int bitrate, cereal::EncodeIndex::Type codec, int out_width, int out_height, bool write) :
             VideoEncoder(filename, type, in_width, in_height, fps, bitrate, codec, out_width, out_height, write) { encoder_init(); }
  ~V4LEncoder();
  void encoder_init();
  int encode_frame(const uint8_t *y_ptr, const uint8_t *u_ptr, const uint8_t *v_ptr,
                   int in_width, int in_height, VisionIpcBufExtra *extra);
  void encoder_open(const char* path);
  void encoder_close();
private:
  int fd;

  bool is_open = false;
  int segment_num = -1;
  int counter = 0;

  SafeQueue<VisionIpcBufExtra> extras;

  static void dequeue_handler(V4LEncoder *e);
  std::thread dequeue_handler_thread;

  VisionBuf buf_in[BUF_IN_COUNT];
  VisionBuf buf_out[BUF_OUT_COUNT];
  SafeQueue<unsigned int> free_buf_in;
};
