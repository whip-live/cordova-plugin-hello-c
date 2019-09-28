//
//  hello.c
//  Cross-platform C functionality
//

#include "hello.h"
#include "GPMF_mp4reader.h"
#include "GPMF_parser.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __ANDROID__
#include <android/log.h>

#define LOG_TAG "GPMFPARSER"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#endif

typedef struct point
{
  char utc_date[17];
  double latitude;
  double longitude;
  double elevation;
  double speed;
  uint16_t accuracy;
  uint32_t offset;
} point;

extern void
PrintGPMF(GPMF_stream* stream);

void
write_point_to_file(FILE*, point*, int);

/*char* concat(const char* s1, const char* s2){
    char* result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}*/

char*
c_hello(char* input)
{
  int32_t ret = GPMF_OK;
  GPMF_stream gs_stream;
  double metadatalength;
  uint32_t* payload = NULL; // buffer to store GPMF samples from the MP4.
  FILE* fgps_data;

  LOGI("Starting to parse");

  char* output_file = NULL;

  char fsMP4[200];
  char* fsWrite = NULL;

  // we move 7 bytes forward to remove the 'file://' protocol
  strcpy(fsMP4, input + 7);

  // create output file name
  output_file = (char*)malloc(sizeof(char) * 200);
  strcpy(output_file, input);
  strcat(output_file, ".json");

  // remove the 'file://' protocol from the write file
  fsWrite = (char*)malloc(sizeof(char) * 200);
  strcpy(fsWrite, output_file + 7);

  LOGI("Read from file %s", fsMP4);
  LOGI("Write to file %s", fsWrite);

  // Start the json list
  fgps_data = fopen(fsWrite, "w");
  fprintf(fgps_data, "[");

  size_t mp4 = OpenMP4Source(fsMP4, MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE);

  LOGI("Mp4 source size %zu", mp4);
  if (mp4 == 0) {
    // TODO: replace with proper error handling
    return "GPM";
  }

  metadatalength = GetDuration(mp4);

  LOGI("Metadata length %f", metadatalength);

  if (metadatalength > 0.0) {
    uint32_t index, payloads = GetNumberPayloads(mp4);

    LOGI("Payloads number %u at index %u", payloads, index);

    uint32_t record_counter = 0;

    for (index = 0; index < payloads; index++) {
      uint32_t payloadsize = GetPayloadSize(mp4, index);

      LOGI("Process Payload %u with size %u", index, payloadsize);

      double in = 0.0, out = 0.0; // times
      payload = GetPayload(mp4, payload, index);
      if (payload == NULL)
        goto cleanup;

      ret = GetPayloadTime(mp4, index, &in, &out);
      if (ret != GPMF_OK)
        goto cleanup;

      ret = GPMF_Init(&gs_stream, payload, payloadsize);
      if (ret != GPMF_OK)
        goto cleanup;

      while (GPMF_OK == GPMF_FindNext(&gs_stream,
                                      STR2FOURCC("STRM"),
                                      GPMF_RECURSE_LEVELS)) {
        LOGI("Found Key STRM");

        GPMF_stream gpsu_stream;
        GPMF_stream gps5_stream;
        GPMF_stream gpsp_stream;
        GPMF_CopyState(&gs_stream, &gpsu_stream);
        GPMF_CopyState(&gs_stream, &gps5_stream);
        GPMF_CopyState(&gs_stream, &gpsp_stream);

        point pt;
        if (GPMF_OK == GPMF_FindNext(&gpsu_stream,
                                     STR2FOURCC("GPSU"),
                                     GPMF_RECURSE_LEVELS)) {
          char* data = (char*)GPMF_RawData(&gpsu_stream);
          uint32_t size = GPMF_RawDataSize(&gpsu_stream);
          if (size > 16) {
            LOGE("Buffer is not enough for utc date: %u", size);
            continue;
          }
          memcpy(pt.utc_date, data, size);
          pt.utc_date[16] = '\0';

          LOGI("> GPSU UTC Date Data %s", pt.utc_date);
        }

        if (GPMF_OK == GPMF_FindNext(&gpsp_stream,
                                     STR2FOURCC("GPSP"),
                                     GPMF_RECURSE_LEVELS)) {
          uint16_t* data = (uint16_t*)GPMF_RawData(&gpsp_stream);
          pt.accuracy = *data;

          LOGI("> GPSP Accuracy %u", pt.accuracy);
        }

        if (GPMF_OK == GPMF_FindNext(&gps5_stream,
                                     STR2FOURCC("GPS5"),
                                     GPMF_RECURSE_LEVELS)) {
          uint32_t number_of_samples = GPMF_Repeat(&gps5_stream);
          uint32_t elements = GPMF_ElementsInStruct(&gps5_stream);

          // if buffersize is 0 move to the next
          uint32_t buffersize = number_of_samples * elements * sizeof(double);
          if (buffersize == 0) {
            continue;
          }

          // Allocate a buffer where we are going to store GPSU5 data
          double* tmpbuffer = malloc(buffersize);

          // Output scaled data as floats
          GPMF_ScaledData(&gps5_stream,
                          tmpbuffer,
                          buffersize,
                          0,
                          number_of_samples,
                          GPMF_TYPE_DOUBLE);

          // Loop over GPS samples
          double* ptr = tmpbuffer;
          for (uint32_t i = 0; i < number_of_samples; i++) {
            pt.latitude = *ptr++;
            pt.longitude = *ptr++;
            pt.elevation = *ptr++;
            ptr++; // skip speed 2D
            pt.speed = *ptr++;

            pt.offset = i * 50; // offset to apply to the timestamp
            LOGI("> GPS5 for sample %u", i);
            LOGI("> > Lat %f, Lon %f", pt.latitude, pt.longitude);
            LOGI("> > Elevation %f", pt.elevation);
            LOGI("> > Speed %f", pt.speed);
            LOGI("> > Offset %u", pt.offset);

            // write row to file
            write_point_to_file(fgps_data, &pt, record_counter > 0);
            record_counter++;
          }
          free(tmpbuffer);
        }
      }
    }

  cleanup:
    LOGI("Close file");
    fprintf(fgps_data, "]");
    fclose(fgps_data);

    LOGI("Cleaning");
    if (payload)
      FreePayload(payload);
    payload = NULL;
    CloseSource(mp4);
  }

  LOGI("Exit");

  return output_file;
}

void
write_point_to_file(FILE* outfile, point* pt, int joined)
{
  LOGI("Write to file");
  if (joined) {
    LOGI("join");
    fprintf(outfile, ", ");
  }
  fprintf(outfile,
          "[%f, %f, %f, %f, \"%s\", %u]",
          pt->latitude,
          pt->longitude,
          pt->elevation,
          pt->speed,
          pt->utc_date,
          pt->offset);
}
