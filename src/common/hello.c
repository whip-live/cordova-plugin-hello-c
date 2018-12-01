//
//  hello.c
//  Cross-platform C functionality
//

#include "hello.h"
#include "GPMF_parser.h"
#include "GPMF_mp4reader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern void PrintGPMF(GPMF_stream *ms);

/*char* concat(const char* s1, const char* s2){
    char* result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}*/

char* c_hello(char* input) {
	int32_t ret = GPMF_OK;
	GPMF_stream metadata_stream, *ms = &metadata_stream;
	double metadatalength;
	uint32_t *payload = NULL; //buffer to store GPMF samples from the MP4.
	FILE *gps_data;

	// get file return data
	if (strcmp(input, "Error") == 0)
	{
		char* result = input;
		return result;
	}

	char* fsMP4[200], fsWrite[200];
	strcpy(fsMP4, input);
	strcpy(fsWrite, input);
	strcat(fsMP4, ".MP4");
	strcat(fsWrite, ".json");

	gps_data = fopen(fsWrite, "w");
	fprintf(gps_data,"[");
	size_t mp4 = OpenMP4Source(fsMP4, MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE);
//	size_t mp4 = OpenMP4SourceUDTA(argv[1]);  //Search for GPMF payload with MP4's udta 

	metadatalength = GetDuration(mp4);

	if (metadatalength > 0.0)
	{
		uint32_t index, payloads = GetNumberPayloads(mp4);
//		printf("found %.2fs of metadata, from %d payloads, within %s\n", metadatalength, payloads, argv[1]);

		for (index = 0; index < payloads; index++)
		{
			uint32_t payloadsize = GetPayloadSize(mp4, index);
			float in = 0.0, out = 0.0; //times
			payload = GetPayload(mp4, payload, index);
			if (payload == NULL)
				goto cleanup;

			ret = GetPayloadTime(mp4, index, &in, &out);
			if (ret != GPMF_OK)
				goto cleanup;

			ret = GPMF_Init(ms, payload, payloadsize);
			if (ret != GPMF_OK)
				goto cleanup;

			double gpsunum;
			if (GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPSU"), GPMF_RECURSE_LEVELS)) {
				uint32_t key = GPMF_Key(ms);
				uint32_t samples = GPMF_Repeat(ms);
				uint32_t elements = GPMF_ElementsInStruct(ms);
				GPMF_stream find_stream;
				char gpsu[17];
				char *data = (char *)GPMF_RawData(ms);
				memcpy(gpsu, data, 17);
				gpsu[16] = '\0';
				printf("GPSU %s\n", gpsu);
				gpsunum = strtod(gpsu,NULL);
				printf("GPSU %.3f\n", gpsunum);
			}
			if (GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPS5"), GPMF_RECURSE_LEVELS) || //GoPro Hero5/6/7 GPS
				GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPRI"), GPMF_RECURSE_LEVELS))   //GoPro Karma GPS
			{
				uint32_t key = GPMF_Key(ms);
				uint32_t samples = GPMF_Repeat(ms);
				uint32_t elements = GPMF_ElementsInStruct(ms);
				uint32_t buffersize = samples * elements * sizeof(double);
				GPMF_stream find_stream;
				double *ptr, *tmpbuffer = malloc(buffersize);
				char units[10][6] = { "" };
				uint32_t unit_samples = 1;

				//printf("MP4 Payload time %.3f to %.3f seconds\n", in, out);

				if (tmpbuffer && samples)
				{
					uint32_t i, j;

					//Search for any units to display
					GPMF_CopyState(ms, &find_stream);
					if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, GPMF_CURRENT_LEVEL) ||
						GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, GPMF_CURRENT_LEVEL))
					{
						char *data = (char *)GPMF_RawData(&find_stream);
						int ssize = GPMF_StructSize(&find_stream);
						unit_samples = GPMF_Repeat(&find_stream);

						for (i = 0; i < unit_samples; i++)
						{
							memcpy(units[i], data, ssize);
							units[i][ssize] = 0;
							data += ssize;
						}
					}

					//GPMF_FormattedData(ms, tmpbuffer, buffersize, 0, samples); // Output data in LittleEnd, but no scale
					GPMF_ScaledData(ms, tmpbuffer, buffersize, 0, samples, GPMF_TYPE_DOUBLE);  //Output scaled data as floats

					ptr = tmpbuffer;
					for (i = 0; i < samples; i++)
					{
						if (i > 0) 
						{
							gpsunum = gpsunum + 0.05;
						}
						if (i == 0 || i == 4 || i == 8 || i == 12 || i == 17) {
							if (index > 0) {
								fprintf(gps_data, ",");
							} else if (index == 0 && i > 0)
							{
								fprintf(gps_data, ",");
							}
							fprintf(gps_data, "[");
						}
						for (j = 0; j < elements; j++)
						{
							if (i == 0 || i == 4 || i == 8 || i== 12 || i == 17)
							{
								if (j == 4) {
									fprintf(gps_data, "%.3f", gpsunum);
									printf("%.3f", gpsunum);
									*ptr++;
								} else {
									fprintf(gps_data, "%f,", *ptr);
									printf("%f%s, ", *ptr++, units[j%unit_samples]);
								}
							}
							else
							{
								*ptr++;
							}
						}
						if (i == 0 || i == 4 || i == 8 || i== 12 || i == 17)
						{
							fprintf(gps_data, "]");
						}
					printf("\n");
					}
					free(tmpbuffer);
				}
			}
			GPMF_ResetState(ms);
			printf("\n");
		}
		fprintf(gps_data,"]");
		fclose(gps_data);

	cleanup:
		if (payload) FreePayload(payload); payload = NULL;
		CloseSource(mp4);
	}
	return fsWrite;
}

