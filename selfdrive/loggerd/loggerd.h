#pragma once

#include <unistd.h>

#include <atomic>
#include <cassert>
#include <cerrno>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "cereal/messaging/messaging.h"
#include "cereal/services.h"
#include "cereal/visionipc/visionipc.h"
#include "cereal/visionipc/visionipc_client.h"
#include "selfdrive/camerad/cameras/camera_common.h"
#include "selfdrive/common/params.h"
#include "selfdrive/common/swaglog.h"
#include "selfdrive/common/timing.h"
#include "selfdrive/common/util.h"
#include "selfdrive/hardware/hw.h"

#include "selfdrive/loggerd/encoder/encoder.h"
#include "selfdrive/loggerd/logger.h"
#ifdef QCOM2
#include "selfdrive/loggerd/encoder/v4l_encoder.h"
#define Encoder V4LEncoder
#else
#include "selfdrive/loggerd/encoder/ffmpeg_encoder.h"
#define Encoder FfmpegEncoder
#endif

constexpr int MAIN_FPS = 20;
const int MAIN_BITRATE = Hardware::TICI() ? 10000000 : 5000000;
const int DCAM_BITRATE = Hardware::TICI() ? MAIN_BITRATE : 2500000;

#define NO_CAMERA_PATIENCE 500 // fall back to time-based rotation if all cameras are dead

const bool LOGGERD_TEST = getenv("LOGGERD_TEST");
const int SEGMENT_LENGTH = LOGGERD_TEST ? atoi(getenv("LOGGERD_SEGMENT_LENGTH")) : 60;

struct LogCameraInfo {
  CameraType type;
  const char *filename;
  VisionStreamType stream_type;
  int frame_width, frame_height;
  int fps;
  int bitrate;
  bool is_h265;
  bool has_qcamera;
  bool enable;
  bool record;
};

const LogCameraInfo cameras_logged[] = {
  {
    .type = RoadCam,
    .stream_type = VISION_STREAM_ROAD,
    .filename = "fcamera.hevc",
    .fps = MAIN_FPS,
    .bitrate = MAIN_BITRATE,
    .is_h265 = true,
    .has_qcamera = true,
    .enable = true,
    .record = true,
    .frame_width = 1928,
    .frame_height = 1208,
  },
  {
    .type = DriverCam,
    .stream_type = VISION_STREAM_DRIVER,
    .filename = "dcamera.hevc",
    .fps = MAIN_FPS,
    .bitrate = DCAM_BITRATE,
    .is_h265 = true,
    .has_qcamera = false,
    .enable = true,
    .record = Params().getBool("RecordFront"),
    .frame_width = 1928,
    .frame_height = 1208,
  },
  {
    .type = WideRoadCam,
    .stream_type = VISION_STREAM_WIDE_ROAD,
    .filename = "ecamera.hevc",
    .fps = MAIN_FPS,
    .bitrate = MAIN_BITRATE,
    .is_h265 = true,
    .has_qcamera = false,
    .enable = Hardware::TICI(),
    .record = Hardware::TICI(),
    .frame_width = 1928,
    .frame_height = 1208,
  },
};
const LogCameraInfo qcam_info = {
  .filename = "qcamera.ts",
  .fps = MAIN_FPS,
  .bitrate = 256000,
  .is_h265 = false,
  .enable = true,
  .record = true,
  .frame_width = Hardware::TICI() ? 526 : 480,
  .frame_height = Hardware::TICI() ? 330 : 360 // keep pixel count the same?
};
