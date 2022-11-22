#ifndef XM_DATA_SOURCE_H
#define XM_DATA_SOURCE_H

#include "miniaudio.h"
#include "jar_xm.h"

typedef struct xm_data_source {
  ma_data_source_base base;
  float *buffer;
  size_t curFrame;  // Could possibly use jar-xm ctx properties instead
  size_t nFrames;
  jar_xm_context_t* jarxmctx;
} xm_data_source;

ma_result xm_data_source_init_file(const char* pFilePath, xm_data_source* pXMDS);
void xm_data_source_uninit(xm_data_source* pXMDS);


#ifdef XMDS_IMPLEMENTATION

int XMDS_SAMPLE_RATE = 48000;
int XMDS_CHANNELS = 2;

ma_result xm_data_source_read(
  ma_data_source* pDataSource,
  void* pFramesOut,
  ma_uint64 frameCount,
  ma_uint64* pFramesRead
) {
  xm_data_source* pXMDS = (xm_data_source*)pDataSource;

  if (pFramesRead != NULL) {
    *pFramesRead = 0;
  }
  if (frameCount == 0) {
    return MA_INVALID_ARGS;
  }
  if (pXMDS == NULL) {
    return MA_INVALID_ARGS;
  }

  ma_result result = MA_SUCCESS;
  ma_uint64 totalFramesRead = 0;

  size_t framesRemaining = pXMDS->nFrames - pXMDS->curFrame;
  size_t actualFramesToRead = ma_min(framesRemaining, frameCount);
  if (actualFramesToRead <= 0) {
    *pFramesRead = 0;
    return MA_AT_END;
  }

  jar_xm_generate_samples(pXMDS->jarxmctx, (float*)pFramesOut, actualFramesToRead);
  pXMDS->curFrame += actualFramesToRead;
  *pFramesRead = actualFramesToRead;

  return MA_SUCCESS;
}

static ma_result xm_data_source_get_data_format(
  ma_data_source* pDataSource,
  ma_format* pFormat,
  ma_uint32* pChannels,
  ma_uint32* pSampleRate,
  ma_channel* pChannelMap,
  size_t channelMapCap
) {
  xm_data_source* pXMDS = (xm_data_source*)pDataSource;

  if (pFormat != NULL) {
    *pFormat = ma_format_f32;
  }
  if (pChannels != NULL) {
    *pChannels = XMDS_CHANNELS;
  }
  if (pSampleRate != NULL) {
    *pSampleRate = XMDS_SAMPLE_RATE;
  }
  if (pChannelMap != NULL) {
    ma_channel_map_init_standard(ma_standard_channel_map_default, pChannelMap, channelMapCap, *pChannels);
  }
  if (pDataSource == NULL) {
    return MA_INVALID_OPERATION;
  }

  return MA_SUCCESS;
}

static ma_result xm_data_source_seek(ma_data_source* pDataSource, ma_uint64 frameIndex) {
  return MA_NOT_IMPLEMENTED;
}

static ma_result xm_data_source_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor) {
  if (pDataSource == NULL || pCursor == NULL) {
    return MA_INVALID_ARGS;
  }

  xm_data_source* pXMDS = (xm_data_source*) pDataSource;
  *pCursor = pXMDS->curFrame;
  return MA_SUCCESS;
}

static ma_result xm_data_source_get_length(ma_data_source* pDataSource, ma_uint64* pLength) {
  if (pLength == NULL || pDataSource == NULL) {
    return MA_INVALID_ARGS;
  }

  xm_data_source* pXMDS = (xm_data_source*) pDataSource;
  *pLength = pXMDS->nFrames;
  return MA_SUCCESS;
}

static ma_data_source_vtable g_xm_data_source_vtable = {
  xm_data_source_read,
  xm_data_source_seek,
  xm_data_source_get_data_format,
  xm_data_source_get_cursor,
  xm_data_source_get_length,
};

ma_result xm_data_source_init_file(
  const char* pFilePath,
  xm_data_source* pXMDS
) {
  ma_result result;
  ma_data_source_config dataSourceConfig;

  if (pXMDS == NULL) {
    return MA_INVALID_ARGS;
  }

  MA_ZERO_OBJECT(pXMDS);
  pXMDS->curFrame = 0;

  dataSourceConfig = ma_data_source_config_init();
  dataSourceConfig.vtable = &g_xm_data_source_vtable;

  result = ma_data_source_init(&dataSourceConfig, &pXMDS->base);
  if (result != MA_SUCCESS) {
    return result;
  }

  int jar_result = jar_xm_create_context_from_file(&pXMDS->jarxmctx, XMDS_SAMPLE_RATE, pFilePath);
  if (jar_result != 0) {
    return MA_ERROR;
  }
  jar_xm_set_max_loop_count(pXMDS->jarxmctx, 1);

  pXMDS->nFrames = jar_xm_get_remaining_samples(pXMDS->jarxmctx);

  return MA_SUCCESS;
}

void xm_data_source_uninit(xm_data_source* pXMDS) {
  jar_xm_free_context(pXMDS->jarxmctx);
  ma_data_source_uninit(&pXMDS->base);
}

#endif

#endif
